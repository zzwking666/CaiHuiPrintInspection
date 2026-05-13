#include"rqw_rqwColor.hpp"

#include <stdexcept>

namespace rw
{
	namespace rqw
	{
		std::tuple<int, int, int> RQWColorToRGB(RQWColor color)
		{
			switch (color)
			{
			case RQWColor::Red:
				return { 255, 0, 0 };
			case RQWColor::Green:
				return { 0, 255, 0 };
			case RQWColor::Blue:
				return { 0, 0, 255 };
			case RQWColor::Yellow:
				return { 255, 255, 0 };
			case RQWColor::Cyan:
				return { 0, 255, 255 };
			case RQWColor::Magenta:
				return { 255, 0, 255 };
			case RQWColor::White:
				return { 255, 255, 255 };
			case RQWColor::Black:
				return { 0, 0, 0 };
			case RQWColor::Orange:
				return { 255, 165, 0 };
			case RQWColor::LightBlue:
				return { 173, 216, 230 };
			case RQWColor::Gray:
				return { 128, 128, 128 };
			case RQWColor::Purple:
				return { 128, 0, 128 };
			case RQWColor::Brown:
				return { 165, 42, 42 };
			case RQWColor::LightBrown:
				return { 210, 180, 140 };
			default:
				throw std::invalid_argument("Unsupported color value.");
			}
		}

		QColor RQWColorToQColor(RQWColor color)
		{
			auto [r, g, b] = RQWColorToRGB(color);
			return QColor(r, g, b);
		}
	}
}