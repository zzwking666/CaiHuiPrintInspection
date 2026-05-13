#pragma once

#include "lgm_ILogger.hpp"
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "spdlog/sinks/rotating_file_sink.h"

namespace rw {
	namespace lgm {

		// 配置参数结构体
		struct RotatingFileSinkConfig {
			std::string filePath{ "logs/default.log" };
			size_t maxFileSize{ 2 * 1024 * 1024 };  // 10MB
			size_t maxFiles{ 30 };
			LogLevel level{ LogLevel::Info };
			std::string pattern{ "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [thread %t] %v" };
		};

		// 管理并复用 rotating_file_sink_mt 的单例管理器
		class RotatingFileSinkManager {
		public:
			// 单例访问
			static RotatingFileSinkManager& instance();

			// 获取默认共享 rotating file sink（创建时使用默认配置）
			std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> getSharedSink();

			// 获取或创建命名的 rotating file sink（不同 name 可用于不同配置）
			// 如果已存在，则返回已存在的 sink（不会修改其配置）
			std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> getOrCreateSink(const std::string& name);

			// 创建或覆盖命名 sink（强制设置配置参数）
			std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> createOrReplaceSink(
				const std::string& name,
				const RotatingFileSinkConfig& config);

			// 全局设置默认配置（仅对之后创建的 sink 生效）
			void setDefaultConfig(const RotatingFileSinkConfig& config);

			// 全局设置默认 pattern（仅对之后创建的 sink 生效）
			void setDefaultPattern(const std::string& pattern);

			// 全局设置默认 level（仅对之后创建的 sink 生效）
			void setDefaultLevel(LogLevel level);

		private:
			RotatingFileSinkManager();
			~RotatingFileSinkManager();

			RotatingFileSinkManager(const RotatingFileSinkManager&) = delete;
			RotatingFileSinkManager& operator=(const RotatingFileSinkManager&) = delete;

			std::map<std::string, std::shared_ptr<spdlog::sinks::rotating_file_sink_mt>> m_sinks;
			std::mutex m_mutex;

			RotatingFileSinkConfig m_defaultConfig;
		};

	} // namespace lgm
} // namespace rw