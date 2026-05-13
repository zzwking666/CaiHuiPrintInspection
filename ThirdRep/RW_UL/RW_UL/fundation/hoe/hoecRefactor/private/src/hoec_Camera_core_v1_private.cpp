#include"hoec_Camera_core_v1_private.hpp"

#include "MvCameraControl.h"
#include"opencv2/opencv.hpp"

#include"DVPCamera.h"
#include"dvpir.h"
#include"dvpParam.h"

namespace rw
{
	namespace hoec_v1
	{
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

				cv::Mat rgbImage;

				cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGB2RGB);

				return rgbImage;
			}
			else if (frameInfo.enPixelType == PixelType_Gvsp_BayerRG8)
			{
				cv::Mat bayerImage(height, width, CV_8UC1, pData);
				cv::Mat rgbImage;
				cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerRG2RGB);
				return rgbImage;
			}
			else if (frameInfo.enPixelType == PixelType_Gvsp_BayerGR8)
			{
				cv::Mat bayerImage(height, width, CV_8UC1, pData);
				cv::Mat rgbImage;
				cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerGR2RGB);
				return rgbImage;
			}
			else if (frameInfo.enPixelType == PixelType_Gvsp_Mono8)
			{
				// 单通道灰度图像
				cv::Mat grayImage(height, width, CV_8UC1, pData);
				return grayImage;
			}
			else if (frameInfo.enPixelType == PixelType_Gvsp_BayerRG10)
			{
				cv::Mat bayerImage(height, width, CV_16UC1, pData);
				cv::Mat rgbImage;
				cv::cvtColor(bayerImage, rgbImage, cv::COLOR_BayerRG2RGB);
				return rgbImage;
			}

			return cv::Mat::zeros(height, width, CV_8UC3);
		}

		cv::Mat ImageFrameConvert::DS_ConvertFrameToMat(const dvpFrame& frame, void* pBuffer)
		{
			int width = frame.iWidth;
			int height = frame.iHeight;
			unsigned char* data = static_cast<unsigned char*>(pBuffer);

			cv::Mat image;

			switch (frame.format) {
			case FORMAT_MONO:
				image = cv::Mat(height, width, CV_8UC1, data);
				break;
			case FORMAT_BGR24:
				image = cv::Mat(height, width, CV_8UC3, data);
				break;
			case FORMAT_BGR32:
				image = cv::Mat(height, width, CV_8UC4, data);
				break;
			case FORMAT_BGR48:
				image = cv::Mat(height, width, CV_16UC3, data);
				break;
			case FORMAT_BGR64:
				image = cv::Mat(height, width, CV_16UC4, data);
				break;
			case FORMAT_RGB24:
				image = cv::Mat(height, width, CV_8UC3, data);
				cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
				break;
			case FORMAT_RGB32:
				image = cv::Mat(height, width, CV_8UC4, data);
				cv::cvtColor(image, image, cv::COLOR_RGBA2BGRA);
				break;
			case FORMAT_RGB48:
				image = cv::Mat(height, width, CV_16UC3, data);
				cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
				break;
			default:
				image = cv::Mat::zeros(height, width, CV_8UC3);
				break;
			}
			return image;
		}
	}
}