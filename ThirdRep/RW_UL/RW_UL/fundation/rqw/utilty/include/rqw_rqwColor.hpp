#pragma once
#include <tuple>
#include<QColor>

namespace rw
{
	namespace rqw
	{
		enum class RQWColor {
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

		std::tuple<int, int, int> RQWColorToRGB(RQWColor color);
		QColor RQWColorToQColor(RQWColor color);
	}
}