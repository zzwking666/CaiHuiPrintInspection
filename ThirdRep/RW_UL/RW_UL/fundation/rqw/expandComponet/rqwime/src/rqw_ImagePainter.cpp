#include "rqw_ImagePainter.h"

#include<QPainter>

namespace rw
{
	namespace rqw
	{
		QColor ImagePainter::toQColor(BasicColor color)
		{
			switch (color)
			{
			case BasicColor::Red:
				return QColor(Qt::red);
			case BasicColor::Green:
				return QColor(Qt::green);
			case BasicColor::Blue:
				return QColor(Qt::blue);
			case BasicColor::Yellow:
				return QColor(Qt::yellow);
			case BasicColor::Cyan:
				return QColor(Qt::cyan);
			case BasicColor::Magenta:
				return QColor(Qt::magenta);
			case BasicColor::White:
				return QColor(Qt::white);
			case BasicColor::Black:
				return QColor(Qt::black);
			case BasicColor::Orange:
				return QColor(255, 165, 0); // RGB for orange
			case BasicColor::LightBlue:
				return QColor(173, 216, 230); // RGB for light blue
			case BasicColor::Gray:
				return QColor(Qt::gray);
			case BasicColor::Purple:
				return QColor(128, 0, 128); // RGB for purple
			case BasicColor::Brown:
				return QColor(165, 42, 42); // RGB for brown
			case BasicColor::LightBrown:
				return QColor(210, 180, 140); // RGB for light brown
			default:
				return QColor(Qt::black); // Default to black if unknown color
			}
		}

		void ImagePainter::drawTextOnImage(QImage& image, const QVector<QString>& texts,
			const std::vector<PainterConfig>& colorList, double proportion)
		{
			if (texts.empty() || proportion <= 0.0 || proportion > 1.0) {
				return;
			}

			QPainter painter(&image);
			painter.setRenderHint(QPainter::Antialiasing);

			//calculate the font size based on the image height and the proportion
			//getting the image height
			int imageHeight = image.height();
			int fontSize = static_cast<int>(imageHeight * proportion);

			QFont font = painter.font();
			font.setPixelSize(fontSize);
			painter.setFont(font);

			int x = 0;
			int y = fontSize;

			for (size_t i = 0; i < texts.size(); ++i) {
				QColor color = (i < colorList.size()) ? colorList[i].textColor : colorList.back().textColor;
				painter.setPen(color);

				painter.drawText(x, y, texts[i]);

				y += fontSize;
			}

			painter.end();
		}

		void ImagePainter::drawTextOnImageWithConfig(QImage& image, const QVector<QString>& texts,
			const std::vector<PainterConfig>& colorList)
		{
			if (texts.empty()) {
				return;
			}

			QPainter painter(&image);
			painter.setRenderHint(QPainter::Antialiasing);

			int x = 0;
			int y = 0;

			for (size_t i = 0; i < texts.size(); ++i) {
				// 获取当前文本的颜色和字体大小
				QColor color = (i < colorList.size()) ? colorList[i].textColor : colorList.back().textColor;
				int fontSize = (i < colorList.size()) ? colorList[i].fontSize : colorList.back().fontSize;

				// 设置画笔颜色
				painter.setPen(color);

				// 设置字体大小
				QFont font = painter.font();
				font.setPixelSize(fontSize);
				painter.setFont(font);

				// 绘制文本
				painter.drawText(x, y + fontSize, texts[i]);

				// 更新 y 坐标，确保下一行文本不会重叠
				y += fontSize;
			}

			painter.end();
		}

