#pragma once

#include"imet_Utility.cuh"

namespace rw
{
	namespace imet
	{
		void launch_letterbox_kernel(
			const unsigned char* src, int src_w, int src_h, int src_stride,
			float* dst, int dst_w, int dst_h,
			float scale, int pad_w, int pad_h,
			unsigned char pad_b, unsigned char pad_g, unsigned char pad_r);

		void launch_letterbox_kernel(
			const unsigned char* src, int src_w, int src_h, int src_stride,
			float* dst, int dst_w, int dst_h,
			float scale, int pad_w, int pad_h,
			unsigned char pad_b, unsigned char pad_g, unsigned char pad_r,
			cudaStream_t stream);

		struct LetterBoxInfo
		{
			float letterBoxScale{};
			int pad_w{};
			int pad_h{};
			int sourceHeight;
			int sourceWidth;
			int inputHeight{};
			int inputWidth{};
		};

		struct LetterBoxConfig
		{
			float* dstDevData;
			int dstHeight;
			int dstWidth;
			unsigned char pad_b;
			unsigned char pad_g;
			unsigned char pad_r;
		};

		struct Detection
		{
		public:
			float bbox[4];
			float conf;
			int classId;
		};

		struct Detection_seg
		{
			// x1, y1, x2, y2
			Detection det;
			float mask[32];  // mask coefficient
			std::vector<float> maskMatrix;  // 2D mask after mask coefficient multiply proto, and scale to original image
		};

		struct ImgPreprocess
		{
		public:
			static LetterBoxInfo LetterBox(const cv::Mat& srcImg, LetterBoxConfig& cfg, cudaStream_t stream);
			static LetterBoxInfo LetterBox(const cv::Mat& srcImg, LetterBoxConfig& cfg);
			static void scale_bbox(Detection & det,const LetterBoxInfo & info);
			static void scale_bbox(Detection_seg& det, const LetterBoxInfo& info);
		};
	}
}


