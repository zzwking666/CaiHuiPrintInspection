#include"ime_utilty.hpp"

namespace rw
{
	cv::Scalar ImagePainter::toScalar(BasicColor color)
	{
		switch (color) {
		case BasicColor::Red:        return cv::Scalar(0, 0, 255);   // BGR: Red
		case BasicColor::Green:      return cv::Scalar(0, 255, 0);   // BGR: Green
		case BasicColor::Blue:       return cv::Scalar(255, 0, 0);   // BGR: Blue
		case BasicColor::Yellow:     return cv::Scalar(0, 255, 255); // BGR: Yellow
		case BasicColor::Cyan:       return cv::Scalar(255, 255, 0); // BGR: Cyan
		case BasicColor::Magenta:    return cv::Scalar(255, 0, 255); // BGR: Magenta
		case BasicColor::White:      return cv::Scalar(255, 255, 255); // BGR: White
		case BasicColor::Black:      return cv::Scalar(0, 0, 0);     // BGR: Black
		case BasicColor::Orange:     return cv::Scalar(0, 165, 255); // BGR: Orange
		case BasicColor::LightBlue:  return cv::Scalar(255, 182, 193); // BGR: Light Blue
		case BasicColor::Gray:       return cv::Scalar(128, 128, 128); // BGR: Gray
		case BasicColor::Purple:     return cv::Scalar(128, 0, 128);   // BGR: Purple
		case BasicColor::Brown:      return cv::Scalar(42, 42, 165);   // BGR: Brown
		case BasicColor::LightBrown: return cv::Scalar(181, 229, 255); // BGR: Light Brown
		default:                     return cv::Scalar(0, 0, 0);     // Default to Black
		}
	}

	void ImagePainter::drawTextOnImage(cv::Mat& mat, const std::vector<std::string>& texts, const std::vector<PainterConfig>& colorList, double proportion)
	{
		if (texts.empty() || proportion <= 0.0 || proportion > 1.0 || mat.empty()) {
			return;
		}

		int imageHeight = mat.rows;
		int fontSize = static_cast<int>(imageHeight * proportion);

		int x = 10;
		int y = fontSize;

		for (size_t i = 0; i < texts.size(); ++i) {
			cv::Scalar textColor = (i < colorList.size()) ? colorList[i].textColor : colorList.back().textColor;

			cv::putText(
				mat,
				texts[i],
				cv::Point(x, y),
				cv::FONT_HERSHEY_SIMPLEX,
				proportion,
				textColor,
				colorList[i].fontThickness,
				cv::LINE_AA
			);

			y += fontSize + 5;
		}
	}

	cv::Mat ImagePainter::drawShapes(const cv::Mat& image, const std::vector<DetectionRectangleInfo>& rectInfo,
		PainterConfig config)
	{
		cv::Mat resultImage = image.clone();
		for (const auto& rect : rectInfo) {
			drawShapesOnSourceImg(resultImage, rect, config);
		}
		return resultImage;
	}

	void ImagePainter::drawShapesOnSourceImg(cv::Mat& image, const std::vector<DetectionRectangleInfo>& rectInfo,
		PainterConfig config)
	{
		for (const auto& rect : rectInfo) {
			drawShapesOnSourceImg(image, rect, config);
		}
	}

	void ImagePainter::drawShapesOnSourceImg(cv::Mat& image, const std::vector<std::vector<size_t>> index,
		const std::vector<DetectionRectangleInfo>& rectInfo, PainterConfig config)
	{
		for (const auto& classId : index) {
			for (const auto& item : classId)
			{
				config.text = std::to_string(rectInfo[item].classId);
				config.fontSize = 10;
				drawShapesOnSourceImg(image, rectInfo[item], config);
			}
		}
	}

	cv::Mat ImagePainter::drawShapes(
		const cv::Mat& image,
		const DetectionRectangleInfo& rectInfo,
		PainterConfig config
	) {
		cv::Mat resultImage = image.clone();

		drawShapesOnSourceImg(resultImage, rectInfo, config);

		return resultImage;
	}

