#pragma once

#include<QImage>

#include"ime_utilty.hpp"

#include<QVector3D>

namespace rw {
	namespace rqw {
		struct ImagePainter
		{
			enum class ShapeType {
				Rectangle,
				Circle
			};

			enum class BasicColor {
				Red,
				Green,
				Blue,
				Yellow,
				Cyan,
				Magenta,
				White,
				Black,
				Orange,
				LightBlue,
				Gray,
				Purple,
				Brown,
				LightBrown
			};

			static QColor toQColor(BasicColor color);

			struct PainterConfig
			{
			public:
				enum class TextLocate
				{
					LeftTopIn,// Text is inside the left top corner of the shape
					LeftTopOut,
					RightTopIn,
					RightTopOut,
					LeftBottomIn,
					LeftBottomOut,
					RightBottomIn,
					RightBottomOut,
					CenterIn,
				};
			public:
				ShapeType shapeType{ ShapeType::Rectangle };
				int thickness = 2;
				QColor color{ Qt::red };
			public:
				QString text;
				int fontSize = 25;
				int fontThickness = 1;
				QColor textColor{ Qt::green };
			public:
				//The location of the text relative to the shape
				TextLocate textLocate{ TextLocate::LeftTopOut };
			};

			static void drawTextOnImage(QImage& image, const QVector<QString>& texts, const std::vector<PainterConfig>& colorList, double proportion = 0.06);
			static void drawTextOnImageWithConfig(QImage& image, const QVector<QString>& texts, const std::vector<PainterConfig>& colorList);
		public:
			static QImage cvMatToQImage(const cv::Mat& mat);

		public:
			enum class CropMode {
				Rectangle,      //calculate the average color of the rectangle
				InscribedCircle  //calculate the average color of the inscribed circle
			};

			QVector3D calculateRegionRGB(const QImage& image, const DetectionRectangleInfo& total, CropMode mode, const QVector<DetectionRectangleInfo>& excludeRegions, CropMode excludeMode);
			QVector3D calculateRegionRGB(const QImage& image, const QRect& rect, CropMode mode, const QVector<QRect>& excludeRegions, CropMode excludeMode);
		public:
			static void drawShapesOnSourceImg(
				QImage& image,
				const QPoint& center,
				int radius,
				PainterConfig config = {}
			);
		public:
			static QImage drawShapes(
				const QImage& image,
				const std::vector<DetectionRectangleInfo>& rectInfo,
				PainterConfig config
			);

			static void drawShapesOnSourceImg(
				QImage& image,
				const std::vector<DetectionRectangleInfo>& rectInfo,
				PainterConfig config = {}
			);

			static void drawShapesOnSourceImg(QImage& image, const std::vector<std::vector<size_t>> index, const std::vector<DetectionRectangleInfo>& rectInfo,
				PainterConfig config = {});

			static QImage drawShapes(
				const QImage& image,
				const DetectionRectangleInfo& rectInfo,
				PainterConfig config = {}
			);

			static void drawShapesOnSourceImg(
				QImage& image,
				const DetectionRectangleInfo& rectInfo,
				PainterConfig config = {}
			);
		public:
			static void drawVerticalLine(QImage& image, int position, const ImagePainter::PainterConfig& config);
			static void drawHorizontalLine(QImage& image, int position, const ImagePainter::PainterConfig& config);
		};
	}
}