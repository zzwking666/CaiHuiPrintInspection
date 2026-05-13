#include"hoei_CPUInfo.hpp"

#include <intrin.h>
#include <windows.h>
#include <thread>
#include <vector>

namespace rw
{
	namespace hoei
	{
		CPUInfo::CPUInfo(const CPUInfo& other)
			: cpuModel(other.cpuModel), coreCount(other.coreCount), logicCoreCount(other.logicCoreCount),
			threadCount(other.threadCount), baseClockSpeed(other.baseClockSpeed), topology(other.topology) {
		}

		CPUInfo::CPUInfo(CPUInfo&& other) noexcept
			: cpuModel(std::move(other.cpuModel)), coreCount(other.coreCount), logicCoreCount(other.logicCoreCount),
			threadCount(other.threadCount), baseClockSpeed(other.baseClockSpeed), topology(std::move(other.topology)) {
		}

		CPUInfo& CPUInfo::operator=(const CPUInfo& other)
		{
			if (this != &other) {
				cpuModel = other.cpuModel;
				coreCount = other.coreCount;
				logicCoreCount = other.logicCoreCount;
				threadCount = other.threadCount;
				baseClockSpeed = other.baseClockSpeed;
				topology = other.topology;
			}
			return *this;
		}

		CPUInfo& CPUInfo::operator=(CPUInfo&& other) noexcept
		{
			if (this != &other) {
				cpuModel = std::move(other.cpuModel);
				coreCount = other.coreCount;
				logicCoreCount = other.logicCoreCount;
				threadCount = other.threadCount;
				baseClockSpeed = other.baseClockSpeed;
				topology = std::move(other.topology);
			}
			return *this;
		}

		void CPUInfo::getCurrentContextInfo()
		{
			auto info = CPUInfoFactory::createCPUInfo();
			*this = info;
		}

		CPUInfo CPUInfoFactory::createCPUInfo()
		{
			CPUInfo info;
			info.cpuModel = CPUInfoFactory::GetCPUModel();
			info.coreCount = CPUInfoFactory::GetCoreCount();
			info.logicCoreCount = CPUInfoFactory::GetLogicCount();
			info.threadCount = CPUInfoFactory::GetThreadCount();
			info.baseClockSpeed = CPUInfoFactory::GetBaseClockSpeed();
			info.topology = CPUInfoFactory::GeyTopology();
			return info;
		}

		std::string CPUInfoFactory::GetCPUModel()
		{
			char cpuInfo[0x40] = { 0 };
			int cpuInfoData[4] = { 0 };

			__cpuid(cpuInfoData, 0x80000002);
			memcpy(cpuInfo, cpuInfoData, sizeof(cpuInfoData));

			__cpuid(cpuInfoData, 0x80000003);
			memcpy(cpuInfo + 16, cpuInfoData, sizeof(cpuInfoData));

			__cpuid(cpuInfoData, 0x80000004);
			memcpy(cpuInfo + 32, cpuInfoData, sizeof(cpuInfoData));

			return std::string(cpuInfo);
		}

		size_t CPUInfoFactory::GetCoreCount() {
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);

			DWORD_PTR processAffinityMask = 0, systemAffinityMask = 0;
			GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask);

			// Windows 10+ 可用 GetLogicalProcessorInformationEx
			DWORD len = 0;
			GetLogicalProcessorInformation(nullptr, &len);
			std::vector<BYTE> buffer(len);
			PSYSTEM_LOGICAL_PROCESSOR_INFORMATION info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.data());
			if (GetLogicalProcessorInformation(info, &len)) {
				size_t coreCount = 0;
				DWORD count = len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
				for (DWORD i = 0; i < count; ++i) {
					if (info[i].Relationship == RelationProcessorCore)
						++coreCount;
				}
				return coreCount;
			}
			return sysInfo.dwNumberOfProcessors; // 兜底返回逻辑处理器数
		}

		size_t CPUInfoFactory::GetLogicCount() {
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			return sysInfo.dwNumberOfProcessors;
		}

		size_t CPUInfoFactory::GetThreadCount() {
			return std::thread::hardware_concurrency();
		}

		double CPUInfoFactory::GetBaseClockSpeed()
		{
			HKEY hKey;
			DWORD data;
			DWORD dataSize = sizeof(data);

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"Hardware\\Description\\System\\CentralProcessor\\0",
				0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				if (RegQueryValueEx(hKey, "~MHz", nullptr, nullptr, reinterpret_cast<LPBYTE>(&data), &dataSize) == ERROR_SUCCESS) {
					RegCloseKey(hKey);
					return static_cast<double>(data);
				}
				RegCloseKey(hKey);
			}
			return 0.0;
		}

		std::vector<CPUInfo::Topology> CPUInfoFactory::GeyTopology()
		{
			std::vector<CPUInfo::Topology> topology;

			DWORD len = 0;
			GetLogicalProcessorInformation(nullptr, &len);
			std::vector<BYTE> buffer(len);
			PSYSTEM_LOGICAL_PROCESSOR_INFORMATION info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.data());
			if (GetLogicalProcessorInformation(info, &len)) {
				DWORD count = len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
				for (DWORD i = 0; i < count; ++i) {
					CPUInfo::Topology topo;
					switch (info[i].Relationship) {
					case RelationProcessorCore:
						topo.type = "Core";
						break;
					case RelationNumaNode:
						topo.type = "NUMA Node";
						break;
					case RelationCache:
						switch (info[i].Cache.Level) {
						case 1: topo.type = "L1 Cache"; break;
						case 2: topo.type = "L2 Cache"; break;
						case 3: topo.type = "L3 Cache"; break;
						default: topo.type = "Cache"; break;
						}
						topo.size = static_cast<double>(info[i].Cache.Size) / 1024.0;
						break;
					default:
						topo.type = "Other";
						break;
					}
					topo.depth = info[i].Relationship;
					topo.index = i;
					topo.description = "";
					topology.push_back(topo);
				}
			}
			return topology;
		}
	}
}