	void ImagePainter::drawShapesOnSourceImg(cv::Mat& image, const DetectionRectangleInfo& rectInfo,
		PainterConfig config)
	{
		if (config.shapeType == ShapeType::Rectangle) {
			std::vector<cv::Point> pts{
	   cv::Point(rectInfo.leftTop.first, rectInfo.leftTop.second),
	   cv::Point(rectInfo.rightTop.first, rectInfo.rightTop.second),
	   cv::Point(rectInfo.rightBottom.first, rectInfo.rightBottom.second),
	   cv::Point(rectInfo.leftBottom.first, rectInfo.leftBottom.second)
			};
			const cv::Point* pts_array = pts.data();
			int npts = static_cast<int>(pts.size());
			cv::polylines(image, &pts_array, &npts, 1, true, config.color, config.thickness);
			/*cv::rectangle(
				image,
				cv::Point(static_cast<int>(rectInfo.leftTop.first), static_cast<int>(rectInfo.leftTop.second)),
				cv::Point(static_cast<int>(rectInfo.rightBottom.first), static_cast<int>(rectInfo.rightBottom.second)),
				config.color,
				config.thickness
			);*/
		}
		else if (config.shapeType == ShapeType::Circle) {
			cv::Point center(rectInfo.center_x, rectInfo.center_y);

			int radius = std::min(rectInfo.width, rectInfo.height) / 2;

			cv::circle(
				image,
				center,
				radius,
				config.color,
				config.thickness
			);
		}

		cv::putText(
			image,
			config.text,
			cv::Point(static_cast<int>(rectInfo.leftTop.first), static_cast<int>(rectInfo.leftTop.second) - 10),
			cv::FONT_HERSHEY_SIMPLEX,
			config.fontSize / 10.0,
			config.textColor,
			config.fontThickness
		);
	}

	void ImagePainter::drawMaskOnSourceImg(cv::Mat& image, const DetectionRectangleInfo& rectInfo, PainterConfig config)
	{
		if (rectInfo.mask_roi.empty())
		{
			return;
		}

		cv::Mat img_roi = image(rectInfo.roi);

		cv::Mat color_mask(img_roi.size(), img_roi.type(), config.color);

		cv::Mat mask;
		cv::threshold(rectInfo.mask_roi, mask, config.thresh, config.maxVal, cv::THRESH_BINARY);
		std::vector<cv::Mat> mask_channels(3, mask);
		cv::Mat mask_3channel;
		cv::merge(mask_channels, mask_3channel);

		if (color_mask.type() != img_roi.type()) {
			color_mask.convertTo(color_mask, img_roi.type());
		}
		if (mask_3channel.type() != img_roi.type()) {
			mask_3channel.convertTo(mask_3channel, img_roi.type());
		}

		cv::addWeighted(img_roi, 1.0, color_mask.mul(mask_3channel), config.alpha, 0, img_roi);
	}

	void ImagePainter::drawVerticalLine(cv::Mat& image, int position, const ImagePainter::PainterConfig& config)
	{
		if (image.empty()) {
			return;
		}

		if (position < 0 || position >= image.cols) {
			return;
		}

		cv::Scalar lineColor = config.color;
		int thickness = config.thickness;

		cv::line(image, cv::Point(position, 0), cv::Point(position, image.rows - 1), lineColor, thickness);
	}

	void ImagePainter::drawHorizontalLine(cv::Mat& image, int position, const ImagePainter::PainterConfig& config)
	{
		if (image.empty()) {
			return;
		}

		if (position < 0 || position >= image.rows) {
			return;
		}

		cv::Scalar lineColor = config.color;
		int thickness = config.thickness;

		cv::line(image, cv::Point(0, position), cv::Point(image.cols - 1, position), lineColor, thickness);
	}

	DetectionRectangleInfo::DetectionRectangleInfo(const DetectionRectangleInfo& other)
		: leftTop(other.leftTop), rightTop(other.rightTop), leftBottom(other.leftBottom), rightBottom(other.rightBottom),
		center_x(other.center_x), center_y(other.center_y), width(other.width), height(other.height), area(other.area),
		classId(other.classId), score(other.score), mask_roi(other.mask_roi.clone()), roi(other.roi), segMaskValid(other.segMaskValid),angle(other.angle){
	}

	DetectionRectangleInfo::DetectionRectangleInfo(DetectionRectangleInfo&& other) noexcept
		: leftTop(std::move(other.leftTop)), rightTop(std::move(other.rightTop)), leftBottom(std::move(other.leftBottom)), rightBottom(std::move(other.rightBottom)),
		center_x(other.center_x), center_y(other.center_y), width(other.width), height(other.height), area(other.area),
		classId(other.classId), score(other.score), mask_roi(std::move(other.mask_roi)), roi(std::move(other.roi)), segMaskValid(std::move(other.segMaskValid)),angle(other.angle){
	}

	DetectionRectangleInfo& DetectionRectangleInfo::operator=(const DetectionRectangleInfo& other)
	{
		if (this != &other)
		{
			leftTop = other.leftTop;
			rightTop = other.rightTop;
			leftBottom = other.leftBottom;
			rightBottom = other.rightBottom;
			center_x = other.center_x;
			center_y = other.center_y;
			width = other.width;
			height = other.height;
			area = other.area;
			classId = other.classId;
			score = other.score;
			mask_roi = other.mask_roi.clone();
			roi = other.roi;
			segMaskValid = other.segMaskValid;
			angle = other.angle;
		}
		return *this;
	}

