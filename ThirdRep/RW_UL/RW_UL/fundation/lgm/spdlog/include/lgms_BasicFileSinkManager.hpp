#pragma once

#include "lgm_ILogger.hpp"
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "spdlog/sinks/basic_file_sink.h"

namespace rw {
	namespace lgm {

		struct BasicFileSinkConfig {
			std::string filePath{ "logs/default.log" };
			bool truncate{ false };  // 是否截断文件（true=覆盖，false=追加）
			LogLevel level{ LogLevel::Info };
			std::string pattern{ "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [thread %t] %v" };
		};

		// 管理并复用 basic_file_sink_mt 的单例管理器
		class BasicFileSinkManager {
		public:
			// 单例访问
			static BasicFileSinkManager& instance();

			// 获取默认共享 basic file sink（创建时使用默认配置）
			std::shared_ptr<spdlog::sinks::basic_file_sink_mt> getSharedSink();

			// 获取或创建命名的 basic file sink（不同 name 可用于不同配置）
			// 如果已存在，则返回已存在的 sink（不会修改其配置）
			std::shared_ptr<spdlog::sinks::basic_file_sink_mt> getOrCreateSink(const std::string& name);

			// 创建或覆盖命名 sink（强制设置配置参数）
			std::shared_ptr<spdlog::sinks::basic_file_sink_mt> createOrReplaceSink(
				const std::string& name,
				const BasicFileSinkConfig& config);

			// 全局设置默认配置（仅对之后创建的 sink 生效）
			void setDefaultConfig(const BasicFileSinkConfig& config);

			// 全局设置默认 pattern（仅对之后创建的 sink 生效）
			void setDefaultPattern(const std::string& pattern);

			// 全局设置默认 level（仅对之后创建的 sink 生效）
			void setDefaultLevel(LogLevel level);

		private:
			BasicFileSinkManager();
			~BasicFileSinkManager();

			BasicFileSinkManager(const BasicFileSinkManager&) = delete;
			BasicFileSinkManager& operator=(const BasicFileSinkManager&) = delete;

			std::map<std::string, std::shared_ptr<spdlog::sinks::basic_file_sink_mt>> m_sinks;
			std::mutex m_mutex;

			BasicFileSinkConfig m_defaultConfig;
		};

	} // namespace lgm
} // namespace rw