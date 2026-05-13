#pragma once

namespace rw
{
	namespace rqw
	{
		struct HalconShapeXLDConfig
		{
		public:
			int minXldLenth = 10;
		};

		struct HalconShapeXLDFindConfig
		{
		public:

			int	findNumber = 1;
			double	minScore = 0.5;
		};
	}
}