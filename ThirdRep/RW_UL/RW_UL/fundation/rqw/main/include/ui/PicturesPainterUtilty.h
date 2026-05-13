#pragma once

#include <QColor>

namespace rw
{
	namespace rqw
	{
		struct RectangeConfig
		{
			int classid;
			QColor color;
			QString name;
			QString descrption;
		};

		struct PainterRectangleInfo
		{
		public:
			using Point = std::pair<double, double>;
		public:
			Point leftTop{};
			Point rightTop{};
			Point leftBottom{};
			Point rightBottom{};
		public:
			double center_x{ -1 };
			double center_y{ -1 };
		public:
			double width{ -1 };
			double height{ -1 };
		public:
			long area{ -1 };
		public:
			size_t classId{ 0 };
			double score{ -1 };
		};
	}
}

