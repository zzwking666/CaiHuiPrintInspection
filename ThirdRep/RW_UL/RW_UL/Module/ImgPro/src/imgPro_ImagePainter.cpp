#include"imgPro_ImagePainter.hpp"

#include <QPainter>

#include "rqw_ImgConvert.hpp"
#include"PicturesPainterUtilty.h"

namespace rw
{
	namespace imgPro
	{
		void ImagePainter::drawVerticalLine(QImage& image, const ConfigDrawLine& cfg)
		{
			QPainter painter(&image);
			if (cfg.isDashed)
			{
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness, Qt::DashLine));
			}
			else
			{
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness));
			}
			
			painter.drawLine(cfg.position, 0, cfg.position, image.height());
		}

		void ImagePainter::drawHorizontalLine(QImage& image, const ConfigDrawLine& cfg)
		{
			QPainter painter(&image);
			if (cfg.isDashed)
			{
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness, Qt::DashLine));
			}
			else
			{
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness));
			}
			painter.drawLine(0, cfg.position, image.width(), cfg.position);
		}

		void ImagePainter::drawSegmentLine(QImage& image, const ConfigDrawSegment& cfg)
		{
			QPainter painter(&image);
			painter.setRenderHint(QPainter::Antialiasing, true);

			// 画线
			if (cfg.isDashed)
			{
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness, Qt::DashLine));
			}
			else
			{
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness));
			}
			QPoint p1(cfg.startPoint.first, cfg.startPoint.second);
			QPoint p2(cfg.endPoint.first, cfg.endPoint.second);
			painter.drawLine(p1, p2);

			// 画文字（如果有）
			if (!cfg.text.isEmpty())
			{
				// 字体大小：优先使用 cfg.fontSize（当 > 0 时），否则回退到基于线宽的默认值
				QFont font = painter.font();
				int fontSize = (cfg.fontSize > 0) ? cfg.fontSize : std::max(8, cfg.thickness * 4);
				font.setPointSize(fontSize);
				painter.setFont(font);

				QFontMetrics fm(font);
				int textW = fm.horizontalAdvance(cfg.text);
				int textH = fm.height();

				// 计算线段方向与垂直方向，用于将文字放在“线的上方”
				const double dx = static_cast<double>(p2.x() - p1.x());
				const double dy = static_cast<double>(p2.y() - p1.y());
				const double segLen = std::hypot(dx, dy);
				double ux = 0.0, uy = 0.0; // 单位方向向量
				if (segLen > 1e-6) { ux = dx / segLen; uy = dy / segLen; }

				// 垂直单元向量（朝上为负垂直）
				double px = -uy;
				double py = ux;

				// 偏移距离：在垂直方向上偏离线，使文字不与线重叠
				const double offset = std::max<double>(textH * 0.6, cfg.thickness + 4);

				// 计算基点（线上的坐标），根据 TextLocate 放左/中/右
				QPointF base;
				auto locate = cfg.textLocate;

				switch (locate)
				{
				case ConfigDrawSegment::TextLocate::Left:
					base = QPointF(p1) + QPointF(ux * std::min(10.0, segLen * 0.05), uy * std::min(10.0, segLen * 0.05));
					break;
				case ConfigDrawSegment::TextLocate::Right:
					base = QPointF(p2) - QPointF(ux * std::min(10.0, segLen * 0.05), uy * std::min(10.0, segLen * 0.05));
					break;
				case ConfigDrawSegment::TextLocate::Middle:
				default:
					base = (QPointF(p1) + QPointF(p2)) * 0.5;
					break;
				}

				// 将文字向“上方”偏移（使用垂直向量），注意 QImage 坐标系：y 向下为正，所以上方为负方向
				QPointF textPos = base + QPointF(px * (-offset), py * (-offset));

				// 将文字左上角调整，使文字居中于基点（横向居中，纵向以基点为基准向上偏移）
				QPointF drawTopLeft(textPos.x() - textW / 2.0, textPos.y() - textH / 2.0);

				// 设置文字颜色并绘制
				painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.textColor)));
				painter.drawText(QRectF(drawTopLeft, QSizeF(textW, textH)), Qt::AlignCenter, cfg.text);
			}
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const DetectionRectangleInfo& rectInfo,
		                                         const ConfigDrawRect& cfg)
		{
			QPainter painter(&image);

			if (cfg.isRegion)
			{

				painter.setRenderHint(QPainter::Antialiasing);
				painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

				// 构造多边形区域
				QPolygonF regionPolygon;
				regionPolygon << QPointF(rectInfo.leftTop.first, rectInfo.leftTop.second)
					<< QPointF(rectInfo.rightTop.first, rectInfo.rightTop.second)
					<< QPointF(rectInfo.rightBottom.first, rectInfo.rightBottom.second)
					<< QPointF(rectInfo.leftBottom.first, rectInfo.leftBottom.second);

				QColor fillColor = rw::rqw::RQWColorToQColor(cfg.rectColor);
				fillColor.setAlphaF(cfg.alpha); // alpha为0~1

				painter.setPen(Qt::NoPen);
				painter.setBrush(QBrush(fillColor));
				painter.drawPolygon(regionPolygon);


			}

			if (cfg.hasFrame)
			{
				if (cfg.isDashed)
				{
					painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.rectColor), cfg.thickness, Qt::DashLine));
				}
				else
				{
					painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.rectColor), cfg.thickness));
				}

				QPolygonF obbPolygon;
				obbPolygon << QPointF(rectInfo.leftTop.first, rectInfo.leftTop.second)
					<< QPointF(rectInfo.rightTop.first, rectInfo.rightTop.second)
					<< QPointF(rectInfo.rightBottom.first, rectInfo.rightBottom.second)
					<< QPointF(rectInfo.leftBottom.first, rectInfo.leftBottom.second);

				painter.drawPolygon(obbPolygon);

				if (cfg.text.isEmpty())
				{
					return;
				}

				painter.setPen(rw::rqw::RQWColorToQColor(cfg.textColor));
				QFont font = painter.font();
				font.setPixelSize(cfg.fontSize);
				painter.setFont(font);

				QPointF textPosition;
				int offset = cfg.fontSize;
				switch (cfg.textLocate) {
				case ConfigDrawRect::TextLocate::LeftTopIn:
					textPosition = QPointF(rectInfo.leftTop.first + 10, rectInfo.leftTop.second + offset);
					break;
				case ConfigDrawRect::TextLocate::LeftTopOut:
					textPosition = QPointF(rectInfo.leftTop.first, rectInfo.leftTop.second - 10);
					break;
				case ConfigDrawRect::TextLocate::RightTopIn:
					textPosition = QPointF(rectInfo.rightTop.first - offset * cfg.text.size() / 1.5, rectInfo.rightTop.second + offset);
					break;
				case ConfigDrawRect::TextLocate::RightTopOut:
					textPosition = QPointF(rectInfo.rightTop.first - offset * cfg.text.size() / 1.5, rectInfo.rightTop.second - 10);
					break;
				case ConfigDrawRect::TextLocate::LeftBottomIn:
					textPosition = QPointF(rectInfo.leftBottom.first + 10, rectInfo.leftBottom.second - 10);
					break;
				case ConfigDrawRect::TextLocate::LeftBottomOut:
					textPosition = QPointF(rectInfo.leftBottom.first, rectInfo.leftBottom.second + offset);
					break;
				case ConfigDrawRect::TextLocate::RightBottomIn:
					textPosition = QPointF(rectInfo.rightBottom.first - offset * cfg.text.size() / 1.5, rectInfo.rightBottom.second - 10);
					break;
				case ConfigDrawRect::TextLocate::RightBottomOut:
					textPosition = QPointF(rectInfo.rightBottom.first - offset * cfg.text.size() / 1.5, rectInfo.rightBottom.second + offset);
					break;
				case ConfigDrawRect::TextLocate::CenterIn:
					textPosition = QPointF(rectInfo.center_x, rectInfo.center_y);
					break;
				default:
					throw std::invalid_argument("Unsupported TextLocate type.");
				}

				painter.drawText(textPosition, cfg.text);
			}

			painter.end();

		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const rqw::PainterRectangleInfo& rectInfo,
			const ConfigDrawRect& cfg)
		{
			DetectionRectangleInfo info;
			info.leftTop = { (rectInfo.leftTop.first) * image.width(),(rectInfo.leftTop.second) * image.height() };
			info.rightTop = { (rectInfo.rightTop.first) * image.width(), (rectInfo.rightTop.second) * image.height() };
			info.leftBottom = { (rectInfo.leftBottom.first) * image.width(), (rectInfo.leftBottom.second) * image.height() };
			info.rightBottom = { (rectInfo.rightBottom.first) * image.width(), (rectInfo.rightBottom.second) * image.height() };
			info.center_x = rectInfo.center_x * image.width();
			info.center_y = rectInfo.center_y * image.height();
			info.width = std::abs(info.leftTop.first - info.rightTop.first);
			info.height = std::abs(info.leftTop.second - info.rightTop.second);
			info.area = static_cast<long>(rectInfo.area);
			info.classId = rectInfo.classId;
			info.score = rectInfo.score;
			info.segMaskValid = false;
			drawShapesOnSourceImg(image, info, cfg);
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const DetectionRectangleInfo& rectInfo,
			const ConfigDrawCircle& cfg)
		{
			QPainter painter(&image);
			painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness));
			int radius = std::min(rectInfo.width, rectInfo.height) / 2;
			painter.drawEllipse(
				QPointF(rectInfo.center_x, rectInfo.center_y),
				radius,
				radius
			);
			painter.end();
		}

		void ImagePainter::drawShapesOnSourceImg(QImage& image, const ConfigDrawCircleWithLocate& cfg)
		{
			QPainter painter(&image);
			painter.setPen(QPen(rw::rqw::RQWColorToQColor(cfg.color), cfg.thickness));
			painter.drawEllipse(cfg.center, cfg.radius, cfg.radius);
			painter.end();
		}

		void ImagePainter::drawTextOnImage(QImage& image, const QVector<QString>& texts,
		                                   const std::vector<Color>& colorList, double proportion)
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
				QColor color = (i < colorList.size()) ? rw::rqw::RQWColorToQColor(colorList[i]) : rw::rqw::RQWColorToQColor(colorList.back());
				painter.setPen(color);

				painter.drawText(x, y, texts[i]);

				y += fontSize;
			}

			painter.end();
		}

		void ImagePainter::drawTextOnImageWithFontSize(QImage& image, const QVector<QString>& texts,
			const std::vector<Color>& colorList, int fontSize)
		{
			if (texts.empty() || fontSize <= 0) {
				return;
			}

			QPainter painter(&image);
			painter.setRenderHint(QPainter::Antialiasing);

			QFont font = painter.font();
			font.setPixelSize(fontSize);
			painter.setFont(font);

			int x = 0;
			int y = fontSize;

			for (int i = 0; i < texts.size(); ++i) {
				QColor color = (i < static_cast<int>(colorList.size())) ? rw::rqw::RQWColorToQColor(colorList[i]) : rw::rqw::RQWColorToQColor(colorList.back());
				painter.setPen(color);

				painter.drawText(x, y, texts[i]);

				y += fontSize;
			}

			painter.end();
		}

		void ImagePainter::drawMaskOnSourceImg(QImage& image, const DetectionRectangleInfo& rectInfo,
			const ConfigDrawMask& cfg)
		{
			// 1. 检查 mask 是否为空
			if (rectInfo.mask_roi.empty()) {
				return;
			}

			// 2. 获取 ROI 区域
			QRect roi(rectInfo.roi.x, rectInfo.roi.y, rectInfo.roi.width, rectInfo.roi.height);
			if (!image.rect().contains(roi)) {
				return;
			}

			// 3. 将 mask_roi 转为 QImage
			QImage maskImg = rw::CvMatToQImage(rectInfo.mask_roi);

			// 4. 阈值处理
			for (int y = 0; y < maskImg.height(); ++y) {
				uchar* line = maskImg.scanLine(y);
				for (int x = 0; x < maskImg.width(); ++x) {
					line[x] = (line[x] > cfg.thresh) ? static_cast<uchar>(cfg.maxVal) : 0;
				}
			}

			// 5. 构造彩色遮罩
			QColor color = rw::rqw::RQWColorToQColor(cfg.maskColor);
			QImage colorMask(maskImg.size(), QImage::Format_ARGB32);
			colorMask.fill(Qt::transparent);
			for (int y = 0; y < maskImg.height(); ++y) {
				const uchar* maskLine = maskImg.constScanLine(y);
				QRgb* colorLine = reinterpret_cast<QRgb*>(colorMask.scanLine(y));
				for (int x = 0; x < maskImg.width(); ++x) {
					if (maskLine[x]) {
						colorLine[x] = QColor(color.red(), color.green(), color.blue(), static_cast<int>(cfg.alpha * 255)).rgba();
					}
				}
			}

			// 6. 叠加到原图 ROI 区域
			QPainter painter(&image);
			painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
			painter.drawImage(roi.topLeft(), colorMask);

			painter.end();

			if (cfg.hasFrame)
			{
				drawShapesOnSourceImg(image, rectInfo, cfg.rectCfg);
			}


		}
	}
}
