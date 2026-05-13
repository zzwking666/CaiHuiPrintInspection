#include "rqw_HalconUtilty.hpp"

#include <QPixmap>

#include "halconcpp/HalconCpp.h"

namespace rw
{
	namespace rqw
	{
		HalconCpp::HImage QImageToHImage(const QImage& qImage)
		{
			// 检查输入 QImage 是否为空
			if (qImage.isNull())
			{
				throw std::invalid_argument("Input QImage is null.");
			}

			// 获取 QImage 的宽度、高度和格式
			int width = qImage.width();
			int height = qImage.height();
			QImage::Format format = qImage.format();

			HalconCpp::HImage hImage;

			// 根据 QImage 的格式生成对应的 HImage
			if (format == QImage::Format_RGB32 //||
				//format == QImage::Format_ARGB32 ||
				//format == QImage::Format_ARGB32_Premultiplied
				)
			{
				hImage.GenImageInterleaved(
					const_cast<void*>(static_cast<const void*>(qImage.bits())), // PixelPointer
					"bgrx",                                                    // ColorFormat
					width,                                                     // OriginalWidth
					height,                                                    // OriginalHeight
					0,                                                         // Alignment
					"byte",                                                    // Type
					width,                                                     // ImageWidth
					height,                                                    // ImageHeight
					0,                                                         // StartRow
					0,                                                         // StartColumn
					8,                                                         // BitsPerChannel
					0                                                          // BitShift
				);
			}
			//else if (format == QImage::Format_RGB888)
			//{
			//	hImage.GenImageInterleaved(
			//		const_cast<void*>(static_cast<const void*>(qImage.bits())), // PixelPointer
			//		"bgr",                                                     // ColorFormat
			//		width,                                                     // OriginalWidth
			//		height,                                                    // OriginalHeight
			//		0,                                                         // Alignment
			//		"byte",                                                    // Type
			//		width,                                                     // ImageWidth
			//		height,                                                    // ImageHeight
			//		0,                                                         // StartRow
			//		0,                                                         // StartColumn
			//		8,                                                         // BitsPerChannel
			//		0                                                          // BitShift
			//	);
			//}
			//else if (format == QImage::Format_Grayscale8 || format == QImage::Format_Indexed8)
			//{
			//	hImage.GenImage1("byte", width, height, const_cast<void*>(static_cast<const void*>(qImage.bits())));
			//}
			else
			{
				throw std::invalid_argument("Unsupported QImage format.");
			}

			return hImage;
		}

		HalconCpp::HImage CvMatToHImage(const cv::Mat& mat)
		{
			// 检查输入 cv::Mat 是否为空
			if (mat.empty())
			{
				throw std::invalid_argument("Input cv::Mat is empty.");
			}

			// 获取 cv::Mat 的宽度、高度和通道数
			int width = mat.cols;
			int height = mat.rows;
			int channels = mat.channels();

			HalconCpp::HImage hImage;

			// 根据 cv::Mat 的通道数生成对应的 HImage
			if (channels == 1)
			{
				// 单通道灰度图像
				hImage.GenImage1("byte", width, height, const_cast<void*>(static_cast<const void*>(mat.data)));
			}
			else if (channels == 3)
			{
				// 三通道彩色图像，OpenCV 默认存储顺序为 BGR
				hImage.GenImageInterleaved(
					const_cast<void*>(static_cast<const void*>(mat.data)), // PixelPointer
					"bgr",                                                // ColorFormat
					width,                                                 // OriginalWidth
					height,                                                // OriginalHeight
					0,                                                     // Alignment
					"byte",                                                // Type
					width,                                                 // ImageWidth
					height,                                                // ImageHeight
					0,                                                     // StartRow
					0,                                                     // StartColumn
					8,                                                     // BitsPerChannel
					0                                                      // BitShift
				);
			}
			else
			{
				throw std::invalid_argument("Unsupported cv::Mat format. Only 1-channel and 3-channel images are supported.");
			}

			return hImage;
		}

		HalconCpp::HImage QPixmapToHImage(const QPixmap& pixmap)
		{
			QImage image = pixmap.toImage();
			return QImageToHImage(image);
		}

		void GlobalHalconData::clear_shapeModels()
		{
			for (const auto& id : _shapeModelIds)
			{
				HalconCpp::ClearShapeModel(id);
			}
			_shapeModelIds.clear();
		}

		GlobalHalconData::GlobalHalconData()
		{
		}

		GlobalHalconData::~GlobalHalconData()
		{
			clear_shapeModels();
		}
	}
}