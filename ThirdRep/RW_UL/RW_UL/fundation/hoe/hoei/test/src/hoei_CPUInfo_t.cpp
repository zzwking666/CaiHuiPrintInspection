#include"hoei_utilty_t.hpp"

#include"hoei_CPUInfo.hpp"

namespace hoei_HardwareInfo
{
	TEST(CPUInfo, getCpuInfo)
	{
		rw::hoei::CPUInfo  cpuInfo;
		cpuInfo.getCurrentContextInfo();
		std::cout << "CpuModel:" << cpuInfo.cpuModel << std::endl;
		std::cout << "BaseClockSpeed:" << cpuInfo.baseClockSpeed << " MHz" << std::endl;
		std::cout << "CoreCount:" << cpuInfo.coreCount << std::endl;
		std::cout << "LogicCoreCount:" << cpuInfo.logicCoreCount << std::endl;
		std::cout << "ThreadCount:" << cpuInfo.threadCount << std::endl;

		for (const auto& item : cpuInfo.topology)
		{
			std::cout << item.size << " ";
			std::cout << item.depth << " ";
			std::cout << item.description << " ";
			std::cout << item.index << " ";
			std::cout << item.type << std::endl;
		}
	}
}