#include"imet_PreProcess.cuh"

namespace rw
{
	namespace imet
	{
        __global__ void letterbox_kernel(
            const unsigned char* src, int src_w, int src_h, int src_stride,
            float* dst, int dst_w, int dst_h,
            float scale, int pad_w, int pad_h,
            unsigned char pad_b, unsigned char pad_g, unsigned char pad_r)
        {
            int dx = blockIdx.x * blockDim.x + threadIdx.x;
            int dy = blockIdx.y * blockDim.y + threadIdx.y;
            if (dx >= dst_w || dy >= dst_h) return;

            float fx = (dx - pad_w + 0.5f) / scale - 0.5f;
            float fy = (dy - pad_h + 0.5f) / scale - 0.5f;

            int x0 = floorf(fx);
            int y0 = floorf(fy);
            int x1 = x0 + 1;
            int y1 = y0 + 1;
            float u = fx - x0;
            float v = fy - y0;

            for (int c = 0; c < 3; ++c) {
                float val = 0.f;

                if (x0 >= 0 && x1 < src_w && y0 >= 0 && y1 < src_h) {

                    const unsigned char* p00 = src + y0 * src_stride + x0 * 3;
                    const unsigned char* p01 = src + y0 * src_stride + x1 * 3;
                    const unsigned char* p10 = src + y1 * src_stride + x0 * 3;
                    const unsigned char* p11 = src + y1 * src_stride + x1 * 3;
                    float v00 = p00[c];
                    float v01 = p01[c];
                    float v10 = p10[c];
                    float v11 = p11[c];

                    val = (1 - u) * (1 - v) * v00 +
                        u * (1 - v) * v01 +
                        (1 - u) * v * v10 +
                        u * v * v11;
                    val /= 255.0f;
                }
                else {

                    if (c == 0) val = pad_b / 255.0f;
                    else if (c == 1) val = pad_g / 255.0f;
                    else val = pad_r / 255.0f;
                }

                dst[c * dst_h * dst_w + dy * dst_w + dx] = val;
            }
        }

        void launch_letterbox_kernel(
            const unsigned char* src, int src_w, int src_h, int src_stride,
            float* dst, int dst_w, int dst_h,
            float scale, int pad_w, int pad_h,
            unsigned char pad_b, unsigned char pad_g, unsigned char pad_r)
        {
            dim3 block(16, 16);
            dim3 grid((dst_w + block.x - 1) / block.x, (dst_h + block.y - 1) / block.y);
            letterbox_kernel << <grid, block, 0 >> > (
                src, src_w, src_h, src_stride,
                dst, dst_w, dst_h,
                scale, pad_w, pad_h,
                pad_b, pad_g, pad_r
                );
        }

        void launch_letterbox_kernel(const unsigned char* src, int src_w, int src_h, int src_stride, float* dst, int dst_w,
            int dst_h, float scale, int pad_w, int pad_h, unsigned char pad_b, unsigned char pad_g, unsigned char pad_r,
            cudaStream_t stream)
        {
            dim3 block(16, 16);
            dim3 grid((dst_w + block.x - 1) / block.x, (dst_h + block.y - 1) / block.y);
            letterbox_kernel << <grid, block, 0, stream >> > (
                src, src_w, src_h, src_stride,
                dst, dst_w, dst_h,
                scale, pad_w, pad_h,
                pad_b, pad_g, pad_r
                );
        }

