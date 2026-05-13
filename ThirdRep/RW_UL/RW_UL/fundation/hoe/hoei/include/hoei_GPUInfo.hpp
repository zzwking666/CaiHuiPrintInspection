#pragma once
#include <string>
#include <vector>

namespace rw {
	namespace hoei {
		struct GPUInfo
		{
		public:
			GPUInfo();
		public:
			std::string cpuModel{};
			static std::string GetCPUModel();
		public:
			size_t coreCount{};
			static size_t GetCoreCount();
		public:
			size_t logicCoreCount{};
			static size_t GetLogicCount();
		public:
			size_t threadCount{};
			static size_t GetThreadCount();
		public:
			//MHz
			double baseClockSpeed{};
			static double GetBaseClockSpeed();
		public:
			struct Topology {
				std::string type; // 类型，例如 "L1 Cache", "Core", "NUMA Node"
				size_t depth;     // 深度
				size_t index;     // 索引
				double size;      // 大小（例如缓存大小，单位 KB）
				std::string description; // 描述信息
			};
			std::vector<Topology> topology;
			static std::vector<Topology> GeyTopology();
		};
	}
}