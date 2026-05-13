#pragma once
#include <cstdint>

namespace rw
{
	namespace hoes
	{
		enum class ByteSize
		{
			FiveBits = 5,
			SixBits = 6,
			SevenBits = 7,
			EightBits = 8
		};

		enum class Parity
		{
			None = 0,
			Odd = 1,
			Even = 2,
			Mark = 3,
			Space = 4
		};

		enum class StopBits
		{
			One = 1,
			Two = 2,
			One_Point_Five
		};

		enum class FlowControl
		{
			None = 0,
			Software,
			Hardware
		};
	}
}