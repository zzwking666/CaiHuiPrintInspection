#pragma once

#include "lgm_ILogger.hpp"
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include"spdlog/sinks/stdout_color_sinks.h"

namespace rw {
	namespace lgm {

		// 管理并复用 stdout_color_sink_mt 的单例管理器
		class ConsoleSinkManager {
		public:
			// 单例访问
			static ConsoleSinkManager& instance();

			// 获取默认共享 console sink（创建时使用默认 pattern/level）
			std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> getSharedSink();

			// 获取或创建命名的 console sink（不同 name 可用于不同格式/级别）
			// 如果已存在，则返回已存在的 sink（不会修改其 level/pattern）
			std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> getOrCreateSink(const std::string& name);

			// 创建或覆盖命名 sink（强制设置 level 与 pattern）
			std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> createOrReplaceSink(
				const std::string& name,
				LogLevel level,
				const std::string& pattern);

			// 全局设置默认 pattern（仅对之后创建的 sink 生效）
			void setDefaultPattern(const std::string& pattern);

			// 全局设置默认 level（仅对之后创建的 sink 生效）
			void setDefaultLevel(LogLevel level);

		private:
			ConsoleSinkManager();
			~ConsoleSinkManager();

			ConsoleSinkManager(const ConsoleSinkManager&) = delete;
			ConsoleSinkManager& operator=(const ConsoleSinkManager&) = delete;

			std::map<std::string, std::shared_ptr<spdlog::sinks::stdout_color_sink_mt>> m_sinks;
			std::mutex m_mutex;

			std::string m_defaultPattern;
			LogLevel m_defaultLevel;
		};

	} // namespace lgm
} // namespace rw