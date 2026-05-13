#pragma once

#include "lgm_ILogger.hpp"
#include "lgms_SpdlogModuleLogger.hpp"
#include <memory>
#include <mutex>


namespace rw
{
	namespace lgm
	{


		inline constexpr const char* moduleName = "RW_UL.foundation";

		using LogLevel = lgm::LogLevel;

		inline struct LoggerCfg
		{
			bool enableFile{ true };

			std::string baseFilePath{ "logs/foundation/foundation_daily.log" };  // 基础文件路径
			int rotationHour{ 0 };      // 轮转时刻-小时（0-23）
			int rotationMinute{ 0 };    // 轮转时刻-分钟（0-59）
			bool truncate{ false };     // 是否截断文件
			uint16_t maxFiles{ 30 };     // 最大文件数（0表示不限制）
			LogLevel fileSinkLevel{ LogLevel::Info };

			bool enableConsole{ true };
			LogLevel consoleSinkLevel{ LogLevel::Info };
			bool async{ true };

			LogLevel level{ LogLevel::Info };
			std::string pattern{ "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [thread %t] %v" };
		}loggerCfg;

		void iniLogger(const LoggerCfg & cfg);

		class Logger : public lgm::ILogger
		{
		public:
			using lgm::ILogger::trace;
			using lgm::ILogger::debug;
			using lgm::ILogger::info;
			using lgm::ILogger::warn;
			using lgm::ILogger::error;
			using lgm::ILogger::critical;
		public:
			static Logger& getInstance();

			Logger(const Logger&) = delete;
			Logger& operator=(const Logger&) = delete;

			Logger(Logger&&) = delete;
			Logger& operator=(Logger&&) = delete;

		public:
			void trace(const std::string& message) override;
			void debug(const std::string& message) override;
			void info(const std::string& message) override;
			void warn(const std::string& message) override;
			void error(const std::string& message) override;
			void critical(const std::string& message) override;

			void setLevel(LogLevel level) override;
			void flush() override;

		protected:
			void logFormatted(LogLevel level, const std::string& message) override;

		private:
			Logger();
			~Logger();

		private:
			std::shared_ptr<lgm::SpdlogModuleLogger> m_logger;
		};

	} // namespace imgPro
} // namespace rw