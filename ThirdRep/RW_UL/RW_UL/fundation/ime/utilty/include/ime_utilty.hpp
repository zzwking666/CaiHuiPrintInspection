#pragma once

#include<string>

#include"opencv2/opencv.hpp"

namespace rw {
	/**
	 * @brief Object detection rectangle information
	 *
	 * For most of the object detection models, the detection result is a rectangle.
	 * This class is used to store the information of the rectangle.
	 * For ime package, the detection result us DetectionRectangleInfo.
	 *
	 * @Warning
	 * Detection:center_x, center_y is source output from the model,other is calculated
	 * Oriented Bounding Boxes: center_x, center_y is source output from the model,leftTop, rightTop, leftBottom, rightBottom is calculated by angle
	 * Segmentation: center_x, center_y is source output from the model,other is calculated
	 */
	struct DetectionRectangleInfo
	{
	public:
		using Point = std::pair<int, int>;
	public:
		Point leftTop{};
		Point rightTop{};
		Point leftBottom{};
		Point rightBottom{};
	public:
		int center_x{ -1 };
		int center_y{ -1 };
	public:
		int width{ -1 };
		int height{ -1 };
	public:
		double angle{0};
	public:
		long area{ -1 };
	public:
		size_t classId{ 0 };
		double score{ -1 };
	public:
		cv::Mat mask_roi;//if it is seg
		cv::Rect roi;
	public:
		bool segMaskValid{ false };
	public:
		DetectionRectangleInfo() = default;

		DetectionRectangleInfo(const DetectionRectangleInfo& other);
		DetectionRectangleInfo(DetectionRectangleInfo&& other) noexcept;
		DetectionRectangleInfo& operator=(const DetectionRectangleInfo& other);
		DetectionRectangleInfo& operator=(DetectionRectangleInfo&& other) noexcept;
		~DetectionRectangleInfo() = default;
	public:
		/**
		 *@Parameters:
		 *  bodyIndexVector: a vector of DetectionRectangleInfo
		 *
		 *@Methods:
		 *  Get the maximum area rectangle iterator from the vector
		 *@Returns:
		 *	The maximum area rectangle iterator from the vector
		 */
		static std::vector<rw::DetectionRectangleInfo>::const_iterator getMaxAreaRectangleIterator(
			const std::vector<rw::DetectionRectangleInfo>& bodyIndexVector);
		/**
		 *@Parameters:
		 *  bodyIndexVector: a vector of DetectionRectangleInfo
		 *	index: a vector of index
		 *@Methods:
		 *  Get the maximum area rectangle iterator from the index of the vector
		 *@Returns:
		 *	The maximum area rectangle iterator from the vector
		 */
		static std::vector<rw::DetectionRectangleInfo>::const_iterator getMaxAreaRectangleIterator(
			const std::vector<rw::DetectionRectangleInfo>& bodyIndexVector, const std::vector<size_t>& index);

		static bool findDetIsInOtherDet(
			const DetectionRectangleInfo& obj,
				const DetectionRectangleInfo& region,	
			int deviation
		);

		static bool findPointIsInOtherDet(
			const Point& point,
			const DetectionRectangleInfo& region,
			int deviation
		);
	public:
		enum PointRelativePosition
		{
			LeftTop,
			RightTop,
			LeftBottom,
			RightBottom,
			Top,
			Right,
			Bottom,
			Left,
			Overlap
		};
		static PointRelativePosition getPointRelativePositionByOther(const Point& first, const Point& other);
	};

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

		static cv::Scalar toScalar(BasicColor color);

		struct PainterConfig
		{
			// ShapeType::Rectangle or ShapeType::Circle
			ShapeType shapeType{ ShapeType::Rectangle };
			// Color of the word
			int fontSize = 5;
			// Thickness of the word
			int fontThickness = 1;
			// Thickness of the shape
			int thickness = 2;
			// Text to be drawn
			std::string text;
			// Color of the shape
			cv::Scalar color{ 0, 0, 255 };
			// Color of the word
			cv::Scalar textColor{ 0, 255, 0 };
		public:
			//For mask painter
			double alpha{ 0.3 };
			double thresh{ 0.5 };
			double maxVal{ 1.0 };
		};

