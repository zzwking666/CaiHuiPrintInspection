#include"imet_ModelEngine_yolov11_det.hpp"

#include"cuda_device_runtime_api.h"

#include<fstream>
#include<memory>
#include <iomanip>
#include <sstream>

namespace rw
{
	namespace imet
	{
		ModelEngine_Yolov11_det::ModelEngine_Yolov11_det(const std::string& modelPath,
			nvinfer1::ILogger& logger)
		{
			init(modelPath, logger);
		}

		ModelEngine_Yolov11_det::~ModelEngine_Yolov11_det()
		{
			for (int i = 0; i < 2; i++)
				(cudaFree(_gpu_buffers[i]));
			delete[] _cpu_output_buffer;
			delete _context;
			delete _engine;
			delete _runtime;
		}

		void ModelEngine_Yolov11_det::init(std::string enginePath, nvinfer1::ILogger& logger)
		{
			std::ifstream engineStream(enginePath, std::ios::binary);
			engineStream.seekg(0, std::ios::end);
			const size_t modelSize = engineStream.tellg();
			engineStream.seekg(0, std::ios::beg);
			std::unique_ptr<char[]> engineData(new char[modelSize]);
			engineStream.read(engineData.get(), modelSize);
			engineStream.close();

			_runtime = nvinfer1::createInferRuntime(logger);
			_engine = _runtime->deserializeCudaEngine(engineData.get(), modelSize);
			_context = _engine->createExecutionContext();

			_input_h = _engine->getTensorShape(_engine->getIOTensorName(0)).d[2];
			_input_w = _engine->getTensorShape(_engine->getIOTensorName(0)).d[3];
			_detection_attribute_size = _engine->getTensorShape(_engine->getIOTensorName(1)).d[1];
			_num_detections = _engine->getTensorShape(_engine->getIOTensorName(1)).d[2];
			_num_classes = _detection_attribute_size - 4;

			_cpu_output_buffer = new float[_num_detections * _detection_attribute_size];
			(cudaMalloc((void**)&_gpu_buffers[0], 3 * _input_w * _input_h * sizeof(float)));

			(cudaMalloc((void**)&_gpu_buffers[1], _detection_attribute_size * _num_detections * sizeof(float)));

			for (int i = 0; i < 10; i++) {
				this->infer();
			}
			cudaDeviceSynchronize();
		}

		void ModelEngine_Yolov11_det::infer()
		{
			this->_context->setInputTensorAddress(_engine->getIOTensorName(0), _gpu_buffers[0]);
			this->_context->setOutputTensorAddress(_engine->getIOTensorName(1), _gpu_buffers[1]);
			this->_context->enqueueV3(NULL);
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_det::postProcess()
		{
			std::vector<Detection> output;
			(cudaMemcpy(_cpu_output_buffer, _gpu_buffers[1], _num_detections * _detection_attribute_size * sizeof(float), cudaMemcpyDeviceToHost));
			std::vector<cv::Rect> boxes;
			std::vector<int> class_ids;
			std::vector<float> confidences;

			const cv::Mat det_output(_detection_attribute_size, _num_detections, CV_32F, _cpu_output_buffer);

			for (int i = 0; i < det_output.cols; ++i) {
				const  cv::Mat classes_scores = det_output.col(i).rowRange(4, 4 + _num_classes);
				cv::Point class_id_point;
				double score;
				minMaxLoc(classes_scores, nullptr, &score, nullptr, &class_id_point);

				if (score > _config.conf_threshold) {
					const float cx = det_output.at<float>(0, i);
					const float cy = det_output.at<float>(1, i);
					const float ow = det_output.at<float>(2, i);
					const float oh = det_output.at<float>(3, i);
					cv::Rect box;
					box.x = static_cast<int>((cx - 0.5 * ow));
					box.y = static_cast<int>((cy - 0.5 * oh));
					box.width = static_cast<int>(ow);
					box.height = static_cast<int>(oh);

					boxes.push_back(box);
					class_ids.push_back(class_id_point.y);
					confidences.push_back(score);
				}
			}

			std::vector<int> nms_result = nmsWithKeepClass(
				boxes, class_ids, confidences, _config.conf_threshold, _config.nms_threshold, _config.classids_nms_together);

			for (int i = 0; i < nms_result.size(); i++)
			{
				Detection result;
				int idx = nms_result[i];
				result.class_id = class_ids[idx];
				result.conf = confidences[idx];
				result.rect = boxes[idx];
				output.push_back(result);
			}
			auto size = output.size();
			if (size == 0) {
				return {};
			}

			auto result = convertDetectionToDetectionRectangleInfo(output);

			return result;
		}

		cv::Mat ModelEngine_Yolov11_det::draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList)
		{
			cv::Mat result = mat.clone();
			ImagePainter::PainterConfig config;
			for (const auto& item : infoList)
			{
				std::ostringstream oss;
				oss << "classId:" << item.classId << " score:" << std::fixed << std::setprecision(2) << item.score;
				config.text = oss.str();
				ImagePainter::drawShapesOnSourceImg(result, item, config);
			}
			return result;
		}

