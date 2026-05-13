#include"imet_PostProcess.cuh"

#include "ime_ModelEngine.h"

namespace rw
{
	namespace imet
	{

		__global__ void decode_kernel(float* src, float* dst, int numBboxes, int numClasses, float confThresh, int maxObjects, int numBoxElement) {
			int position = blockDim.x * blockIdx.x + threadIdx.x;
			if (position >= numBboxes) return;

			float* pitem = src + (4 + numClasses) * position;
			float* classConf = pitem + 4;
			float confidence = 0;
			int label = 0;
			for (int i = 0; i < numClasses; i++) {
				if (classConf[i] > confidence) {
					confidence = classConf[i];
					label = i;
				}
			}

			if (confidence < confThresh) return;

			int index = (int)atomicAdd(dst, 1);
			if (index >= maxObjects) return;

			float cx = pitem[0];
			float cy = pitem[1];
			float width = pitem[2];
			float height = pitem[3];

			float left = cx - width * 0.5f;
			float top = cy - height * 0.5f;
			float right = cx + width * 0.5f;
			float bottom = cy + height * 0.5f;

			float* pout_item = dst + 1 + index * numBoxElement;
			pout_item[0] = left;
			pout_item[1] = top;
			pout_item[2] = right;
			pout_item[3] = bottom;
			pout_item[4] = confidence;
			pout_item[5] = label;
			pout_item[6] = 1;  // 1 = keep, 0 = ignore
		}

		void PostProcess::decode_det(float* src, float* dst, int numBboxes, int numClasses, float confThresh, int maxObjects, int numBoxElement, cudaStream_t stream)
		{
			cudaMemsetAsync(dst, 0, sizeof(int), stream);
			int blockSize = 256;
			int gridSize = (numBboxes + blockSize - 1) / blockSize;
			decode_kernel << <gridSize, blockSize, 0, stream >> > (src, dst, numBboxes, numClasses, confThresh, maxObjects, numBoxElement);
		}

		void PostProcess::decode_det(float* src, float* dst, int numBboxes, int numClasses, float confThresh,
			int maxObjects, int numBoxElement)
		{
			cudaMemset(dst, 0, sizeof(int));
			int blockSize = 256;
			int gridSize = (numBboxes + blockSize - 1) / blockSize;
			decode_kernel << <gridSize, blockSize >> > (src, dst, numBboxes, numClasses, confThresh, maxObjects, numBoxElement);
		}


		__global__ void decode_kernel_seg(float* src, float* dst, int numBboxes, int numClasses, int numMasks, float confThresh, int maxObjects, int numBoxElement)
		{
			int position = blockDim.x * blockIdx.x + threadIdx.x;
			if (position >= numBboxes)
				return;

			float* pitem = src + (4 + numClasses + numMasks) * position;
			float* classConf = pitem + 4;
			float confidence = 0;
			int label = 0;
			for (int i = 0; i < numClasses; i++)
			{
				if (classConf[i] > confidence)
				{
					confidence = classConf[i];
					label = i;
				}
			}

			if (confidence < confThresh)
				return;

			int index = (int)atomicAdd(dst, 1);
			if (index >= maxObjects)
				return;

			float cx = pitem[0];
			float cy = pitem[1];
			float width = pitem[2];
			float height = pitem[3];

			float left = cx - width * 0.5f;
			float top = cy - height * 0.5f;
			float right = cx + width * 0.5f;
			float bottom = cy + height * 0.5f;

			float* pout_item = dst + 1 + index * numBoxElement;
			pout_item[0] = left;
			pout_item[1] = top;
			pout_item[2] = right;
			pout_item[3] = bottom;
			pout_item[4] = confidence;
			pout_item[5] = label;
			pout_item[6] = 1; // 1 = keep, 0 = ignore
			for (int j = 0; j < numMasks; j++)
			{
				pout_item[7 + j] = pitem[4 + numClasses + j];
			}
		}

		void PostProcess::decode_seg(float* src, float* dst, int numBboxes, int numClasses, int numMasks,
			float confThresh, int maxObjects, int numBoxElement, cudaStream_t stream)
		{
			cudaMemset(dst, 0, sizeof(int));
			int blockSize = 256;
			int gridSize = (numBboxes + blockSize - 1) / blockSize;
			decode_kernel_seg << <gridSize, blockSize, 0, stream >> > (src, dst, numBboxes, numClasses, numMasks, confThresh, maxObjects, numBoxElement);

		}

