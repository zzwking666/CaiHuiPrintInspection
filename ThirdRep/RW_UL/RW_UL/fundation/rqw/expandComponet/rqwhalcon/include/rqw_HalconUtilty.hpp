#pragma once

#include<QImage>
#include<QPixmap>

#include "rqw_rqwColor.hpp"
#include"opencv2/opencv.hpp"

namespace HalconCpp
{
	class HTuple;
	class HObject;
	class HImage;
}

namespace rw {
	namespace rqw {
		class HalconShapeModel;
		class HalconShapeXLDModel;

		using HalconWidgetDisObjectId = int;
		using HalconShapeId = HalconCpp::HTuple;

		HalconCpp::HImage QImageToHImage(const QImage& qImage);
		HalconCpp::HImage CvMatToHImage(const cv::Mat& mat);
		HalconCpp::HImage QPixmapToHImage(const QPixmap& pixmap);

		struct PainterConfig
		{
		public:
			PainterConfig() = default;

			PainterConfig(const RQWColor& color, int thickness)
				: color(color), thickness(thickness) {
			}

			PainterConfig(const PainterConfig& other) = default;

			PainterConfig(PainterConfig&& other) noexcept = default;

			PainterConfig& operator=(const PainterConfig& other) = default;

			PainterConfig& operator=(PainterConfig&& other) noexcept = default;

			~PainterConfig() = default;
		public:
			RQWColor color{ RQWColor::Black };
			int thickness{ 3 };
		};

		class GlobalHalconData
		{
		public:
			friend HalconShapeXLDModel;
			friend HalconShapeModel;
		public:
			static GlobalHalconData& getInstance()
			{
				static GlobalHalconData instance;
				return instance;
			}

			GlobalHalconData(const GlobalHalconData&) = delete;
			GlobalHalconData& operator=(const GlobalHalconData&) = delete;
		private:
			void clear_shapeModels();
		private:
			GlobalHalconData();
			~GlobalHalconData();
		private:
			std::vector<HalconCpp::HTuple> _shapeModelIds;
		};
	} // namespace rqw
} // namespace rw