		void ModelEngine_Yolov11_det::preprocess(const cv::Mat& mat)
		{
			_sourceWidth = mat.cols;
			_sourceHeight = mat.rows;

			if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::Resize)
			{
				auto infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(_gpu_buffers[0], infer_image.data, _input_w * _input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::LetterBox)
			{
				cv::Mat letterbox_image = PreProcess::letterbox(mat, _input_w, _input_h, _config.letterBoxColor, letterBoxScale, letterBoxdw, letterBoxdh);
				auto infer_image = cv::dnn::blobFromImage(letterbox_image, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(_gpu_buffers[0], infer_image.data, _input_w * _input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::CenterCrop)
			{
				cv::Mat center_crop_image = PreProcess::centerCrop(mat, _input_w, _input_h, _config.centerCropColor, &_centerCropParams);
				auto infer_image = cv::dnn::blobFromImage(center_crop_image, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(_gpu_buffers[0], infer_image.data, _input_w * _input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
			else
			{
				auto infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(_gpu_buffers[0], infer_image.data, _input_w * _input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_det::convertDetectionToDetectionRectangleInfo(
			const std::vector<Detection>& detections)
		{
			if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::Resize)
			{
				return convertWhenResize(detections);
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::LetterBox)
			{
				return convertWhenLetterBox(detections);
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::CenterCrop)
			{
				return convertWhenCentralCrop(detections);
			}
			else
			{
				return convertWhenResize(detections);
			}
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_det::convertWhenResize(
			const std::vector<Detection>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());
			auto scaleX = _sourceWidth / static_cast<float>(_input_w);
			auto scaleY = _sourceHeight / static_cast<float>(_input_h);
			for (const auto& item : detections)
			{
				DetectionRectangleInfo resultItem;
				resultItem.width = item.rect.width * scaleX;
				resultItem.height = item.rect.height * scaleY;
				resultItem.leftTop.first = item.rect.x * scaleX;
				resultItem.leftTop.second = item.rect.y * scaleY;
				resultItem.rightTop.first = item.rect.x * scaleX + item.rect.width * scaleX;
				resultItem.rightTop.second = item.rect.y * scaleY;
				resultItem.leftBottom.first = item.rect.x * scaleX;
				resultItem.leftBottom.second = item.rect.y * scaleY + item.rect.height * scaleY;
				resultItem.rightBottom.first = item.rect.x * scaleX + item.rect.width * scaleX;
				resultItem.rightBottom.second = item.rect.y * scaleY + item.rect.height * scaleY;
				resultItem.center_x = item.rect.x * scaleX + item.rect.width * scaleX / 2;
				resultItem.center_y = item.rect.y * scaleY + item.rect.height * scaleY / 2;
				resultItem.area = item.rect.width * scaleX * item.rect.height * scaleY;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;
				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_det::convertWhenLetterBox(
			const std::vector<Detection>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			float scale = letterBoxScale;
			int dw = letterBoxdw;
			int dh = letterBoxdh;

			for (const auto& item : detections)
			{
				DetectionRectangleInfo resultItem;

				float x1 = (item.rect.x - dw) / scale;
				float y1 = (item.rect.y - dh) / scale;
				float x2 = (item.rect.x + item.rect.width - dw) / scale;
				float y2 = (item.rect.y + item.rect.height - dh) / scale;

				resultItem.leftTop.first = x1;
				resultItem.leftTop.second = y1;
				resultItem.rightTop.first = x2;
				resultItem.rightTop.second = y1;
				resultItem.leftBottom.first = x1;
				resultItem.leftBottom.second = y2;
				resultItem.rightBottom.first = x2;
				resultItem.rightBottom.second = y2;
				resultItem.width = x2 - x1;
				resultItem.height = y2 - y1;
				resultItem.center_x = (x1 + x2) / 2;
				resultItem.center_y = (y1 + y2) / 2;
				resultItem.area = resultItem.width * resultItem.height;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;
				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_det::convertWhenCentralCrop(
			const std::vector<Detection>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			const auto& params = _centerCropParams;

			for (const auto& item : detections)
			{
				float x1 = item.rect.x + params.crop_x - params.pad_left;
				float y1 = item.rect.y + params.crop_y - params.pad_top;
				float x2 = item.rect.x + item.rect.width + params.crop_x - params.pad_left;
				float y2 = item.rect.y + item.rect.height + params.crop_y - params.pad_top;

				DetectionRectangleInfo resultItem;
				resultItem.leftTop.first = x1;
				resultItem.leftTop.second = y1;
				resultItem.rightTop.first = x2;
				resultItem.rightTop.second = y1;
				resultItem.leftBottom.first = x1;
				resultItem.leftBottom.second = y2;
				resultItem.rightBottom.first = x2;
				resultItem.rightBottom.second = y2;
				resultItem.width = x2 - x1;
				resultItem.height = y2 - y1;
				resultItem.center_x = (x1 + x2) / 2;
				resultItem.center_y = (y1 + y2) / 2;
				resultItem.area = resultItem.width * resultItem.height;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;
				result.push_back(resultItem);
			}
			return result;
		}
	}
}