		void PostProcess::decode_seg(float* src, float* dst, int numBboxes, int numClasses, int numMasks,
			float confThresh, int maxObjects, int numBoxElement)
		{
			cudaMemset(dst, 0, sizeof(int));
			int blockSize = 256;
			int gridSize = (numBboxes + blockSize - 1) / blockSize;
			decode_kernel_seg << <gridSize, blockSize >> > (src, dst, numBboxes, numClasses, numMasks, confThresh, maxObjects, numBoxElement);
		}

		__device__ float sigmoid(float data)
		{
			return 1.0f / (1.0f + expf(-data));
		}

		__global__ void matrix_multiply_kernel(float* aMatrix, int aCols, float* bMatrix, int bCols, float* cMatrix, int cSize, bool sigm)
		{
			int position = blockDim.x * blockIdx.x + threadIdx.x;
			if (position >= cSize)
				return;

			float target = 0;
			for (int j = 0; j < aCols; j++)
			{
				target += aMatrix[position / bCols * aCols + j] * bMatrix[j * bCols + position % bCols];
			}
			if (sigm)
				cMatrix[position] = sigmoid(target);
			else
				cMatrix[position] = target;
		}

		void PostProcess::matrix_multiply(float* aMatrix, int aRows, int aCols, float* bMatrix, int bRows, int bCols,
			float* cMatrix, cudaStream_t stream, bool sigm)
		{
			int cSize = aRows * bCols;
			int blockSize = 256;
			int gridSize = (cSize + blockSize - 1) / blockSize;
			matrix_multiply_kernel << <gridSize, blockSize, 0, stream >> > (aMatrix, aCols, bMatrix, bCols, cMatrix, cSize, sigm);

		}

		__global__ void downsample_bbox_kernel(float* bbox, int length, float heightRatio, float widthRatio)
		{
			int position = blockDim.x * blockIdx.x + threadIdx.x;
			if (position >= length)
				return;

			if (position % 2 == 0)
			{
				bbox[position] = bbox[position] * widthRatio;
			}
			else
			{
				bbox[position] = bbox[position] * heightRatio;
			}
		}

		void PostProcess::downsample_bbox(float* bboxDevice, int length, float heightRatio, float widthRatio,
			cudaStream_t stream)
		{
			int blockSize = length < 128 ? length : 128;
			int gridSize = (length + blockSize - 1) / blockSize;
			downsample_bbox_kernel << <gridSize, blockSize, 0, stream >> > (bboxDevice, length, heightRatio, widthRatio);
		}

		__global__ void crop_mask_kernel(float* masks, int maskNum, int maskHeight, int maskWidth, float* bboxes)
		{
			int ix = threadIdx.x + blockIdx.x * blockDim.x; // x in global threads
			int iy = threadIdx.y + blockIdx.y * blockDim.y;

			int maskWidthTotal = maskNum * maskWidth;
			if (ix >= maskWidthTotal || iy >= maskHeight)
				return;

			int maskIdx = ix / maskWidth;

			float* presentMask = masks + maskIdx * maskHeight * maskWidth;
			float* presentBbox = bboxes + maskIdx * 4;

			int ix_present = ix % maskWidth;
			int idx = ix_present + iy * maskWidth;
			if (ix_present < presentBbox[0] || ix_present > presentBbox[2] || iy < presentBbox[1] || iy > presentBbox[3])
			{
				presentMask[idx] = 0.0f;
			}
		}

		void PostProcess::crop_mask(float* masksDevice, int maskNum, int maskHeight, int maskWidth, float* bboxesDevice,
			cudaStream_t stream)
		{
			// alloc 2D thread shape (h, w), h >= maskHeight, w >= maskNum * maskWidth
	// that is to say: view 2D mask as horizontal mode
			int maskWidthTotal = maskNum * maskWidth;
			dim3 blockSize(32, 32);
			dim3 gridSize((maskWidthTotal + blockSize.x - 1) / blockSize.x, (maskHeight + blockSize.y - 1) / blockSize.y);
			crop_mask_kernel << <gridSize, blockSize, 0, stream >> > (masksDevice, maskNum, maskHeight, maskWidth, bboxesDevice);

		}

