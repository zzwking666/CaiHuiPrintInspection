#pragma once

#include <cstdint>
#include <cstring>

namespace rw
{
	namespace hoem
	{
		using Address16 = uint16_t;
		using Quantity = uint16_t;
		using UInt16 = uint16_t;
		using UInt32 = uint32_t;


		enum class Endianness
		{
			BigEndian,    // 大端
			LittleEndian  // 小端
		};


		inline UInt32 floatToUInt32(float f)
		{
			UInt32 u{};
			std::memcpy(&u, &f, sizeof(u));
			return u;
		}

		inline float uint32ToFloat(UInt32 u)
		{
			float f{};
			std::memcpy(&f, &u, sizeof(f));
			return f;
		}
	}
}
