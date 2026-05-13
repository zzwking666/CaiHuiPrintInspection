#pragma once

#include "lgm_ILogger.hpp"
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "spdlog/sinks/daily_file_sink.h"

namespace rw {
	namespace lgm {

		// 配置参数结构体
		struct DailyFileSinkConfig {
			std::string baseFilePath{ "logs/daily.log" };  // 基础文件路径
			int rotationHour{ 0 };      // 轮转时刻-小时（0-23）
			int rotationMinute{ 0 };    // 轮转时刻-分钟（0-59）
			bool truncate{ false };     // 是否截断文件
			uint16_t maxFiles{ 30 };     // 最大文件数（0表示不限制）
			LogLevel level{ LogLevel::Info };
			std::string pattern{ "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [thread %t] %v" };
		};

		// 管理并复用 daily_file_sink_mt 的单例管理器
		class DailyFileSinkManager {
		public:
			// 单例访问
			static DailyFileSinkManager& instance();

			// 获取默认共享 daily file sink（创建时使用默认配置）
			std::shared_ptr<spdlog::sinks::daily_file_sink_mt> getSharedSink();

			// 获取或创建命名的 daily file sink（不同 name 可用于不同配置）
			// 如果已存在，则返回已存在的 sink（不会修改其配置）
			std::shared_ptr<spdlog::sinks::daily_file_sink_mt> getOrCreateSink(const std::string& name);

			// 创建或覆盖命名 sink（强制设置配置参数）
			std::shared_ptr<spdlog::sinks::daily_file_sink_mt> createOrReplaceSink(
				const std::string& name,
				const DailyFileSinkConfig& config);

			// 全局设置默认配置（仅对之后创建的 sink 生效）
			void setDefaultConfig(const DailyFileSinkConfig& config);

			// 全局设置默认 pattern（仅对之后创建的 sink 生效）
			void setDefaultPattern(const std::string& pattern);

			// 全局设置默认 level（仅对之后创建的 sink 生效）
			void setDefaultLevel(LogLevel level);

		private:
			DailyFileSinkManager();
			~DailyFileSinkManager();

			DailyFileSinkManager(const DailyFileSinkManager&) = delete;
			DailyFileSinkManager& operator=(const DailyFileSinkManager&) = delete;

			std::map<std::string, std::shared_ptr<spdlog::sinks::daily_file_sink_mt>> m_sinks;
			std::mutex m_mutex;

			DailyFileSinkConfig m_defaultConfig;
		};

	} // namespace lgm
} // namespace rw