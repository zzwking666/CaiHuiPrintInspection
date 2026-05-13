#include"ime_utilty_private.hpp"

namespace rw
{
	std::vector<int> nmsWithKeepClass(
		const std::vector<cv::Rect>& boxes,
		const std::vector<int>& class_ids,
		const std::vector<float>& confidences,
		float conf_threshold,
		float nms_threshold,
		const std::vector<size_t>& need_keep_classids)
	{
		std::vector<int> nms_result;
		std::set<size_t> keep_set(need_keep_classids.begin(), need_keep_classids.end());

		if (boxes.empty()) return nms_result;

		if (need_keep_classids.empty()) {
			// All classes are to be NMSed together
			std::map<int, std::vector<int>> class_to_indices;
			for (int i = 0; i < class_ids.size(); ++i) {
				class_to_indices[class_ids[i]].push_back(i);
			}
			for (const auto& kv : class_to_indices) {
				std::vector<cv::Rect> class_boxes;
				std::vector<float> class_confidences;
				std::vector<int> class_nms;
				for (int idx : kv.second) {
					class_boxes.push_back(boxes[idx]);
					class_confidences.push_back(confidences[idx]);
				}
				if (!class_boxes.empty())
					cv::dnn::NMSBoxes(class_boxes, class_confidences, conf_threshold, nms_threshold, class_nms);
				for (int nms_idx : class_nms) {
					nms_result.push_back(kv.second[nms_idx]);
				}
			}
			return nms_result;
		}

		// 1. The classes that need to be NMSed together
		std::vector<cv::Rect> keep_boxes;
		std::vector<float> keep_confidences;
		std::vector<int> keep_indices;
		// 2. other classes are grouped
		std::map<int, std::vector<int>> class_to_indices;
		for (int i = 0; i < class_ids.size(); ++i) {
			if (keep_set.count(class_ids[i])) {
				keep_boxes.push_back(boxes[i]);
				keep_confidences.push_back(confidences[i]);
				keep_indices.push_back(i);
			}
			else {
				class_to_indices[class_ids[i]].push_back(i);
			}
		}
		//3. NMS for the classes that need to be NMSed together
		std::vector<int> keep_nms;
		if (!keep_boxes.empty())
			cv::dnn::NMSBoxes(keep_boxes, keep_confidences, conf_threshold, nms_threshold, keep_nms);
		for (int nms_idx : keep_nms) {
			nms_result.push_back(keep_indices[nms_idx]);
		}
		//4. NMS for other classes
		for (const auto& kv : class_to_indices) {
			std::vector<cv::Rect> class_boxes;
			std::vector<float> class_confidences;
			std::vector<int> class_nms;
			for (int idx : kv.second) {
				class_boxes.push_back(boxes[idx]);
				class_confidences.push_back(confidences[idx]);
			}
			if (!class_boxes.empty())
				cv::dnn::NMSBoxes(class_boxes, class_confidences, conf_threshold, nms_threshold, class_nms);
			for (int nms_idx : class_nms) {
				nms_result.push_back(kv.second[nms_idx]);
			}
		}
		return nms_result;
	}

	cv::Mat PreProcess::letterbox(const cv::Mat& src, int target_w, int target_h, cv::Scalar color, float& out_scale, int& out_dw, int& out_dh)
	{
		int src_w = src.cols;
		int src_h = src.rows;
		float r = std::min(target_w / (float)src_w, target_h / (float)src_h);
		int new_unpad_w = int(round(src_w * r));
		int new_unpad_h = int(round(src_h * r));
		int dw = (target_w - new_unpad_w) / 2;
		int dh = (target_h - new_unpad_h) / 2;

		cv::Mat resized;
		cv::resize(src, resized, cv::Size(new_unpad_w, new_unpad_h));

		cv::Mat out(target_h, target_w, src.type(), color);
		resized.copyTo(out(cv::Rect(dw, dh, new_unpad_w, new_unpad_h)));

		out_scale = r;
		out_dw = dw;
		out_dh = dh;
		return out;
	}

	cv::Mat PreProcess::letterbox(const cv::Mat& src, int target_w, int target_h, cv::Scalar color, float* out_scale,
		int* out_dw, int* out_dh)
	{
		int src_w = src.cols;
		int src_h = src.rows;
		float r = std::min(target_w / (float)src_w, target_h / (float)src_h);
		int new_unpad_w = int(round(src_w * r));
		int new_unpad_h = int(round(src_h * r));
		int dw = (target_w - new_unpad_w) / 2;
		int dh = (target_h - new_unpad_h) / 2;

		cv::Mat resized;
		cv::resize(src, resized, cv::Size(new_unpad_w, new_unpad_h));

		cv::Mat out(target_h, target_w, src.type(), color);
		resized.copyTo(out(cv::Rect(dw, dh, new_unpad_w, new_unpad_h)));

		if (out_scale) *out_scale = r;
		if (out_dw) *out_dw = dw;
		if (out_dh) *out_dh = dh;
		return out;
	}

	cv::Mat PreProcess::centerCrop(const cv::Mat& src, int target_w, int target_h, cv::Scalar pad_color,
		CenterCropParams* out_params)
	{
		int src_w = src.cols;
		int src_h = src.rows;

		int pad_left = std::max(0, (target_w - src_w) / 2);
		int pad_right = std::max(0, target_w - src_w - pad_left);
		int pad_top = std::max(0, (target_h - src_h) / 2);
		int pad_bottom = std::max(0, target_h - src_h - pad_top);

		cv::Mat padded;
		if (pad_left > 0 || pad_right > 0 || pad_top > 0 || pad_bottom > 0) {
			cv::copyMakeBorder(src, padded, pad_top, pad_bottom, pad_left, pad_right, cv::BORDER_CONSTANT, pad_color);
		}
		else {
			padded = src;
		}

		int crop_x = std::max(0, (padded.cols - target_w) / 2);
		int crop_y = std::max(0, (padded.rows - target_h) / 2);

		if (out_params) {
			out_params->pad_left = pad_left;
			out_params->pad_top = pad_top;
			out_params->crop_x = crop_x;
			out_params->crop_y = crop_y;
		}

		cv::Rect roi(crop_x, crop_y, target_w, target_h);
		return padded(roi).clone();
	}
}