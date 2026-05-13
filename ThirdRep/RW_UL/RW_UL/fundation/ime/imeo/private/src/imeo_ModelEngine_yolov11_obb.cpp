#include"imeo_ModelEngine_yolov11_obb.hpp"

#include<fstream>
#include<memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace rw
{
	namespace imeo
	{
		ModelEngine_Yolov11_obb::ModelEngine_Yolov11_obb(const std::string& modelPath)
		{
			init(modelPath);
		}

		ModelEngine_Yolov11_obb::~ModelEngine_Yolov11_obb()
		{
			_output_tensors[0].release();
		}

		void ModelEngine_Yolov11_obb::preprocess(const cv::Mat& mat)
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

		void ModelEngine_Yolov11_obb::infer()
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
		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_obb::postProcess()
		{
			_cpu_output_buffer = _output_tensors[0].GetTensorMutableData<float>();
			std::vector<Detection> boxes;

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
					const float angle = det_output.at<float>(4 + _num_classes, i);
					Detection box;
					box.angle = angle;
					box.central_x = cx;
					box.central_y = cy;
					box.width = ow;
					box.height = oh;
					box.conf = score;
					box.class_id = class_id_point.y;
					boxes.push_back(box);
				}
			}
			//std::vector<Detection> nms_boxes = rotatedNMS(boxes, config.nms_threshold);
			std::vector<Detection> nms_boxes = rotatedNmsWithKeepClass(boxes, _config.conf_threshold, _config.nms_threshold, _config.classids_nms_together);

			auto result = convertDetectionToDetectionRectangleInfo(nms_boxes);

			return result;
		}

		void ModelEngine_Yolov11_obb::init(const std::string& engine_path)
		{
			_env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "yolo");
			Ort::SessionOptions options;
			OrtSessionOptionsAppendExecutionProvider_CUDA(options, 0);
			std::wstring path = stringToWString(engine_path);
			_session = Ort::Session(_env, path.c_str(), options);
			Ort::AllocatorWithDefaultOptions allocator;

			input_name = _session.GetInputNameAllocated(0, allocator).get();
			output_name = _session.GetOutputNameAllocated(0, allocator).get();

			_input_node_names.push_back(input_name.c_str());
			_output_node_names.push_back(output_name.c_str());

			auto input_shape = _session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			auto output_shape = _session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			_input_h = input_shape[2];
			_input_w = input_shape[3];
			_detection_attribute_size = output_shape[1];
			_num_detections = output_shape[2];
			_num_classes = output_shape[1] - 5;

			cv::Mat zero_mat = cv::Mat::zeros(_input_h, _input_w, CV_8UC3);
			preprocess(zero_mat);
			for (int i = 0; i < 10; i++) {
				this->infer();
			}
		}

		std::vector<ModelEngine_Yolov11_obb::Detection> ModelEngine_Yolov11_obb::rotatedNmsWithKeepClass(
			const std::vector<Detection>& dets, float confThreshold, float nmsThreshold,
			const std::vector<size_t>& needKeepClassids)
		{
			std::vector<int> nms_indices;
			std::set<size_t> keep_set(needKeepClassids.begin(), needKeepClassids.end());

			if (dets.empty()) return {};

			std::vector<int> keep_indices;
			std::map<int, std::vector<int>> class_to_indices;
			for (int i = 0; i < dets.size(); ++i) {
				if (dets[i].conf < confThreshold) continue;
				if (keep_set.count(dets[i].class_id)) {
					keep_indices.push_back(i);
				}
				else {
					class_to_indices[dets[i].class_id].push_back(i);
				}
			}

			if (!keep_indices.empty()) {
				std::vector<int> sorted_indices = keep_indices;
				std::sort(sorted_indices.begin(), sorted_indices.end(), [&](int a, int b) {
					return dets[a].conf > dets[b].conf;
					});
				std::vector<bool> suppressed(sorted_indices.size(), false);
				for (size_t i = 0; i < sorted_indices.size(); ++i) {
					if (suppressed[i]) continue;
					int idx_i = sorted_indices[i];
					nms_indices.push_back(idx_i);
					for (size_t j = i + 1; j < sorted_indices.size(); ++j) {
						if (suppressed[j]) continue;
						int idx_j = sorted_indices[j];
						if (rotatedIoU(dets[idx_i], dets[idx_j]) > nmsThreshold) {
							suppressed[j] = true;
						}
					}
				}
			}

			for (const auto& kv : class_to_indices) {
				const auto& indices = kv.second;
				if (indices.empty()) continue;
				std::vector<int> sorted_indices = indices;
				std::sort(sorted_indices.begin(), sorted_indices.end(), [&](int a, int b) {
					return dets[a].conf > dets[b].conf;
					});
				std::vector<bool> suppressed(sorted_indices.size(), false);
				for (size_t i = 0; i < sorted_indices.size(); ++i) {
					if (suppressed[i]) continue;
					int idx_i = sorted_indices[i];
					nms_indices.push_back(idx_i);
					for (size_t j = i + 1; j < sorted_indices.size(); ++j) {
						if (suppressed[j]) continue;
						int idx_j = sorted_indices[j];
						if (rotatedIoU(dets[idx_i], dets[idx_j]) > nmsThreshold) {
							suppressed[j] = true;
						}
					}
				}
			}

			std::vector<Detection> nms_result;
			nms_result.reserve(nms_indices.size());
			for (int idx : nms_indices) {
				nms_result.push_back(dets[idx]);
			}
			return nms_result;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_obb::convertDetectionToDetectionRectangleInfo(
			const std::vector<Detection>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			std::vector<Detection> postDections;
			if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::Resize)
			{
				postDections = convertWhenResize(detections);
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::LetterBox)
			{
				postDections = convertWhenLetterBox(detections);
			}
			else if (_config.imagePretreatmentPolicy == ImagePretreatmentPolicy::CenterCrop)
			{
				postDections = convertWhenCentralCrop(detections);
			}
			else
			{
				postDections = convertWhenResize(detections);
			}

			for (const auto& item : postDections)
			{
				DetectionRectangleInfo resultItem;

				float cx = item.central_x;
				float cy = item.central_y;
				float w = item.width;
				float h = item.height;
				float angle_rad = item.angle;

				float dx[4] = { -w / 2,  w / 2,  w / 2, -w / 2 };
				float dy[4] = { -h / 2, -h / 2,  h / 2,  h / 2 };

				std::pair<int, int> corners[4];
				for (int i = 0; i < 4; ++i) {
					float x_rot = dx[i] * std::cos(angle_rad) - dy[i] * std::sin(angle_rad);
					float y_rot = dx[i] * std::sin(angle_rad) + dy[i] * std::cos(angle_rad);
					corners[i].first = static_cast<int>(std::round(cx + x_rot));
					corners[i].second = static_cast<int>(std::round(cy + y_rot));
				}

				resultItem.leftTop = corners[0];
				resultItem.rightTop = corners[1];
				resultItem.rightBottom = corners[2];
				resultItem.leftBottom = corners[3];
				resultItem.center_x = static_cast<int>(std::round(cx));
				resultItem.center_y = static_cast<int>(std::round(cy));
				resultItem.width = static_cast<int>(std::round(w));
				resultItem.height = static_cast<int>(std::round(h));
				resultItem.area = static_cast<long>(std::round(w * h));
				resultItem.classId = static_cast<size_t>(item.class_id);
				resultItem.score = item.conf;

				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<ModelEngine_Yolov11_obb::Detection> ModelEngine_Yolov11_obb::convertWhenLetterBox(
			const std::vector<Detection>& detections)
		{
			std::vector<Detection> result;
			result.reserve(detections.size());
			const auto& params = _centerCropParams;
			float scale = _letterBoxScale;
			int dw = _letterBoxdw;
			int dh = _letterBoxdh;
			for (const auto& item : detections)
			{
				float x1 = (item.central_x - dw) / scale;
				float y1 = (item.central_y - dh) / scale;
				float x2 = (item.central_x + item.width - dw) / scale;
				float y2 = (item.central_y + item.height - dh) / scale;
				Detection resultItem;
				resultItem.central_x = x1;
				resultItem.central_y = y1;
				resultItem.width = x2 - x1;
				resultItem.height = y2 - y1;
				resultItem.angle = item.angle;
				resultItem.conf = item.conf;
				resultItem.class_id = item.class_id;
				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<ModelEngine_Yolov11_obb::Detection> ModelEngine_Yolov11_obb::convertWhenCentralCrop(
			const std::vector<Detection>& detections)
		{
			std::vector<Detection> result;
			result.reserve(detections.size());
			const auto& params = _centerCropParams;
			for (const auto& item : detections)
			{
				float x1 = item.central_x + params.crop_x - params.pad_left;
				float y1 = item.central_y + params.crop_y - params.pad_top;
				float x2 = item.central_x + item.width + params.crop_x - params.pad_left;
				float y2 = item.central_y + item.height + params.crop_y - params.pad_top;

				Detection resultItem;
				resultItem.central_x = (x1 + x2) / 2;
				resultItem.central_y = (y1 + y2) / 2;
				resultItem.width = x2 - x1;
				resultItem.height = y2 - y1;
				resultItem.angle = item.angle;
				resultItem.conf = item.conf;
				resultItem.class_id = item.class_id;
				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<ModelEngine_Yolov11_obb::Detection> ModelEngine_Yolov11_obb::convertWhenResize(
			const std::vector<Detection>& detections)
		{
			std::vector<Detection> result;
			result.reserve(detections.size());
			auto scaleX = _sourceWidth / static_cast<float>(_input_w);
			auto scaleY = _sourceHeight / static_cast<float>(_input_h);
			for (const auto& item : detections)
			{
				Detection resultItem;
				resultItem.width = item.width * scaleX;
				resultItem.height = item.height * scaleY;
				resultItem.central_x = item.central_x * scaleX;
				resultItem.central_y = item.central_y * scaleY;
				resultItem.angle = item.angle;
				resultItem.conf = item.conf;
				resultItem.class_id = item.class_id;
				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<ModelEngine_Yolov11_obb::Detection> ModelEngine_Yolov11_obb::rotatedNMS(
			const std::vector<ModelEngine_Yolov11_obb::Detection>& dets, double iouThreshold)
		{
			std::vector<ModelEngine_Yolov11_obb::Detection> result;
			if (dets.empty()) return result;

			// �����ŶȽ�������
			std::vector<size_t> idxs(dets.size());
			std::iota(idxs.begin(), idxs.end(), 0);
			std::sort(idxs.begin(), idxs.end(), [&](size_t i, size_t j) {
				return dets[i].conf > dets[j].conf;
				});

			std::vector<bool> suppressed(dets.size(), false);

			for (size_t i = 0; i < idxs.size(); ++i) {
				if (suppressed[idxs[i]]) continue;
				result.push_back(dets[idxs[i]]);
				for (size_t j = i + 1; j < idxs.size(); ++j) {
					if (suppressed[idxs[j]]) continue;
					if (rotatedIoU(dets[idxs[i]], dets[idxs[j]]) > iouThreshold) {
						suppressed[idxs[j]] = true;
					}
				}
			}
			return result;
		}

		double ModelEngine_Yolov11_obb::rotatedIoU(const ModelEngine_Yolov11_obb::Detection& a,
			const ModelEngine_Yolov11_obb::Detection& b)
		{
			cv::RotatedRect rect1 = toRotatedRect(a);
			cv::RotatedRect rect2 = toRotatedRect(b);

			std::vector<cv::Point2f> intersection;
			auto interType = cv::rotatedRectangleIntersection(rect1, rect2, intersection);

			if (interType == cv::INTERSECT_NONE || intersection.empty())
				return 0.0;

			double interArea = cv::contourArea(intersection);
			double area1 = rect1.size.area();
			double area2 = rect2.size.area();
			double iou = interArea / (area1 + area2 - interArea);
			return iou;
		}

		cv::RotatedRect ModelEngine_Yolov11_obb::toRotatedRect(const ModelEngine_Yolov11_obb::Detection& det)
		{
			return cv::RotatedRect(
				cv::Point2f(det.central_x + det.width / 2.0f, det.central_y + det.height / 2.0f), // center
				cv::Size2f(det.width, det.height),
				det.angle
			);
		}

		cv::Mat ModelEngine_Yolov11_obb::draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList)
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

		std::wstring ModelEngine_Yolov11_obb::stringToWString(const std::string& str)
		{
			size_t len = std::mbstowcs(nullptr, str.c_str(), 0);
			std::wstring wstr(len, L'\0');
			std::mbstowcs(&wstr[0], str.c_str(), len);
			return wstr;
		}
	}
}