		/**
		 * @Parameters:
		 *   mat: The input/output cv::Mat image on which the text will be drawn.
		 *   texts: A vector of strings, each representing a line of text to be drawn.
		 *   colorList: A vector of PainterConfig, specifying the color and font thickness for each line of text.
		 *              If the number of texts exceeds colorList, the last PainterConfig is used for the remaining lines.
		 *   proportion: The font size as a proportion of the image height, must be in the range (0, 1].
		 * @Methods:
		 *   Draws each string from texts onto the image mat, starting from the top-left corner,
		 *   using the corresponding color and font settings from colorList. The vertical spacing
		 *   between lines is determined by the calculated font size.
		 * @Returns:
		 *   None. The function draws text directly on the input mat image.
		 */
		static void drawTextOnImage(cv::Mat& mat, const std::vector<std::string>& texts, const std::vector<PainterConfig>& colorList, double proportion = 0.8);

		/**
		 * @Parameters:
		 *   image: The input cv::Mat image on which shapes will be drawn (the original image is not modified).
		 *   rectInfo: A vector of DetectionRectangleInfo, each representing a rectangle or shape to be drawn.
		 *   config: PainterConfig specifying drawing parameters such as color, thickness, and shape type.
		 * @Methods:
		 *   Draws shapes (e.g., oriented rectangles or circles) for each DetectionRectangleInfo in rectInfo
		 *   onto a copy of the input image, using the provided drawing configuration.
		 * @Returns:
		 *   A new cv::Mat image with all specified shapes drawn on it.
		 */
		static cv::Mat drawShapes(
			const cv::Mat& image,
			const std::vector<DetectionRectangleInfo>& rectInfo,
			PainterConfig config
		);

		/**
		 * @Parameters:
		 *   image: The input/output cv::Mat image on which the shapes will be drawn.
		 *   rectInfo: A vector of DetectionRectangleInfo, each representing a rectangle or shape to be drawn.
		 *   config: PainterConfig specifying drawing parameters such as color, thickness, and shape type.
		 * @Methods:
		 *   Draws shapes (e.g., oriented rectangles or circles) for each DetectionRectangleInfo in rectInfo
		 *   directly onto the input image, using the provided drawing configuration.
		 * @Returns:
		 *   None. The function draws shapes directly on the input image.
		 */
		static void drawShapesOnSourceImg(
			cv::Mat& image,
			const std::vector<DetectionRectangleInfo>& rectInfo,
			PainterConfig config = {}
		);

		/**
		 * @Parameters:
		 *   image: The input/output cv::Mat image on which the shapes will be drawn.
		 *   index: A vector of vectors, where each inner vector contains indices referring to rectInfo.
		 *   rectInfo: A vector of DetectionRectangleInfo, each representing a rectangle or shape to be drawn.
		 *   config: PainterConfig specifying drawing parameters such as color, thickness, and shape type.
		 * @Methods:
		 *   For each group in index, draws shapes (e.g., oriented rectangles or circles) for the corresponding DetectionRectangleInfo
		 *   directly onto the input image, using the provided drawing configuration.
		 * @Returns:
		 *   None. The function draws shapes directly on the input image.
		 */
		static void drawShapesOnSourceImg(cv::Mat& image, const std::vector<std::vector<size_t>> index, const std::vector<DetectionRectangleInfo>& rectInfo,
			PainterConfig config = {});

		/// Draw shapes on a copy of the image
		static cv::Mat drawShapes(
			const cv::Mat& image,
			const DetectionRectangleInfo& rectInfo,
			PainterConfig config = {}
		);

		/// Draw shapes directly on the source image
		static void drawShapesOnSourceImg(
			cv::Mat& image,
			const DetectionRectangleInfo& rectInfo,
			PainterConfig config = {}
		);

		static void drawMaskOnSourceImg(
			cv::Mat& image,
			const DetectionRectangleInfo& rectInfo,
			PainterConfig config = {}
		);

		/// Draw a vertical line on the image at the specified position
		static  void drawVerticalLine(cv::Mat& image, int position, const ImagePainter::PainterConfig& config);
		/// Draw a horizontal line on the image at the specified position
		static void drawHorizontalLine(cv::Mat& image, int position, const ImagePainter::PainterConfig& config);
	};
}