#pragma once

#include <map>
#include <string>
#include <memory>
#include <shared_mutex>
#include <optional>
#include "fmt/format.h"

namespace rw {
	namespace lgm {

		enum class LogLevel {
			Trace,
			Debug,
			Info,
			Warn,
			Error,
			Critical,
			Off
		};

		class ILogger {
		public:
			virtual ~ILogger() = default;

			// 基础日志接口
			virtual void trace(const std::string& message) = 0;
			virtual void debug(const std::string& message) = 0;
			virtual void info(const std::string& message) = 0;
			virtual void warn(const std::string& message) = 0;
			virtual void error(const std::string& message) = 0;
			virtual void critical(const std::string& message) = 0;

			// 带格式化参数的接口
			template<typename... Args>
			void trace(const std::string& fmt, Args&&... args) {
				auto formatted = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
				logFormatted(LogLevel::Trace, formatted);
			}

			template<typename... Args>
			void debug(const std::string& fmt, Args&&... args) {
				auto formatted = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
				logFormatted(LogLevel::Debug, formatted);
			}

			template<typename... Args>
			void info(const std::string& fmt, Args&&... args) {
				auto formatted = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
				logFormatted(LogLevel::Info, formatted);
			}

			template<typename... Args>
			void warn(const std::string& fmt, Args&&... args) {
				auto formatted = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
				logFormatted(LogLevel::Warn, formatted);
			}

			template<typename... Args>
			void error(const std::string& fmt, Args&&... args) {
				auto formatted = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
				logFormatted(LogLevel::Error, formatted);
			}

			template<typename... Args>
			void critical(const std::string& fmt, Args&&... args) {
				auto formatted = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
				logFormatted(LogLevel::Critical, formatted);
			}

			virtual void setLevel(LogLevel level) = 0;
			virtual void flush() = 0;

		protected:
			virtual void logFormatted(LogLevel level, const std::string& message) = 0;
		};

		/**
		 * @brief 线程安全的日志管理器
		 */
		class LoggerManager {
		public:
			static LoggerManager& getInstance();

			void registerLogger(const std::string& name, std::shared_ptr<ILogger> logger);

			std::shared_ptr<ILogger> getLogger(const std::string& name) const;

			void removeLogger(const std::string& name);

			void removeAllLoggers();

			bool hasLogger(const std::string& name) const;

		private:
			LoggerManager() = default;
			~LoggerManager() = default;
			LoggerManager(const LoggerManager&) = delete;
			LoggerManager& operator=(const LoggerManager&) = delete;

			std::map<std::string, std::shared_ptr<ILogger>> m_loggers;
			mutable std::shared_mutex m_mutex;  
		};

	} // namespace lgm
} // namespace rw