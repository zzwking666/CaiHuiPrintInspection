#include"hoei_HardwareInfo.hpp"

#include <windows.h>
#include <intrin.h>
#include <sstream>
#include <stdexcept>


namespace rw
{
	namespace hoei
	{
		HardwareInfo::HardwareInfo(const HardwareInfo& other)
			: cpu(other.cpu), motherBoard(other.motherBoard) {
		}

		HardwareInfo::HardwareInfo(HardwareInfo&& other) noexcept
			: cpu(std::move(other.cpu)), motherBoard(std::move(other.motherBoard)) {
		}

		HardwareInfo& HardwareInfo::operator=(const HardwareInfo& other)
		{
			if (this != &other) {
				cpu = other.cpu;
				motherBoard = other.motherBoard;
			}
			return *this;
		}

		HardwareInfo& HardwareInfo::operator=(HardwareInfo&& other) noexcept
		{
			if (this != &other) {
				cpu = std::move(other.cpu);
				motherBoard = std::move(other.motherBoard);
			}
			return *this;
		}

		void HardwareInfo::getCurrentContextInfo()
		{
			*this = HardwareInfoFactory::createHardwareInfo();
		}

		HardwareInfo HardwareInfoFactory::createHardwareInfo()
		{
			HardwareInfo info;
			info.cpu.getCurrentContextInfo();
			info.motherBoard.getCurrentContextInfo();
			return info;
		}
	}
}