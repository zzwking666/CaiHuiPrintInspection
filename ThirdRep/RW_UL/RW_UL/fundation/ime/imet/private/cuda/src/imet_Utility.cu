#include"imet_Utility.cuh"

namespace rw
{
	namespace imet
	{
		// CUDA kernel for transpose [attr, num] -> [num, attr]
		__global__ void transpose_kernel(const float* input, float* output, int rows, int cols, int totalElements)
		{
			int row = blockIdx.y * blockDim.y + threadIdx.y;
			int col = blockIdx.x * blockDim.x + threadIdx.x;
			if (row < rows && col < cols) {
				int in_idx = row * cols + col;
				int out_idx = col * rows + row;
				output[out_idx] = input[in_idx];
			}
		}

		void Utility::transpose(float* src, float* dst, int rows, int cols, cudaStream_t stream)
		{
			dim3 block(16, 16);
			dim3 grid((cols + block.x - 1) / block.x, (rows + block.y - 1) / block.y);
			int totalElements = rows * cols;
			transpose_kernel << <grid, block, 0, stream >> > (src, dst, rows, cols, totalElements);
		}

		void Utility::transpose(float* src, float* dst, int rows, int cols)
		{
			dim3 block(16, 16);
			dim3 grid((cols + block.x - 1) / block.x, (rows + block.y - 1) / block.y);
			int totalElements = rows * cols;
			transpose_kernel << <grid, block >> > (src, dst, rows, cols, totalElements);
			cudaDeviceSynchronize();
		}

		__device__ float box_iou(
			float aleft, float atop, float aright, float abottom,
			float bleft, float btop, float bright, float bbottom
		) {
			float cleft = max(aleft, bleft);
			float ctop = max(atop, btop);
			float cright = min(aright, bright);
			float cbottom = min(abottom, bbottom);

			float c_area = max(cright - cleft, 0.0f) * max(cbottom - ctop, 0.0f);
			if (c_area == 0.0f) return 0.0f;

			float a_area = max(0.0f, aright - aleft) * max(0.0f, abottom - atop);
			float b_area = max(0.0f, bright - bleft) * max(0.0f, bbottom - btop);
			return c_area / (a_area + b_area - c_area);
		}

		__global__ void nms_kernel(float* data, float kNmsThresh, int maxObjects, int numBoxElement, size_t* ids, int id_nums) {
			int position = blockDim.x * blockIdx.x + threadIdx.x;
			int count = min((int)data[0], maxObjects);
			if (position >= count) return;

			// left, top, right, bottom, confidence, class, keepflag
			float* pcurrent = data + 1 + position * numBoxElement;
			int flag = 0;
			for (int tt = 0; tt < id_nums; tt++)
			{
				if (pcurrent[5] == ids[tt])
				{
					flag = 1;
				}
			}
			float* pitem;
			for (int i = 0; i < count; i++) {
				pitem = data + 1 + i * numBoxElement;

				if (i == position || (flag == 0 && pcurrent[5] != pitem[5]))
					continue;

				if (pitem[4] >= pcurrent[4]) {
					if (pitem[4] == pcurrent[4] && i < position) continue;

					float iou = box_iou(
						pcurrent[0], pcurrent[1], pcurrent[2], pcurrent[3],
						pitem[0], pitem[1], pitem[2], pitem[3]
					);

					if (iou > kNmsThresh) {
						pcurrent[6] = 0;  // 1 = keep, 0 = ignore
						return;
					}
				}
			}
		}


		void Utility::nms_det(float* data, float kNmsThresh, int maxObjects, int numBoxElement, size_t* id_data, int id_nums, cudaStream_t stream)
		{
			int blockSize = maxObjects < 256 ? maxObjects : 256;
			int gridSize = (maxObjects + blockSize - 1) / blockSize;
			nms_kernel << <gridSize, blockSize, 0, stream >> > (data, kNmsThresh, maxObjects, numBoxElement, id_data, id_nums);
		}

		void Utility::nms_det(float* data, float kNmsThresh, int maxObjects, int numBoxElement, size_t* id_data,
			int id_nums)
		{
			int blockSize = maxObjects < 256 ? maxObjects : 256;
			int gridSize = (maxObjects + blockSize - 1) / blockSize;
			nms_kernel << <gridSize, blockSize >> > (data, kNmsThresh, maxObjects, numBoxElement, id_data, id_nums);
		}

		__device__ float box_iou_seg(
			float aleft, float atop, float aright, float abottom,
			float bleft, float btop, float bright, float bbottom)
		{
			float cleft = max(aleft, bleft);
			float ctop = max(atop, btop);
			float cright = min(aright, bright);
			float cbottom = min(abottom, bbottom);

			float c_area = max(cright - cleft, 0.0f) * max(cbottom - ctop, 0.0f);
			if (c_area == 0.0f)
				return 0.0f;

			float a_area = max(0.0f, aright - aleft) * max(0.0f, abottom - atop);
			float b_area = max(0.0f, bright - bleft) * max(0.0f, bbottom - btop);
			return c_area / (a_area + b_area - c_area);
		}