		QImage ImagePainter::cvMatToQImage(const cv::Mat& mat)
		{
			QImage result;
			if (mat.type() == CV_8UC1) {
				result = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_Grayscale8);
			}
			else if (mat.type() == CV_8UC3) {
				result = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).rgbSwapped();
			}
			else if (mat.type() == CV_8UC4) {
				result = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGBA8888);
			}
			else {
				result = QImage();
			}

			return result;
		}

		QVector3D ImagePainter::calculateRegionRGB(const QImage& image, const DetectionRectangleInfo& total,
			CropMode mode, const QVector<DetectionRectangleInfo>& excludeRegions, CropMode excludeMode)
		{
			QRect rect_total(
				QPoint(total.leftTop.first, total.leftTop.second),
				QSize(total.width, total.height)
			);

			QVector<QRect> rect_exclude;
			for (const auto& item : excludeRegions)
			{
				QRect rect_excludeItem(
					QPoint(item.leftTop.first, item.leftTop.second),
					QSize(item.width, item.height)
				);
				rect_exclude.push_back(rect_excludeItem);
			}
			return calculateRegionRGB(image, rect_total, mode, rect_exclude, excludeMode);
		}

		QVector3D ImagePainter::calculateRegionRGB(const QImage& image, const QRect& rect, CropMode mode,
			const QVector<QRect>& excludeRegions, CropMode excludeMode)
		{
			if (image.isNull()) {
				throw std::invalid_argument("Input image is empty.");
			}

			if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
				throw std::invalid_argument("Input image must be a 3-channel (RGB) image.");
			}

			QRect validRect = rect.intersected(image.rect());
			if (validRect.isEmpty()) {
				throw std::invalid_argument("The rectangle is outside the image bounds.");
			}

			QImage mask(image.size(), QImage::Format_Grayscale8);
			mask.fill(0);

			QPainter painter(&mask);
			painter.setBrush(Qt::white);
			if (mode == CropMode::Rectangle) {
				painter.drawRect(validRect);
			}
			else if (mode == CropMode::InscribedCircle) {
				int radius = std::min(validRect.width(), validRect.height()) / 2;
				QPoint center(validRect.center());
				painter.drawEllipse(center, radius, radius);
			}
			else {
				throw std::invalid_argument("Invalid crop mode.");
			}

			painter.setBrush(Qt::black);
			for (const auto& excludeRect : excludeRegions) {
				QRect validExcludeRect = excludeRect.intersected(validRect);
				if (validExcludeRect.isEmpty()) {
					continue;
				}

				if (excludeMode == CropMode::Rectangle) {
					painter.drawRect(validExcludeRect);
				}
				else if (excludeMode == CropMode::InscribedCircle) {
					int radius = std::min(validExcludeRect.width(), validExcludeRect.height()) / 2;
					QPoint center(validExcludeRect.center());
					painter.drawEllipse(center, radius, radius);
				}
			}
			painter.end();

			double totalR = 0, totalG = 0, totalB = 0;
			int pixelCount = 0;

			for (int y = 0; y < image.height(); ++y) {
				for (int x = 0; x < image.width(); ++x) {
					if (qGray(mask.pixel(x, y)) > 0) {
						QColor color(image.pixel(x, y));
						totalR += color.red();
						totalG += color.green();
						totalB += color.blue();
						++pixelCount;
					}
				}
			}

			if (pixelCount == 0) {
				throw std::runtime_error("No valid pixels in the specified region.");
			}

			return QVector3D(totalR / pixelCount, totalG / pixelCount, totalB / pixelCount);
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const QPoint& center, int radius, PainterConfig config)
		{
			QPainter painter(&image);
			painter.setPen(QPen(config.color, config.thickness));

			if (config.shapeType == ShapeType::Circle) {
				painter.drawEllipse(center, radius, radius);
			}
			else if (config.shapeType == ShapeType::Rectangle) {
				int sideLength = radius * 2;
				QRect rect(center.x() - radius, center.y() - radius, sideLength, sideLength);
				painter.drawRect(rect);
			}

			painter.end();
		}

		QImage ImagePainter::drawShapes(const QImage& image, const std::vector<DetectionRectangleInfo>& rectInfo,
			PainterConfig config)
		{
			QImage resultImage = image.copy();
			for (const auto& item : rectInfo)
			{
				drawShapesOnSourceImg(resultImage, item, config);
			}
			return resultImage;
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const std::vector<DetectionRectangleInfo>& rectInfo,
			PainterConfig config)
		{
			for (const auto& item : rectInfo)
			{
				drawShapesOnSourceImg(image, item, config);
			}
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const std::vector<std::vector<size_t>> index,
			const std::vector<DetectionRectangleInfo>& rectInfo, PainterConfig config)
		{
			for (const auto& classId : index) {
				for (const auto& item : classId)
				{
					drawShapesOnSourceImg(image, rectInfo[item], config);
				}
			}
		}

		QImage ImagePainter::drawShapes(const QImage& image, const DetectionRectangleInfo& rectInfo, PainterConfig config)
		{
			QImage resultImage = image.copy();
			drawShapesOnSourceImg(resultImage, rectInfo, config);
			return resultImage;
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const DetectionRectangleInfo& rectInfo, PainterConfig config)
		{
			if (config.shapeType == ShapeType::Rectangle) {
				QPainter painter(&image);
				painter.setPen(QPen(config.color, config.thickness));

				QPolygonF obbPolygon;
				obbPolygon << QPointF(rectInfo.leftTop.first, rectInfo.leftTop.second)
					<< QPointF(rectInfo.rightTop.first, rectInfo.rightTop.second)
					<< QPointF(rectInfo.rightBottom.first, rectInfo.rightBottom.second)
					<< QPointF(rectInfo.leftBottom.first, rectInfo.leftBottom.second);

				painter.drawPolygon(obbPolygon);
			}
			else if (config.shapeType == ShapeType::Circle) {
				QPainter painter(&image);
				painter.setPen(QPen(config.color, config.thickness));
				int radius = std::min(rectInfo.width, rectInfo.height) / 2;
				painter.drawEllipse(
					QPointF(rectInfo.center_x, rectInfo.center_y),
					radius,
					radius
				);
			}
			QPainter textPainter(&image);
			textPainter.setPen(config.textColor);

			QFont font = textPainter.font();
			font.setPixelSize(config.fontSize);
			textPainter.setFont(font);

			QPointF textPosition;
			int offset = config.fontSize; // 动态计算偏移量，基于字体大小的一半
			switch (config.textLocate) {
			case PainterConfig::TextLocate::LeftTopIn:
				textPosition = QPointF(rectInfo.leftTop.first + 10, rectInfo.leftTop.second + offset);
				break;
			case PainterConfig::TextLocate::LeftTopOut:
				textPosition = QPointF(rectInfo.leftTop.first, rectInfo.leftTop.second - 10);
				break;
			case PainterConfig::TextLocate::RightTopIn:
				textPosition = QPointF(rectInfo.rightTop.first - offset * config.text.size() / 1.5, rectInfo.rightTop.second + offset);
				break;
			case PainterConfig::TextLocate::RightTopOut:
				textPosition = QPointF(rectInfo.rightTop.first - offset * config.text.size() / 1.5, rectInfo.rightTop.second - 10);
				break;
			case PainterConfig::TextLocate::LeftBottomIn:
				textPosition = QPointF(rectInfo.leftBottom.first + 10, rectInfo.leftBottom.second - 10);
				break;
			case PainterConfig::TextLocate::LeftBottomOut:
				textPosition = QPointF(rectInfo.leftBottom.first, rectInfo.leftBottom.second + offset);
				break;
			case PainterConfig::TextLocate::RightBottomIn:
				textPosition = QPointF(rectInfo.rightBottom.first - offset * config.text.size() / 1.5, rectInfo.rightBottom.second - 10);
				break;
			case PainterConfig::TextLocate::RightBottomOut:
				textPosition = QPointF(rectInfo.rightBottom.first - offset * config.text.size() / 1.5, rectInfo.rightBottom.second + offset);
				break;
			case PainterConfig::TextLocate::CenterIn:
				textPosition = QPointF(rectInfo.center_x, rectInfo.center_y);
				break;
			default:
				throw std::invalid_argument("Unsupported TextLocate type.");
			}

			textPainter.drawText(textPosition, config.text);
		}

		void ImagePainter::drawVerticalLine(QImage& image, int position, const ImagePainter::PainterConfig& config)
		{
			QPainter painter(&image);
			painter.setPen(QPen(config.color, config.thickness));
			painter.drawLine(position, 0, position, image.height());
		}
		void ImagePainter::drawHorizontalLine(QImage& image, int position, const ImagePainter::PainterConfig& config)
		{
			QPainter painter(&image);
			painter.setPen(QPen(config.color, config.thickness));
			painter.drawLine(0, position, image.width(), position);
		}
	}
}