#include"hoei_utilty_t.hpp"

#include"hoei_HardwareInfo.hpp"

namespace hoei_HardwareInfo
{
	TEST(HardwareInfo, getHardwareInfo)
	{
		rw::hoei::HardwareInfo  hardwareInfo;

		std::cout << "CpuModel:" << hardwareInfo.cpu.cpuModel << std::endl;
		std::cout << "BaseClockSpeed:" << hardwareInfo.cpu.baseClockSpeed << " MHz" << std::endl;
		std::cout << "CoreCount:" << hardwareInfo.cpu.coreCount << std::endl;
		std::cout << "LogicCoreCount:" << hardwareInfo.cpu.logicCoreCount << std::endl;
		std::cout << "ThreadCount:" << hardwareInfo.cpu.threadCount << std::endl;

		for (const auto& item : hardwareInfo.cpu.topology)
		{
			std::cout << item.size << " ";
			std::cout << item.depth << " ";
			std::cout << item.description << " ";
			std::cout << item.index << " ";
			std::cout << item.type << std::endl;
		}
		std::cout << "MotherboardUniqueID:" << hardwareInfo.motherBoard.UUID << std::endl;
	}
}