	DetectionRectangleInfo& DetectionRectangleInfo::operator=(DetectionRectangleInfo&& other) noexcept
	{
		if (this != &other)
		{
			leftTop = std::move(other.leftTop);
			rightTop = std::move(other.rightTop);
			leftBottom = std::move(other.leftBottom);
			rightBottom = std::move(other.rightBottom);
			center_x = other.center_x;
			center_y = other.center_y;
			width = other.width;
			height = other.height;
			area = other.area;
			classId = other.classId;
			score = other.score;
			mask_roi = std::move(other.mask_roi);
			roi = other.roi;
			segMaskValid = other.segMaskValid;
			angle = other.angle;
		}
		return *this;
	}

	std::vector<rw::DetectionRectangleInfo>::const_iterator DetectionRectangleInfo::getMaxAreaRectangleIterator(
		const std::vector<rw::DetectionRectangleInfo>& bodyIndexVector)
	{
		if (bodyIndexVector.empty()) {
			return bodyIndexVector.end();
		}

		return std::max_element(
			bodyIndexVector.begin(),
			bodyIndexVector.end(),
			[](const rw::DetectionRectangleInfo& a, const rw::DetectionRectangleInfo& b) {
				return a.area < b.area;
			}
		);
	}

	std::vector<rw::DetectionRectangleInfo>::const_iterator DetectionRectangleInfo::getMaxAreaRectangleIterator(const std::vector<rw::DetectionRectangleInfo>& bodyIndexVector, const std::vector<size_t>& index)
	{
		if (bodyIndexVector.empty()) {
			return bodyIndexVector.end();
		}
		if (index.empty())
		{
			return bodyIndexVector.end();
		}
		auto currentArea = bodyIndexVector[index[0]].area;
		int maxIndex = index[0];
		for (int i = 0; i < index.size(); i++)
		{
			if (bodyIndexVector[index[i]].area > currentArea)
			{
				currentArea = bodyIndexVector[index[i]].area;
				maxIndex = index[i];
			}
		}

		return bodyIndexVector.begin() + maxIndex;
	}

	bool DetectionRectangleInfo::findDetIsInOtherDet(const DetectionRectangleInfo& obj,
		const DetectionRectangleInfo& region, int deviation)
	{
		auto tempRegion = region;
		if (tempRegion.leftTop.first - deviation < 0)
		{
			tempRegion.leftTop.first = 0;
		}
		else {
			tempRegion.leftTop.first = tempRegion.leftTop.first - deviation;
		}
		if (tempRegion.leftTop.second - deviation < 0)
		{
			tempRegion.leftTop.second = 0;
		}
		else {
			tempRegion.leftTop.second = tempRegion.leftTop.second - deviation;
		}

		tempRegion.rightBottom.first += deviation;
		tempRegion.rightBottom.second += deviation;

		auto leftTopResult = getPointRelativePositionByOther(obj.leftTop, region.leftTop);
		auto rightBottomResult = getPointRelativePositionByOther(obj.rightBottom, region.rightBottom);

		if (PointRelativePosition::RightBottom == leftTopResult &&
			PointRelativePosition::LeftTop == rightBottomResult)
		{
			return true;
		}

		return false;
	}

	 bool DetectionRectangleInfo::findPointIsInOtherDet(
		const Point& point,
		const DetectionRectangleInfo& region,
		int deviation
	) {
		auto tempRegion = region;
		if (tempRegion.leftTop.first - deviation < 0)
		{
			tempRegion.leftTop.first = 0;
		}
		else {
			tempRegion.leftTop.first = tempRegion.leftTop.first - deviation;
		}
		if (tempRegion.leftTop.second - deviation < 0)
		{
			tempRegion.leftTop.second = 0;
		}
		else {
			tempRegion.leftTop.second = tempRegion.leftTop.second - deviation;
		}

		tempRegion.rightBottom.first += deviation;
		tempRegion.rightBottom.second += deviation;

		auto leftTopResult = getPointRelativePositionByOther(point, region.leftTop);
		auto rightBottomResult = getPointRelativePositionByOther(point, region.rightBottom);

		if (PointRelativePosition::RightBottom == leftTopResult &&
			PointRelativePosition::LeftTop == rightBottomResult)
		{
			return true;
		}

		return false;
	}


	DetectionRectangleInfo::PointRelativePosition DetectionRectangleInfo::getPointRelativePositionByOther(
		const Point& first, const Point& other)
	{
		if (first.first == other.first && first.second == other.second)
		{
			return Overlap;
		}
		else if (first.first <= other.first && first.second <= other.second)
		{
			return LeftTop;
		}
		else if (first.first >= other.first && first.second <= other.second)
		{
			return RightTop;
		}
		else if (first.first <= other.first && first.second >= other.second)
		{
			return LeftBottom;
		}
		else if (first.first >= other.first && first.second >= other.second)
		{
			return RightBottom;
		}
		else if (first.first == other.first && first.second < other.second)
		{
			return Top;
		}
		else if (first.first > other.first && first.second == other.second)
		{
			return Right;
		}
		else if (first.first == other.first && first.second > other.second)
		{
			return Bottom;
		}
		else if (first.first < other.first && first.second == other.second)
		{
			return Left;
		}
	}
}
