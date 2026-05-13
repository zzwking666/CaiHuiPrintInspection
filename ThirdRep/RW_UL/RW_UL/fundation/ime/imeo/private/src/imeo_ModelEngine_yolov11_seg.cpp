#include"imeo_ModelEngine_yolov11_seg.hpp"

#include <cuda_runtime_api.h>

namespace rw
{
	namespace imeo
	{
		ModelEngine_Yolov11_Seg::ModelEngine_Yolov11_Seg(const std::string& modelPath)
		{
			init(modelPath);
		}

		ModelEngine_Yolov11_Seg::~ModelEngine_Yolov11_Seg()
		{
			_output_tensors[0].release();
		}

		void ModelEngine_Yolov11_Seg::preprocess(const cv::Mat& mat)
		{
			_sourceWidth = mat.cols;
			_sourceHeight = mat.rows;

			if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::Resize)
			{
				_infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::LetterBox)
			{
				_infer_image = PreProcess::letterbox(mat, _input_w, _input_h, _config.letterBoxColor, _letterBoxScale, _letterBoxdw, _letterBoxdh);
				_infer_image = cv::dnn::blobFromImage(_infer_image, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::CenterCrop)
			{
				_infer_image = PreProcess::centerCrop(mat, _input_w, _input_h, _config.centerCropColor, &_centerCropParams);
				_infer_image = cv::dnn::blobFromImage(_infer_image, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
			}
			else
			{
				_infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(_input_w, _input_h), cv::Scalar(0, 0, 0), true);
			}

			std::vector<int64_t>input_node_dims = { 1,3,_input_h,_input_h };
			auto input_size = 1 * 3 * _input_h * _input_w;
			auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
			_input_tensor = Ort::Value::CreateTensor(
				memory_info,
				(float*)_infer_image.data,
				input_size,
				input_node_dims.data(),
				input_node_dims.size()
			);
			_ort_inputs.clear();
			_ort_inputs.push_back(std::move(_input_tensor));
		}

		void ModelEngine_Yolov11_Seg::infer()
		{
			_output_tensors = _session.Run(
				Ort::RunOptions{ nullptr },
				(const char* const*)_input_node_names.data(),
				_ort_inputs.data(),
				_ort_inputs.size(),
				(const char* const*)_output_node_names.data(),
				_output_node_names.size()
			);
		}
		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_Seg::postProcess()
		{
			std::vector<Detection> output;

			_cpu_output_buffer = _output_tensors[0].GetTensorMutableData<float>();
			std::vector<cv::Rect> boxes;
			std::vector<int> class_ids;
			std::vector<float> confidences;

			const cv::Mat det_output(_detection_attribute_size, _num_detections, CV_32F, _cpu_output_buffer);

			for (int i = 0; i < det_output.cols; ++i) {
				const cv::Mat classes_scores = det_output.col(i).rowRange(4, 4 + _num_classes);
				cv::Point class_id_point;
				double score;
				cv::minMaxLoc(classes_scores, nullptr, &score, nullptr, &class_id_point);

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
				result.bbox = boxes[idx];
				output.push_back(result);
			}

			return convertDetectionToDetectionRectangleInfo(output);
		}

		void ModelEngine_Yolov11_Seg::init(const std::string& engine_path)
		{
			_env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "yolo");
			Ort::SessionOptions options;
			OrtSessionOptionsAppendExecutionProvider_CUDA(options, 0);
			std::wstring path = stringToWString(engine_path);
			_session = Ort::Session(_env, path.c_str(), options);
			Ort::AllocatorWithDefaultOptions allocator;

			input_name = _session.GetInputNameAllocated(0, allocator).get();
			output_name = _session.GetOutputNameAllocated(0, allocator).get();
			output_name1 = _session.GetOutputNameAllocated(1, allocator).get();

			for (int i = 0; i < _session.GetInputCount(); ++i)
			{
				_session.GetInputNameAllocated(i, allocator);
			}

			_input_node_names.push_back(input_name.c_str());
			_output_node_names.push_back(output_name.c_str());
			_output_node_names.push_back(output_name1.c_str());

			auto input_shape = _session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			auto output_shape = _session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			_input_h = input_shape[2];
			_input_w = input_shape[3];
			_detection_attribute_size = output_shape[1];
			_num_detections = output_shape[2];
			_maskCoefficientNum = 32;
			_num_classes = output_shape[1] - 4 - _maskCoefficientNum;

			cv::Mat zero_mat = cv::Mat::zeros(_input_h, _input_w, CV_8UC3);
			preprocess(zero_mat);
			for (int i = 0; i < 10; i++) {
				this->infer();
			}
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_Seg::convertDetectionToDetectionRectangleInfo(
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

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_Seg::convertWhenResize(
			const std::vector<Detection>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());
			auto scaleX = _sourceWidth / static_cast<float>(_input_w);
			auto scaleY = _sourceHeight / static_cast<float>(_input_h);
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

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_Seg::convertWhenLetterBox(
			const std::vector<Detection>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			// letterBoxScale: scale , letterBoxdw/letterBoxdh: padding
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

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_Seg::convertWhenCentralCrop(
			const std::vector<Detection>& detections)
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

		cv::Mat ModelEngine_Yolov11_Seg::draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList)
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

		std::wstring ModelEngine_Yolov11_Seg::stringToWString(const std::string& str)
		{
			size_t len = std::mbstowcs(nullptr, str.c_str(), 0);
			std::wstring wstr(len, L'\0');
			std::mbstowcs(&wstr[0], str.c_str(), len);
			return wstr;
		}
	}
}