		__global__ void cut_mask_kernel(
			float* masks, int maskNum, int maskHeight, int maskWidth,
			float* cutMasks, int cutMaskTop, int cutMaskLeft, int cutMaskH, int cutMaskW)
		{
			int ix = threadIdx.x + blockIdx.x * blockDim.x; // x in global threads
			int iy = threadIdx.y + blockIdx.y * blockDim.y;

			int cutMaskWidthTotal = maskNum * cutMaskW;
			if (ix >= cutMaskWidthTotal || iy >= cutMaskH)
				return;

			int maskIdx = ix / cutMaskW;

			float* presentCutMask = cutMasks + maskIdx * cutMaskH * cutMaskW;
			float* presentMask = masks + maskIdx * maskHeight * maskWidth;

			int ix_present = ix % cutMaskW;
			int idx = ix_present + iy * cutMaskW;

			int src_ix = ix_present + cutMaskLeft;
			int src_iy = iy + cutMaskTop;
			int src_idx = src_ix + src_iy * maskWidth;

			presentCutMask[idx] = presentMask[src_idx];
		}

		void PostProcess::cut_mask(float* masksDevice, int maskNum, int maskHeight, int maskWidth,
			float* cutMasksDevice, int cutMaskTop, int cutMaskLeft, int cutMaskH, int cutMaskW, cudaStream_t stream)
		{
			int cutMaskWidthTotal = maskNum * cutMaskW;
			dim3 blockSize(32, 32);
			dim3 gridSize((cutMaskWidthTotal + blockSize.x - 1) / blockSize.x, (cutMaskH + blockSize.y - 1) / blockSize.y);
			cut_mask_kernel << <gridSize, blockSize, 0, stream >> > (masksDevice, maskNum, maskHeight, maskWidth,
				cutMasksDevice, cutMaskTop, cutMaskLeft, cutMaskH, cutMaskW);

		}

		__global__ void resize_kernel(float* masks, int maskNum, int maskHeight, int maskWidth, float* dstMasks, int dstMaskH, int dstMaskW)
		{
			int ix = threadIdx.x + blockIdx.x * blockDim.x; // x in global threads
			int iy = threadIdx.y + blockIdx.y * blockDim.y;

			int dstMaskWidthTotal = maskNum * dstMaskW;
			if (ix >= dstMaskWidthTotal || iy >= dstMaskH)
				return;

			int maskIdx = ix / dstMaskW;

			float* srcMask = masks + maskIdx * maskHeight * maskWidth;
			float* dstMask = dstMasks + maskIdx * dstMaskH * dstMaskW;

			ix = ix % dstMaskW;
			int idx = ix + iy * dstMaskW;

			float scaleY = (float)dstMaskH / (float)maskHeight;
			float scaleX = (float)dstMaskW / (float)maskWidth;

			float beforeX = float(ix + 0.5) / scaleX - 0.5;
			float beforeY = float(iy + 0.5) / scaleY - 0.5;
			int topY = static_cast<int>(beforeY);
			int bottomY = topY + 1;
			int leftX = static_cast<int>(beforeX);
			int rightX = leftX + 1;

			float u = beforeX - leftX;
			float v = beforeY - topY;

			if (topY >= maskHeight - 1)
			{ 
				topY = maskHeight - 1;
				bottomY = maskHeight - 1;
			}
			if (leftX >= maskWidth - 1)
			{ // 对应原图的坐标位于最后一列
				leftX = maskWidth - 1;
				rightX = maskWidth - 1;
			}

			dstMask[idx] = (1. - u) * (1. - v) * srcMask[leftX + topY * maskWidth] + (u) * (1. - v) * srcMask[rightX + topY * maskWidth] + (1. - u) * (v)*srcMask[leftX + bottomY * maskWidth] + u * v * srcMask[rightX + bottomY * maskWidth];
		}

		void PostProcess::resize(float* masksDevice, int maskNum, int maskHeight, int maskWidth, float* dstMasksDevice,
			int dstMaskH, int dstMaskW, cudaStream_t stream)
		{
			int dstMaskWidthTotal = maskNum * dstMaskW;
			dim3 blockSize(32, 32);
			dim3 gridSize((dstMaskWidthTotal + blockSize.x - 1) / blockSize.x, (dstMaskH + blockSize.y - 1) / blockSize.y);
			resize_kernel << <gridSize, blockSize, 0, stream >> > (masksDevice, maskNum, maskHeight, maskWidth, dstMasksDevice, dstMaskH, dstMaskW);
		}
	}
}
