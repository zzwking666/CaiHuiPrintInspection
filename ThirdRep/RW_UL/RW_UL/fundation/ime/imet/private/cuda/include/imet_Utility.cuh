#pragma once

#include <cuda_runtime.h>
#include <opencv2/opencv.hpp>

namespace rw
{
	namespace imet
	{
		struct Utility
		{
			static void transpose(
				float* src,
				float* dst, 
				int rows, 
				int cols, 
				cudaStream_t stream
			);

			static void transpose(
				float* src,
				float* dst,
				int rows,
				int cols
			);

			static void nms_det(
				float* data, 
				float kNmsThresh,
				int maxObjects,
				int numBoxElement,
				size_t* id_data,
				int id_nums,
				cudaStream_t stream
			);

			static void nms_det(
				float* data,
				float kNmsThresh,
				int maxObjects,
				int numBoxElement,
				size_t* id_data,
				int id_nums
			);

			static void nms_seg(
				float* data,
				float kNmsThresh,
				int maxObjects, 
				int numBoxElement, 
				size_t* id_data, 
				int id_nums,
				cudaStream_t stream
			);
			static void nms_seg(
				float* data,
				float kNmsThresh,
				int maxObjects,
				int numBoxElement,
				size_t* id_data,
				int id_nums
			);

			static void copy_submatrix(
				const float* src,
				float* dst, 
				int batch, 
				int src_rows, 
				int dst_rows, 
				int cols, 
				cudaStream_t stream
			);

			static void copy_submatrix(
				const float* src,
				float* dst,
				int batch,
				int src_rows,
				int dst_rows,
				int cols
			);

			static void draw_mask(cv::Mat& img, float* mask);
		};
	}
}