		__global__ void nms_kernel_seg(float* data, float kNmsThresh, int maxObjects, int numBoxElement, size_t* ids, int id_nums)
		{
			int position = blockDim.x * blockIdx.x + threadIdx.x;
			int count = min((int)data[0], maxObjects);
			if (position >= count)
				return;

			// left, top, right, bottom, confidence, class, keepflag
			float* pcurrent = data + 1 + position * numBoxElement;
			int flag = 0;
			for (int tt = 0; tt < id_nums; tt++)
			{
				if (pcurrent[5] == ids[tt])
				{
					flag = 1;
				}
			}
			float* pitem;
			for (int i = 0; i < count; i++)
			{
				pitem = data + 1 + i * numBoxElement;
				if (i == position || (flag == 0 && pcurrent[5] != pitem[5]))
					continue;

				if (pitem[4] >= pcurrent[4])
				{
					if (pitem[4] == pcurrent[4] && i < position)
						continue;

					float iou = box_iou_seg(
						pcurrent[0], pcurrent[1], pcurrent[2], pcurrent[3],
						pitem[0], pitem[1], pitem[2], pitem[3]);

					if (iou > kNmsThresh)
					{
						pcurrent[6] = 0; // 1 = keep, 0 = ignore
						return;
					}
				}
			}
		}

		void Utility::nms_seg(float* data, float kNmsThresh, int maxObjects, int numBoxElement, size_t* id_data,
			int id_nums, cudaStream_t stream)
		{
			int blockSize = maxObjects < 256 ? maxObjects : 256;
			int gridSize = (maxObjects + blockSize - 1) / blockSize;
			nms_kernel_seg << <gridSize, blockSize, 0, stream >> > (data, kNmsThresh, maxObjects, numBoxElement, id_data, id_nums);

		}

		void Utility::nms_seg(float* data, float kNmsThresh, int maxObjects, int numBoxElement, size_t* id_data,
			int id_nums)
		{
			int blockSize = maxObjects < 256 ? maxObjects : 256;
			int gridSize = (maxObjects + blockSize - 1) / blockSize;
			nms_kernel_seg << <gridSize, blockSize >> > (data, kNmsThresh, maxObjects, numBoxElement, id_data, id_nums);
		}

		// CUDA kernel: 拷贝子矩阵
		__global__ void copy_submatrix_kernel(
			const float* src, float* dst,
			int batch, int src_rows, int dst_rows, int cols)
		{
			int b = blockIdx.z;
			int r = blockIdx.y * blockDim.y + threadIdx.y;
			int c = blockIdx.x * blockDim.x + threadIdx.x;

			if (b < batch && r < dst_rows && c < cols) {
				int src_idx = b * src_rows * cols + r * cols + c;
				int dst_idx = b * dst_rows * cols + r * cols + c;
				dst[dst_idx] = src[src_idx];
			}
		}

		void Utility::copy_submatrix(const float* src, float* dst, int batch, int src_rows, int dst_rows, int cols,
			cudaStream_t stream)
		{
			dim3 block(32, 8, 1);
			dim3 grid(
				(cols + block.x - 1) / block.x,
				(dst_rows + block.y - 1) / block.y,
				batch
			);
			copy_submatrix_kernel << <grid, block, 0, stream >> > (src, dst, batch, src_rows, dst_rows, cols);
		}

		void Utility::copy_submatrix(const float* src, float* dst, int batch, int src_rows, int dst_rows, int cols)
		{
			dim3 block(32, 8, 1);
			dim3 grid(
				(cols + block.x - 1) / block.x,
				(dst_rows + block.y - 1) / block.y,
				batch
			);
			copy_submatrix_kernel << <grid, block >> > (src, dst, batch, src_rows, dst_rows, cols);
		}

		__global__ void draw_mask_kernel(uchar* imgData, float* mask, int h, int w, int color_b, int color_g, int color_r) {
			int ix = threadIdx.x + blockIdx.x * blockDim.x;
			int iy = threadIdx.y + blockIdx.y * blockDim.y;
			int idx = ix + iy * w;
			int idx3 = idx * 3;

			if (ix >= w || iy >= h) return;

			if (mask[idx] > 0.5) {
				imgData[idx3] = static_cast<uchar>(imgData[idx3] * 0.5 + color_b * 0.5);
				imgData[idx3 + 1] = static_cast<uchar>(imgData[idx3 + 1] * 0.5 + color_g * 0.5);
				imgData[idx3 + 2] = static_cast<uchar>(imgData[idx3 + 2] * 0.5 + color_r * 0.5);
			}
		}

		void Utility::draw_mask(cv::Mat& img, float* mask)
		{
			int h = img.rows;
			int w = img.cols;
			int wh = h * w;
			int elements = wh * 3;

			uchar* imgDataDevice;
			cudaMalloc((void**)&imgDataDevice, elements * sizeof(uchar));
			cudaMemcpy(imgDataDevice, img.data, elements * sizeof(uchar), cudaMemcpyHostToDevice);

			float* maskDevice;
			cudaMalloc((void**)&maskDevice, wh * sizeof(float));
			cudaMemcpy(maskDevice, mask, wh * sizeof(float), cudaMemcpyHostToDevice);



			// 随机生成RGB
			int color_b = rand() % 256;
			int color_g = rand() % 256;
			int color_r = rand() % 256;

			dim3 blockSize(32, 32);
			dim3 gridSize((w + blockSize.x - 1) / blockSize.x, (h + blockSize.y - 1) / blockSize.y);

			draw_mask_kernel << <gridSize, blockSize >> > (imgDataDevice, maskDevice, h, w, color_b, color_g, color_r);

			cudaMemcpy(img.data, imgDataDevice, elements * sizeof(uchar), cudaMemcpyDeviceToHost);

			cudaFree(imgDataDevice);
			cudaFree(maskDevice);
		}
	}
}
