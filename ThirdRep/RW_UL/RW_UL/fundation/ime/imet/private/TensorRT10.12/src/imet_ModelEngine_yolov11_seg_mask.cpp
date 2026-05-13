#include "imet_ModelEngine_yolov11_seg_mask.hpp"

#include"cuda_device_runtime_api.h"

#include<fstream>
#include<memory>

namespace rw {
	namespace imet {
		void ModelEngine_Yolov11_seg_mask::preprocess(const cv::Mat& mat)
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
				/*cv::Mat center_crop_image = PreProcess::centerCrop(mat, input_w, input_h, config.centerCropColor, &_centerCropParams);
				auto infer_image = cv::dnn::blobFromImage(center_crop_image, 1.f / 255.f, cv::Size(input_w, input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(gpu_buffers[0], infer_image.data, input_w * input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			*/
			}
			else
			{
				auto infer_image = cv::dnn::blobFromImage(mat, 1.f / 255.f, cv::Size(input_w, input_h), cv::Scalar(0, 0, 0), true);
				(cudaMemcpy(gpu_buffers[0], infer_image.data, input_w * input_h * mat.channels() * sizeof(float), cudaMemcpyHostToDevice));
			}
		}

		void ModelEngine_Yolov11_seg_mask::infer()
		{
			this->context->setInputTensorAddress(engine->getIOTensorName(0), gpu_buffers[0]);
			this->context->setOutputTensorAddress(engine->getIOTensorName(1), gpu_buffers[1]);
			this->context->setOutputTensorAddress(engine->getIOTensorName(2), gpu_buffers[2]);
			this->context->enqueueV3(NULL);
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg_mask::postProcess()
		{
			std::vector<DetectionSeg> output;

			// 从 GPU 拷贝检测结果和掩膜原型到 CPU
			(cudaMemcpy(cpu_output_buffer, gpu_buffers[1], num_detections * detection_attribute_size * sizeof(float), cudaMemcpyDeviceToHost));
			(cudaMemcpy(cpu_output_buffer2, gpu_buffers[2], maskCoefficientNum * mask_h * mask_w * sizeof(float), cudaMemcpyDeviceToHost));

			std::vector<cv::Rect> boxes;
			std::vector<int> class_ids;
			std::vector<float> confidences;
			std::vector<cv::Mat> mask_sigmoids;

			// 将检测结果和掩膜原型转换为 OpenCV 矩阵
			const cv::Mat det_output(detection_attribute_size, num_detections, CV_32F, cpu_output_buffer);
			cv::Mat mask_protos(maskCoefficientNum, mask_h * mask_w, CV_32F, cpu_output_buffer2);

			for (int i = 0; i < det_output.cols; ++i) {
				// 提取类别分数
				const cv::Mat classes_scores = det_output.col(i).rowRange(4, 4 + num_classes);
				cv::Point class_id_point;
				double score;
				minMaxLoc(classes_scores, nullptr, &score, nullptr, &class_id_point);

				if (score > config.conf_threshold) {
					// 提取边界框信息
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

					// 提取掩膜系数
					const cv::Mat mask_coeffs = det_output.col(i).rowRange(4 + num_classes, detection_attribute_size);
					if (mask_coeffs.rows != mask_protos.rows) {
						throw std::runtime_error("Mismatch between mask_coeffs and mask_protos dimensions.");
					}

					// 计算掩膜
					cv::Mat mask_flat = mask_coeffs.t() * mask_protos; // [1, mask_h * mask_w]
					if (mask_flat.total() != mask_h * mask_w) {
						throw std::runtime_error("Mismatch between mask_flat size and mask_h * mask_w.");
					}

					cv::Mat mask = mask_flat.reshape(1, mask_h); // [mask_h, mask_w]
					cv::Mat mask_sigmoid;
					cv::exp(-mask, mask_sigmoid);
					mask_sigmoid = 1.0 / (1.0 + mask_sigmoid); // 应用 sigmoid 激活函数
					mask_sigmoids.push_back(mask_sigmoid);
				}
			}

			// 非极大值抑制 (NMS)
			std::vector<int> nms_result = nmsWithKeepClass(
				boxes, class_ids, confidences, config.conf_threshold, config.nms_threshold, config.classids_nms_together);

			for (int i = 0; i < nms_result.size(); i++) {
				DetectionSeg result;
				int idx = nms_result[i];
				result.class_id = class_ids[idx];
				result.conf = confidences[idx];
				result.bbox = boxes[idx];

				// 保存掩膜
				result.mask_sigmoid = mask_sigmoids[idx];
				output.push_back(result);
			}

			masks = output;
			auto result = convertToDetectionRectangleInfo(output);

			return result;
		}

		void ModelEngine_Yolov11_seg_mask::init(const std::string& enginePath, nvinfer1::ILogger& logger)
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
			maskCoefficientNum = engine->getTensorShape(engine->getIOTensorName(2)).d[1];
			mask_h = engine->getTensorShape(engine->getIOTensorName(2)).d[2];
			mask_w = engine->getTensorShape(engine->getIOTensorName(2)).d[3];
			detection_attribute_size = engine->getTensorShape(engine->getIOTensorName(1)).d[1];
			num_detections = engine->getTensorShape(engine->getIOTensorName(1)).d[2];
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

		ModelEngine_Yolov11_seg_mask::ModelEngine_Yolov11_seg_mask(const std::string& modelPath,
			nvinfer1::ILogger& logger)
		{
			init(modelPath, logger);
		}

		ModelEngine_Yolov11_seg_mask::~ModelEngine_Yolov11_seg_mask()
		{
			for (int i = 0; i < 2; i++)
				(cudaFree(gpu_buffers[i]));
			delete[] cpu_output_buffer;
			delete[] cpu_output_buffer2;
			delete context;
			delete engine;
			delete runtime;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg_mask::convertToDetectionRectangleInfo(const std::vector<DetectionSeg>& detections)
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

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg_mask::convertWhenResize(
			const std::vector<DetectionSeg>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());
			auto scaleX = _sourceWidth / static_cast<float>(input_w);
			auto scaleY = _sourceHeight / static_cast<float>(input_h);
			for (const auto& item : detections)
			{
				// 调整 bbox 到原图比例
				cv::Rect bbox = item.bbox;
				bbox.x = static_cast<int>(bbox.x * scaleX);
				bbox.y = static_cast<int>(bbox.y * scaleY);
				bbox.width = static_cast<int>(bbox.width * scaleX);
				bbox.height = static_cast<int>(bbox.height * scaleY);

				DetectionRectangleInfo resultItem;
				resultItem.width = bbox.width;
				resultItem.height = bbox.height;
				resultItem.leftTop.first = bbox.x;
				resultItem.leftTop.second = bbox.y;
				resultItem.rightTop.first = bbox.x + bbox.width;
				resultItem.rightTop.second = bbox.y;
				resultItem.leftBottom.first = bbox.x;
				resultItem.leftBottom.second = bbox.y + bbox.height;
				resultItem.rightBottom.first = bbox.x + bbox.width;
				resultItem.rightBottom.second = bbox.y + bbox.height;
				resultItem.center_x = bbox.x + bbox.width / 2;
				resultItem.center_y = bbox.y + bbox.height / 2;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;

				cv::Rect img_rect(0, 0, _sourceWidth, _sourceHeight);
				cv::Rect roi = bbox & img_rect;
				if (roi.width <= 0 || roi.height <= 0) continue;
				resultItem.roi = roi;
				resultItem.segMaskValid = true;

				cv::Mat mask_resized;
				cv::resize(item.mask_sigmoid, mask_resized, cv::Size(_sourceWidth, _sourceHeight), 0, 0, cv::INTER_LINEAR);
				cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, -bbox.x, 0, 1, -bbox.y);
				cv::Mat mask_translated = cv::Mat::zeros(_sourceWidth, _sourceHeight, mask_resized.type());
				cv::warpAffine(mask_resized, mask_translated, translation_matrix, cv::Size(_sourceWidth, _sourceHeight), cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);
				cv::Mat mask_bin;
				cv::threshold(mask_translated, mask_bin, 0.5, 1.0, cv::THRESH_BINARY);
				cv::Mat mask_roi = mask_bin(cv::Rect(0, 0, roi.width, roi.height));
				resultItem.area = cv::countNonZero(mask_roi);
				resultItem.mask_roi = mask_roi.clone();

				result.push_back(resultItem);
			}

			return result;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg_mask::convertWhenCentralCrop(
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

				cv::Rect bbox = item.bbox;
				bbox.x = item.bbox.x + params.crop_x - params.pad_left;
				bbox.y = item.bbox.y + params.crop_y - params.pad_top;
				bbox.width = item.bbox.x + item.bbox.width + params.crop_x - params.pad_left - bbox.x;
				bbox.height = item.bbox.y + item.bbox.height + params.crop_y - params.pad_top - bbox.y;

				DetectionRectangleInfo resultItem;
				resultItem.width = bbox.width;
				resultItem.height = bbox.height;
				resultItem.leftTop.first = bbox.x;
				resultItem.leftTop.second = bbox.y;
				resultItem.rightTop.first = bbox.x + bbox.width;
				resultItem.rightTop.second = bbox.y;
				resultItem.leftBottom.first = bbox.x;
				resultItem.leftBottom.second = bbox.y + bbox.height;
				resultItem.rightBottom.first = bbox.x + bbox.width;
				resultItem.rightBottom.second = bbox.y + bbox.height;
				resultItem.center_x = bbox.x + bbox.width / 2;
				resultItem.center_y = bbox.y + bbox.height / 2;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;

				cv::Rect img_rect(0, 0, _sourceWidth, _sourceHeight);
				cv::Rect roi = bbox & img_rect;
				if (roi.width <= 0 || roi.height <= 0) continue;
				resultItem.roi = roi;
				resultItem.segMaskValid = true;

				// 获取中心裁剪的偏移量
				int pad_left = _centerCropParams.pad_left;
				int pad_top = _centerCropParams.pad_top;
				int crop_x = _centerCropParams.crop_x;
				int crop_y = _centerCropParams.crop_y;

				// 将掩膜调整回裁剪后的尺寸
				cv::Mat mask_resized;
				cv::resize(item.mask_sigmoid, mask_resized, cv::Size(input_w, input_h), 0, 0, cv::INTER_LINEAR);

				// 计算平移矩阵，将掩膜从裁剪后的坐标系映射回原始图像
				cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, crop_x - pad_left - bbox.x, 0, 1, crop_y - pad_top - bbox.y);
				cv::Mat mask_translated = cv::Mat::zeros(_sourceHeight, _sourceWidth, mask_resized.type());
				cv::warpAffine(mask_resized, mask_translated, translation_matrix, cv::Size(_sourceWidth, _sourceHeight), cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0);

				// 二值化掩膜
				cv::Mat mask_bin;
				cv::threshold(mask_translated, mask_bin, 0.5, 1.0, cv::THRESH_BINARY);

				// 提取ROI区域的掩膜
				cv::Mat mask_roi = mask_bin(cv::Rect(roi.x, roi.y, roi.width, roi.height));
				resultItem.area = cv::countNonZero(mask_roi);
				resultItem.mask_roi = mask_roi.clone();

				result.push_back(resultItem);
			}
			return result;
		}

		std::vector<DetectionRectangleInfo> ModelEngine_Yolov11_seg_mask::convertWhenLetterBox(
			const std::vector<DetectionSeg>& detections)
		{
			std::vector<DetectionRectangleInfo> result;
			result.reserve(detections.size());

			float scale = _letterBoxScale;
			int dw = _letterBoxdw;
			int dh = _letterBoxdh;

			for (const auto& item : detections)
			{
				cv::Rect bbox = item.bbox;
				bbox.x = (item.bbox.x - dw) / scale;
				bbox.y = (item.bbox.y - dh) / scale;
				bbox.width = (item.bbox.x + item.bbox.width - dw) / scale - bbox.x;
				bbox.height = (item.bbox.y + item.bbox.height - dh) / scale - bbox.y;

				DetectionRectangleInfo resultItem;
				resultItem.width = bbox.width;
				resultItem.height = bbox.height;
				resultItem.leftTop.first = bbox.x;
				resultItem.leftTop.second = bbox.y;
				resultItem.rightTop.first = bbox.x + bbox.width;
				resultItem.rightTop.second = bbox.y;
				resultItem.leftBottom.first = bbox.x;
				resultItem.leftBottom.second = bbox.y + bbox.height;
				resultItem.rightBottom.first = bbox.x + bbox.width;
				resultItem.rightBottom.second = bbox.y + bbox.height;
				resultItem.center_x = bbox.x + bbox.width / 2;
				resultItem.center_y = bbox.y + bbox.height / 2;
				resultItem.classId = item.class_id;
				resultItem.score = item.conf;

				cv::Rect img_rect(0, 0, _sourceWidth, _sourceHeight);
				cv::Rect roi = bbox & img_rect;
				if (roi.width <= 0 || roi.height <= 0) continue;
				resultItem.roi = roi;
				resultItem.segMaskValid = true;

				// 计算 LetterBox 的缩放比例和填充偏移
				float scale = _letterBoxScale;
				int dw = _letterBoxdw;
				int dh = _letterBoxdh;

				// 调整掩膜到原图比例
				cv::Mat mask_resized;
				cv::resize(item.mask_sigmoid, mask_resized, cv::Size(input_w, input_h), 0, 0, cv::INTER_LINEAR);

				// 去除填充并应用缩放
				cv::Mat mask_translated = mask_resized(cv::Rect(dw, dh, input_w - 2 * dw, input_h - 2 * dh));
				cv::resize(mask_translated, mask_translated, cv::Size(_sourceWidth, _sourceHeight), 0, 0, cv::INTER_LINEAR);

				// 二值化掩膜
				cv::Mat mask_bin;
				cv::threshold(mask_translated, mask_bin, 0.5, 1.0, cv::THRESH_BINARY);

				// 提取 ROI 区域
				cv::Mat mask_roi = mask_bin(roi);
				resultItem.area = cv::countNonZero(mask_roi);
				resultItem.mask_roi = mask_roi.clone();

				result.push_back(resultItem);
			}
			return result;
		}

		cv::Mat ModelEngine_Yolov11_seg_mask::draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList)
		{
			cv::Mat result = mat.clone();
			ImagePainter::PainterConfig config;
			for (const auto& item : infoList)
			{
				std::ostringstream oss;
				oss << "classId:" << item.classId << " score:" << std::fixed << std::setprecision(2) << item.score;
				config.text = oss.str();
				ImagePainter::drawShapesOnSourceImg(result, item, config);

				int blue = (item.classId * 37) % 256;  // 37 是一个随机质数，用于生成分布均匀的值
				int green = (item.classId * 73) % 256; // 73 是另一个随机质数
				int red = (item.classId * 109) % 256;  // 109 是另一个随机质数

				config.color = cv::Scalar(blue, green, red); // BGR 格式
				config.alpha = 10;
				ImagePainter::drawMaskOnSourceImg(result, item, config);
			}

			return result;
		}
	}
}