        LetterBoxInfo ImgPreprocess::LetterBox(const cv::Mat& srcImg, LetterBoxConfig& cfg, cudaStream_t stream)
        {
            LetterBoxInfo info;

            cv::Mat mat_continuous = srcImg.isContinuous() ? srcImg : srcImg.clone();
            float scale = (std::min)(cfg.dstWidth / (float)srcImg.cols, cfg.dstHeight / (float)srcImg.rows);
            int new_w = static_cast<int>(srcImg.cols * scale);
            int new_h = static_cast<int>(srcImg.rows * scale);
            int pad_w = (cfg.dstWidth - new_w) / 2;
            int pad_h = (cfg.dstHeight - new_h) / 2;

            unsigned char* d_src = nullptr;
            size_t src_bytes = srcImg.rows * srcImg.cols * srcImg.elemSize();
            cudaMalloc((void**)&d_src, src_bytes);
            cudaMemcpyAsync(d_src, mat_continuous.data, src_bytes, cudaMemcpyHostToDevice, stream);
            launch_letterbox_kernel(
                d_src, srcImg.cols, srcImg.rows, srcImg.step,
                cfg.dstDevData, cfg.dstWidth, cfg.dstHeight,
                scale, pad_w, pad_h,
                cfg.pad_b, cfg.pad_g, cfg.pad_r, stream
            );
            cudaFree(d_src);


            info.letterBoxScale = scale;
            info.pad_w = pad_w;
            info.pad_h = pad_h;
			info.sourceHeight = srcImg.rows;
			info.sourceWidth = srcImg.cols;
			info.inputHeight = cfg.dstHeight;
			info.inputWidth = cfg.dstWidth;

            return info;
        }

        LetterBoxInfo ImgPreprocess::LetterBox(const cv::Mat& srcImg, LetterBoxConfig& cfg)
        {
            LetterBoxInfo info;

            cv::Mat mat_continuous = srcImg.isContinuous() ? srcImg : srcImg.clone();
            float scale = (std::min)(cfg.dstWidth / (float)srcImg.cols, cfg.dstHeight / (float)srcImg.rows);
            int new_w = static_cast<int>(srcImg.cols * scale);
            int new_h = static_cast<int>(srcImg.rows * scale);
            int pad_w = (cfg.dstWidth - new_w) / 2;
            int pad_h = (cfg.dstHeight - new_h) / 2;

            unsigned char* d_src = nullptr;
            size_t src_bytes = srcImg.rows * srcImg.cols * srcImg.elemSize();
            cudaMalloc((void**)&d_src, src_bytes);
            cudaMemcpy(d_src, mat_continuous.data, src_bytes, cudaMemcpyHostToDevice);

            launch_letterbox_kernel(
                d_src, srcImg.cols, srcImg.rows, srcImg.step,
                cfg.dstDevData, cfg.dstWidth, cfg.dstHeight,
                scale, pad_w, pad_h,
                cfg.pad_b, cfg.pad_g, cfg.pad_r
            );
            cudaFree(d_src);


            info.letterBoxScale = scale;
            info.pad_w = pad_w;
            info.pad_h = pad_h;
            info.sourceHeight = srcImg.rows;
            info.sourceWidth = srcImg.cols;
            info.inputHeight = cfg.dstHeight;
            info.inputWidth = cfg.dstWidth;

            return info;
        }

        void ImgPreprocess::scale_bbox(Detection& det, const LetterBoxInfo& info)
        {
            auto& bbox = det.bbox;
            auto& r = info.letterBoxScale;
            auto r_w = info.inputWidth / (info.sourceWidth * 1.0);
            auto r_h = info.inputHeight / (info.sourceHeight * 1.0);
            float pad_h = (info.inputHeight - r * info.sourceHeight) / 2;
            float pad_w = (info.inputWidth - r * info.sourceWidth) / 2;

            bbox[0] = (bbox[0] - pad_w) / r;
            bbox[1] = (bbox[1] - pad_h) / r;
            bbox[2] = (bbox[2] - pad_w) / r;
            bbox[3] = (bbox[3] - pad_h) / r;
        }

        void ImgPreprocess::scale_bbox(Detection_seg& det, const LetterBoxInfo& info)
        {
            auto& bbox = det.det.bbox;
            auto& r = info.letterBoxScale;
            auto r_w = info.inputWidth / (info.sourceWidth * 1.0);
            auto r_h = info.inputHeight / (info.sourceHeight * 1.0);
            float pad_h = (info.inputHeight - r * info.sourceHeight) / 2;
            float pad_w = (info.inputWidth - r * info.sourceWidth) / 2;
            bbox[0] = (bbox[0] - pad_w) / r;
            bbox[1] = (bbox[1] - pad_h) / r;
            bbox[2] = (bbox[2] - pad_w) / r;
			bbox[3] = (bbox[3] - pad_h) / r;
        }
	}
    
}

