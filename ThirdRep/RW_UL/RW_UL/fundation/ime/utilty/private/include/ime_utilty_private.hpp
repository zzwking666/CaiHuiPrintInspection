#pragma once

#include"opencv2/opencv.hpp"

#include <string>
#include <vector>

namespace rw
{
	std::vector<int> nmsWithKeepClass(
		const std::vector<cv::Rect>& boxes,
		const std::vector<int>& class_ids,
		const std::vector<float>& confidences,
		float conf_threshold,
		float nms_threshold,
		const std::vector<size_t>& need_keep_classids);

	struct PreProcess {
	public:
		//letterbox function: scale + fill
		static cv::Mat letterbox(const cv::Mat& src, int target_w, int target_h, cv::Scalar color,
			float& out_scale, int& out_dw, int& out_dh);
		static cv::Mat letterbox(const cv::Mat& src, int target_w, int target_h, cv::Scalar color = cv::Scalar(0, 0, 0),
			float* out_scale = nullptr, int* out_dw = nullptr, int* out_dh = nullptr);
	public:
		struct CenterCropParams {
			int pad_left, pad_top;
			int crop_x, crop_y;
		};

		static cv::Mat centerCrop(
			const cv::Mat& src, int target_w, int target_h,
			cv::Scalar pad_color = cv::Scalar(0, 0, 0),
			CenterCropParams* out_params = nullptr
		);
	};
}