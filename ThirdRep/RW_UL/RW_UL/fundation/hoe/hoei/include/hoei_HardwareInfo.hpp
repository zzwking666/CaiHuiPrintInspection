#pragma once

#include <string>
#include <vector>

#include"hoei_CPUInfo.hpp"
#include"hoei_MotherBoardInfo.hpp"

namespace rw {
	namespace hoei {
		struct HardwareInfo
		{
		public:
			CPUInfo cpu{};
			MotherBoardInfo motherBoard{};
		public:
			HardwareInfo() = default;

			HardwareInfo(const HardwareInfo& other);

			HardwareInfo(HardwareInfo&& other) noexcept;

			HardwareInfo& operator=(const HardwareInfo& other);

			HardwareInfo& operator=(HardwareInfo&& other) noexcept;
		public:
			void getCurrentContextInfo();
		};

		class HardwareInfoFactory
		{
		public:
			static HardwareInfo createHardwareInfo();
		};
	}
}