#pragma once

#include"imet_Utility.cuh"

namespace rw
{
	namespace imet
	{
		struct PostProcess
		{
			static void decode_det(
				float* src,
				float* dst,
				int numBboxes,
				int numClasses,
				float confThresh,
				int maxObjects, 
				int numBoxElement,
				cudaStream_t stream
			);

			static void decode_det(
				float* src,
				float* dst,
				int numBboxes,
				int numClasses,
				float confThresh,
				int maxObjects,
				int numBoxElement
			);


			static void decode_seg(float* src,
				float* dst, 
				int numBboxes, 
				int numClasses,
				int numMasks,
				float confThresh, 
				int maxObjects,
				int numBoxElement,
				cudaStream_t stream
			);

			static void decode_seg(float* src,
				float* dst,
				int numBboxes,
				int numClasses,
				int numMasks,
				float confThresh,
				int maxObjects,
				int numBoxElement
			);

			static void matrix_multiply(
				float* aMatrix, 
				int aRows, 
				int aCols,
				float* bMatrix,
				int bRows, 
				int bCols, 
				float* cMatrix,
				cudaStream_t stream,
				bool sigm
			);

			static void downsample_bbox(
				float* bboxDevice, 
				int length,
				float heightRatio, 
				float widthRatio, 
				cudaStream_t stream
			);

			static void crop_mask(
				float* masksDevice, 
				int maskNum, 
				int maskHeight,
				int maskWidth,
				float* bboxesDevice,
				cudaStream_t stream
			);

			static void cut_mask(
				float* masksDevice,
				int maskNum,
				int maskHeight, 
				int maskWidth,
				float* cutMasksDevice, 
				int cutMaskTop,
				int cutMaskLeft, 
				int cutMaskH, 
				int cutMaskW,
				cudaStream_t stream
			);

			static void resize(float* masksDevice, int maskNum, int maskHeight, int maskWidth, float* dstMasksDevice, int dstMaskH, int dstMaskW, cudaStream_t stream);
			
		};
	}
}
