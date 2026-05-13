#include "hoec_Camera_core_private.hpp"
#include "opencv2/opencv.hpp"
#include "MvCameraControl.h"
#include "CameraParams.h"

namespace rw {
	namespace hoec {
		cv::Mat ImageFrameConvert::MVS_ConvertFrameToMat(const MV_FRAME_OUT& frameInfo)
		{
			// 获取图像宽度和高度
			int width = frameInfo.stFrameInfo.nWidth;
			int height = frameInfo.stFrameInfo.nHeight;

			// 获取图像数据指针
			unsigned char* data = frameInfo.pBufAddr;

			// 根据图像像素格式创建cv::Mat
			cv::Mat image;
			switch (frameInfo.stFrameInfo.enPixelType) {
			case PixelType_Gvsp_BayerRG8:
				// Bayer RG8格式图像
				image = cv::Mat(height, width, CV_8UC1, data);
				cv::cvtColor(image, image, cv::COLOR_BayerRG2BGR);
				break;
			case PixelType_Gvsp_Mono8:
				// 单通道灰度图像
				image = cv::Mat(height, width, CV_8UC1, data);
				break;
			case PixelType_Gvsp_Mono10:
			case PixelType_Gvsp_Mono12:
				// 单通道灰度图像，10位和12位需要转换为16位
				image = cv::Mat(height, width, CV_16UC1, data);
				break;
			case PixelType_Gvsp_RGB8_Packed:
				// 三通道RGB图像
				image = cv::Mat(height, width, CV_8UC3, data);
				// OpenCV默认的颜色顺序是BGR，需要转换为RGB
				cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
				break;
			case PixelType_Gvsp_YUV422_Packed:
				// YUV422格式图像
				image = cv::Mat(height, width, CV_8UC2, data);
				cv::cvtColor(image, image, cv::COLOR_YUV2BGR_Y422);
				break;
			case PixelType_Gvsp_YUV422_YUYV_Packed:
				// YUV422 UYVY格式图像
				image = cv::Mat(height, width, CV_8UC2, data);
				cv::cvtColor(image, image, cv::COLOR_YUV2BGR_UYVY);
				break;
			case PixelType_Gvsp_BayerGR8:
				// Bayer GR8格式图像
				image = cv::Mat(height, width, CV_8UC1, data);
				cv::cvtColor(image, image, cv::COLOR_BayerGR2BGR);
				break;
			case PixelType_Gvsp_BayerGB8:
				// Bayer GB8格式图像
				image = cv::Mat(height, width, CV_8UC1, data);
				cv::cvtColor(image, image, cv::COLOR_BayerGB2BGR);
				break;
			case PixelType_Gvsp_BayerGR10:
			case PixelType_Gvsp_BayerGR12:
				// Bayer GR10和GR12格式图像，10位和12位需要转换为16位
				image = cv::Mat(height, width, CV_16UC1, data);
				cv::cvtColor(image, image, cv::COLOR_BayerGR2BGR);
				break;
			case PixelType_Gvsp_BayerGR10_Packed:
			case PixelType_Gvsp_BayerGR12_Packed:
				// Bayer GR10和GR12 Packed格式图像，10位和12位需要转换为16位
				image = cv::Mat(height, width, CV_16UC1, data);
				cv::cvtColor(image, image, cv::COLOR_BayerGR2BGR);
				break;
			default:
				std::cerr << "Unsupported pixel format which is " << frameInfo.stFrameInfo.enPixelType << std::endl;
				return cv::Mat();
			}

			return image;
		}

		cv::Mat ImageFrameConvert::MVS_ConvertFrameToMat(const MV_FRAME_OUT_INFO_EX& frameInfo, unsigned char* pData)
		{
			int width = frameInfo.nWidth;
			int height = frameInfo.nHeight;

			if (frameInfo.enPixelType == PixelType_Gvsp_BayerGB8)
			{
				cv::Mat bayerImage(height, width, CV_8UC1, pData);

				// Step 2: 创建一个用于存储 RGB 图像的 cv::Mat
				cv::Mat rgbImage;

				// Step 3: 使用 OpenCV 的 cvtColor 函数将 Bayer GB 8 转换为 RGB
				cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGB2RGB);

				return rgbImage;
			}
			else if (frameInfo.enPixelType == PixelType_Gvsp_BayerRG8)
			{
				cv::Mat bayerImage(height, width, CV_8UC1, pData);

				// Step 2: 创建一个用于存储 RGB 图像的 cv::Mat
				cv::Mat rgbImage;

				// Step 3: 使用 OpenCV 的 cvtColor 函数将 Bayer GB 8 转换为 RGB
				cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerRG2RGB);

				return rgbImage;
			}
			int channels = 1; // 默认单通道

			// 根据像素格式设置通道数
			switch (frameInfo.enPixelType) {
			case PixelType_Gvsp_Mono8:
				channels = 1;
				break;
			case PixelType_Gvsp_Mono10:
			case PixelType_Gvsp_Mono12:
				channels = 1;
				break;
			case PixelType_Gvsp_RGB8_Packed:
				channels = 3;
				break;
			case PixelType_Gvsp_YUV422_Packed:
			case PixelType_Gvsp_YUV422_YUYV_Packed:
				channels = 2;
				break;
			case PixelType_Gvsp_BayerGR8:
			case PixelType_Gvsp_BayerGB8:
			case PixelType_Gvsp_BayerGR10:
			case PixelType_Gvsp_BayerGR12:
			case PixelType_Gvsp_BayerGR10_Packed:
			case PixelType_Gvsp_BayerGR12_Packed:
				channels = 1;
				break;

			default:
				std::cerr << "Unsupported pixel format which is " << frameInfo.enPixelType << std::endl;
				return cv::Mat();
			}

			// 创建cv::Mat对象
			cv::Mat image(height, width, (channels == 1) ? CV_8UC1 : CV_8UC3, pData);

			// 根据像素格式进行颜色转换
			switch (frameInfo.enPixelType) {
			case PixelType_Gvsp_RGB8_Packed:
				cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
				break;
			case PixelType_Gvsp_YUV422_Packed:
				cv::cvtColor(image, image, cv::COLOR_YUV2BGR_Y422);
				break;
			case PixelType_Gvsp_YUV422_YUYV_Packed:
				cv::cvtColor(image, image, cv::COLOR_YUV2BGR_UYVY);
				break;
			case PixelType_Gvsp_BayerGR8:
				cv::cvtColor(image, image, cv::COLOR_BayerGR2BGR);
				break;
			case PixelType_Gvsp_BayerGB8:
				cv::cvtColor(image, image, cv::COLOR_BayerGB2BGR);
				break;
			case PixelType_Gvsp_BayerGR10:
			case PixelType_Gvsp_BayerGR12:
			case PixelType_Gvsp_BayerGR10_Packed:
			case PixelType_Gvsp_BayerGR12_Packed:
				cv::cvtColor(image, image, cv::COLOR_BayerGR2BGR);
				break;
			case PixelType_Gvsp_BayerRG8:
				cv::cvtColor(image, image, cv::COLOR_BayerRG2BGR);
				break;
			default:
				break;
			}

			return image;
		}
	} // namespace hoec
} // namespace rw