#include "imet_ModelEngine_yolov11_seg.hpp"

#include"cuda_device_runtime_api.h"

#include<fstream>
#include<memory>

namespace rw {
	namespace imet {
		void ModelEngine_Yolov11_seg::preprocess(const cv::Mat& mat)
		{
			_sourceWidth = mat.cols;
			_sourceHeight = mat.rows;

			if (config.imagePretreatmentPolicy == ImagePretreatmentPolicy::Resize)
			{
				auto infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(input_w, input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(gpu_buffers[0], infer_image.data, input_w * input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
			else if (config.imagePretreatmentPolicy == ImagePretreatmentPolicy::LetterBox)
			{
				cv::Mat letterbox_image = PreProcess::letterbox(mat, input_w, input_h, config.letterBoxColor, _letterBoxScale, _letterBoxdw, _letterBoxdh);
				auto infer_image = cv::dnn::blobFromImage(letterbox_image, 1.f / 255.f, cv::Size(input_w, input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(gpu_buffers[0], infer_image.data, input_w * input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
			else if (config.imagePretreatmentPolicy == ImagePretreatmentPolicy::CenterCrop)
			{
				cv::Mat center_crop_image = PreProcess::centerCrop(mat, input_w, input_h, config.centerCropColor, &_centerCropParams);
				auto infer_image = cv::dnn::blobFromImage(center_crop_image, 1.f / 255.f, cv::Size(input_w, input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(gpu_buffers[0], infer_image.data, input_w * input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
			else
			{
				auto infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(input_w, input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(gpu_buffers[0], infer_image.data, input_w * input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
		}

		void ModelEngine_Yolov11_seg::infer()
		{
			this->context->setInputTensorAddress(engine->getIOTensorName(0), gpu_buffers[0]);
			this->context->setOutputTensorAddress(engine->getIOTensorName(1), gpu_buffers[1]);
			this->context->setOutputTensorAddress(engine->getIOTensorName(2), gpu_buffers[2]);
			this->context->enqueueV3(NULL);
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg::postProcess()
		{
			std::vector<DetectionSeg> output;
			(cudaMemcpy(cpu_output_buffer, gpu_buffers[1], num_detections * detection_attribute_size * sizeof(float), cudaMemcpyDeviceToHost));
			(cudaMemcpy(cpu_output_buffer2, gpu_buffers[2], num_detections * detection_attribute_size * sizeof(float), cudaMemcpyDeviceToHost));
			std::vector<cv::Rect> boxes;
			std::vector<int> class_ids;
			std::vector<float> confidences;

			const cv::Mat det_output(detection_attribute_size, num_detections, CV_32F, cpu_output_buffer);

			for (int i = 0; i < det_output.cols; ++i) {
				const  cv::Mat classes_scores = det_output.col(i).rowRange(4, 4 + num_classes);
				cv::Point class_id_point;
				double score;
				minMaxLoc(classes_scores, nullptr, &score, nullptr, &class_id_point);

				if (score > config.conf_threshold) {
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
				boxes, class_ids, confidences, config.conf_threshold, config.nms_threshold, config.classids_nms_together);

			for (int i = 0; i < nms_result.size(); i++)
			{
				DetectionSeg result;
				int idx = nms_result[i];
				result.class_id = class_ids[idx];
				result.conf = confidences[idx];
				result.bbox = boxes[idx];
				output.push_back(result);
			}

			auto result = convertToDetectionRectangleInfo(output);
			return result;
		}

		void ModelEngine_Yolov11_seg::init(const std::string& enginePath, nvinfer1::ILogger& logger)
		{
			std::ifstream engineStream(enginePath, std::ios::binary);
			engineStream.seekg(0, std::ios::end);
			const size_t modelSize = engineStream.tellg();
			engineStream.seekg(0, std::ios::beg);
			std::unique_ptr<char[]> engineData(new char[modelSize]);
			engineStream.read(engineData.get(), modelSize);
			engineStream.close();

			runtime = nvinfer1::createInferRuntime(logger);
			engine = runtime->deserializeCudaEngine(engineData.get(), modelSize);
			context = engine->createExecutionContext();

			input_h = engine->getTensorShape(engine->getIOTensorName(0)).d[2];
			input_w = engine->getTensorShape(engine->getIOTensorName(0)).d[3];
			detection_attribute_size = engine->getTensorShape(engine->getIOTensorName(1)).d[1];
			num_detections = engine->getTensorShape(engine->getIOTensorName(1)).d[2];
			maskCoefficientNum = 32;
			num_classes = detection_attribute_size - 4 - maskCoefficientNum;

			auto thirdOutputSize = engine->getTensorShape(engine->getIOTensorName(2)).d[1];
			auto thirdOutputSize2 = engine->getTensorShape(engine->getIOTensorName(2)).d[2];
			auto thirdOutputSize3 = engine->getTensorShape(engine->getIOTensorName(2)).d[3];

			cpu_output_buffer = new float[num_detections * detection_attribute_size];
			(cudaMalloc((void**)&gpu_buffers[0], 3 * input_w * input_h * sizeof(float)));

			(cudaMalloc((void**)&gpu_buffers[1], detection_attribute_size * num_detections * sizeof(float)));

			cpu_output_buffer2 = new float[thirdOutputSize * thirdOutputSize2 * thirdOutputSize3];
			(cudaMalloc((void**)&gpu_buffers[2], thirdOutputSize * thirdOutputSize2 * thirdOutputSize3 * sizeof(float)));

			for (int i = 0; i < 10; i++) {
				this->infer();
			}
			cudaDeviceSynchronize();
		}

		ModelEngine_Yolov11_seg::ModelEngine_Yolov11_seg(const std::string& modelPath,
			nvinfer1::ILogger& logger)
		{
			init(modelPath, logger);
		}

		ModelEngine_Yolov11_seg::~ModelEngine_Yolov11_seg()
		{
			for (int i = 0; i < 2; i++)
				(cudaFree(gpu_buffers[i]));
			delete[] cpu_output_buffer;
			delete[] cpu_output_buffer2;
			delete context;
			delete engine;
			delete runtime;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg::convertToDetectionRectangleInfo(const std::vector<DetectionSeg>& detections)
		{
			if (config.imagePretreatmentPolicy == ImagePretreatmentPolicy::Resize)
			{
				return convertWhenResize(detections);
			}
			else if (config.imagePretreatmentPolicy == ImagePretreatmentPolicy::LetterBox)
			{
				return convertWhenLetterBox(detections);
			}
			else if (config.imagePretreatmentPolicy == ImagePretreatmentPolicy::CenterCrop)
			{
				return convertWhenCentralCrop(detections);
			}
			else
			{
				return convertWhenResize(detections);
			}
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg::convertWhenResize(
			const std::vector<DetectionSeg>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());
			auto scaleX = _sourceWidth / static_cast<float>(input_w);
			auto scaleY = _sourceHeight / static_cast<float>(input_h);
			for (const auto& item : detections)
			{
				DetectionRectangleInfo resultItem;
				resultItem.width = item.bbox.width * scaleX;
				resultItem.height = item.bbox.height * scaleY;
				resultItem.leftTop.first = item.bbox.x * scaleX;
				resultItem.leftTop.second = item.bbox.y * scaleY;
				resultItem.rightTop.first = item.bbox.x * scaleX + item.bbox.width * scaleX;
				resultItem.rightTop.second = item.bbox.y * scaleY;
				resultItem.leftBottom.first = item.bbox.x * scaleX;
				resultItem.leftBottom.second = item.bbox.y * scaleY + item.bbox.height * scaleY;
				resultItem.rightBottom.first = item.bbox.x * scaleX + item.bbox.width * scaleX;
				resultItem.rightBottom.second = item.bbox.y * scaleY + item.bbox.height * scaleY;
				resultItem.center_x = item.bbox.x * scaleX + item.bbox.width * scaleX / 2;
				resultItem.center_y = item.bbox.y * scaleY + item.bbox.height * scaleY / 2;
				resultItem.area = item.bbox.width * scaleX * item.bbox.height * scaleY;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;
				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg::convertWhenCentralCrop(
			const std::vector<DetectionSeg>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			const auto& params = _centerCropParams;

			for (const auto& item : detections)
			{
				//Calculate the original coordinates
				float x1 = item.bbox.x + params.crop_x - params.pad_left;
				float y1 = item.bbox.y + params.crop_y - params.pad_top;
				float x2 = item.bbox.x + item.bbox.width + params.crop_x - params.pad_left;
				float y2 = item.bbox.y + item.bbox.height + params.crop_y - params.pad_top;

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

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg::convertWhenLetterBox(
			const std::vector<DetectionSeg>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			// letterBoxScale: 缩放系数，letterBoxdw/letterBoxdh: padding
			float scale = _letterBoxScale;
			int dw = _letterBoxdw;
			int dh = _letterBoxdh;

			for (const auto& item : detections)
			{
				DetectionRectangleInfo resultItem;

				// 反算到原图坐标
				float x1 = (item.bbox.x - dw) / scale;
				float y1 = (item.bbox.y - dh) / scale;
				float x2 = (item.bbox.x + item.bbox.width - dw) / scale;
				float y2 = (item.bbox.y + item.bbox.height - dh) / scale;

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

		cv::Mat ModelEngine_Yolov11_seg::draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList)
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
	}
}