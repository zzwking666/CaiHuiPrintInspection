#pragma once

#include "lgm_ILogger.hpp"
#include <memory>

namespace spdlog {
	class logger;
}

namespace rw
{
	namespace lgm
	{
		// 文件 Sink 类型枚举
		enum class FileSinkType {
			Basic,      // 基础文件 sink
			Rotating,   // 循环文件 sink（默认）
			Daily,      // 每日文件 sink
			Hourly      // 每小时文件 sink
		};

		struct SpdlogModuleLoggerConfig {
			bool enableConsole{ true };
			LogLevel consoleSinkLevel{ LogLevel::Info };

			bool enableFile{ true };
			FileSinkType fileSinkType{ FileSinkType::Daily };  
			std::string fileSinkID{};//if is empty , use default file sink
			bool async{ true };

			LogLevel level{ LogLevel::Info };
			std::string pattern{ "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [thread %t] %v" };
		};

		class SpdlogModuleLogger : public ILogger
		{
		public:
			using ILogger::trace;
			using ILogger::debug;
			using ILogger::info;
			using ILogger::warn;
			using ILogger::error;
			using ILogger::critical;
		public:
			static std::shared_ptr<SpdlogModuleLogger> create(const std::string& moduleName, const SpdlogModuleLoggerConfig& cfg);
			~SpdlogModuleLogger() override;
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
			class SpdlogModuleImpl;
			std::unique_ptr<SpdlogModuleImpl> m_pImpl;

			explicit SpdlogModuleLogger(std::unique_ptr<SpdlogModuleImpl> impl);

		};
	}
}