#pragma once

#include <QImage>

#include "ime_utilty.hpp"
#include "imgPro_ImageProcessUtilty.hpp"
#include"rqw_rqwColor.hpp"

//TODO:拆分出一个独立的绘图模块使用Cmake配置

namespace rw
{
	namespace rqw
	{
		struct PainterRectangleInfo;
	}

	namespace imgPro
	{
		using Point = std::pair<int, int>;

		inline double pointDistance(const Point& a, const Point& b) noexcept
		{
			const double dx = static_cast<double>(a.first) - static_cast<double>(b.first);
			const double dy = static_cast<double>(a.second) - static_cast<double>(b.second);
			return std::hypot(dx, dy);
		}

		struct ConfigDrawCircle
		{
			int thickness = 1;
			Color color = Color::Red;
		};

		struct ConfigDrawCircleWithLocate
		{
			int thickness = 1;
			Color color = Color::Red;
			QPoint center{ 0, 0 };
			int radius = 0;
		};

		struct ConfigDrawLine
		{
			int position = 0;
			int thickness = 1;
			Color color = Color::Red;
			bool isDashed{ false };
		};

		struct ConfigDrawSegment
		{
			Point startPoint{};
			Point endPoint{};
			int thickness = 1;
			Color color = Color::Red;
			bool isDashed{ false };
			QString text{};
			Color textColor = Color::Red;
			int fontSize{3};
			enum class TextLocate
			{
				Left,
				Middle,
				Right
			};
			TextLocate textLocate = TextLocate::Middle;
		};

		struct ConfigDrawRect
		{
		public:
			int thickness = 1;
			Color rectColor = Color::Red;
			QString text;
			Color textColor = Color::Red;
			int fontSize = 3;
		public:
			enum class TextLocate
			{
				LeftTopIn,
				LeftTopOut,
				RightTopIn,
				RightTopOut,
				LeftBottomIn,
				LeftBottomOut,
				RightBottomIn,
				RightBottomOut,
				CenterIn,
			};
			TextLocate textLocate = TextLocate::LeftTopOut;
		public:
			bool isRegion{false};
			double alpha{ 0.3 };
			double thresh{ 0.5 };
			double maxVal{ 1.0 };
			bool hasFrame{ true };
			bool isDashed{false};
		};

		struct ConfigDrawMask
		{
		public:
			rw::rqw::RQWColor maskColor = rw::rqw::RQWColor::Red;
			double alpha{ 0.3 };
			double thresh{ 0.5 };
			double maxVal{ 1.0 };
			bool hasFrame{ true };
		public:
			ConfigDrawRect rectCfg;
		};

		struct ImagePainter
		{
		public:
			static void drawVerticalLine(
				QImage& image, 
				const ConfigDrawLine& cfg
			);
			static void drawHorizontalLine(
				QImage& image,
				const ConfigDrawLine& cfg
			);

			static void drawSegmentLine(
				QImage& image,
				const ConfigDrawSegment& cfg
				);

			static void drawShapesOnSourceImg(
				QImage& image, 
				const DetectionRectangleInfo& rectInfo,
				const ConfigDrawRect& cfg
			);
			static void drawShapesOnSourceImg(
				QImage& image, 
				const rqw::PainterRectangleInfo& rectInfo,
				const ConfigDrawRect& cfg
			);
			static void drawShapesOnSourceImg(
				QImage& image,
				const DetectionRectangleInfo& rectInfo,
				const ConfigDrawCircle& cfg
			);

			static void drawShapesOnSourceImg(
				QImage& image,
				const ConfigDrawCircleWithLocate& cfg
			);

			static void drawTextOnImage(
				QImage& image, 
				const QVector<QString>& texts,
				const std::vector<Color>& colorList,
				double proportion
			);
			static void drawTextOnImageWithFontSize(
				QImage& image, 
				const QVector<QString>& texts,
				const std::vector<Color>& colorList,
				int fontSize
			);
		public:
			static void drawMaskOnSourceImg(
				QImage& image, 
				const DetectionRectangleInfo& rectInfo,
				const ConfigDrawMask& cfg
			);
		};
	}
}