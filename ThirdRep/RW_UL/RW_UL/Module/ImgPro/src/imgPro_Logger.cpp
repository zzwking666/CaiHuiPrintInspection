#include "imgPro_Logger.hpp"

#include"lgms_RotatingFileSinkManager.hpp"

namespace rw
{
	namespace imgPro
	{
		Logger& Logger::getInstance()
		{
			static Logger instance;
			return instance;
		}

		Logger::Logger()
		{
			lgm::RotatingFileSinkConfig rotatingFileSinkConfig;
			rotatingFileSinkConfig.level = loggerCfg.fileSinkLevel;
			rotatingFileSinkConfig.filePath = loggerCfg.filePath;
			rotatingFileSinkConfig.maxFileSize = loggerCfg.maxFileSize; 
			rotatingFileSinkConfig.maxFiles = loggerCfg.maxFiles;
			lgm::RotatingFileSinkManager::instance().createOrReplaceSink(moduleName, rotatingFileSinkConfig);


			lgm::SpdlogModuleLoggerConfig config;
			config.enableConsole = loggerCfg.enableConsole;
			config.consoleSinkLevel = loggerCfg.consoleSinkLevel;
			config.enableFile = loggerCfg.enableFile;
			config.async = loggerCfg.async;
			config.level = loggerCfg.level;
			config.pattern = loggerCfg.pattern;
			config.fileSinkID = moduleName;

			m_logger = lgm::SpdlogModuleLogger::create(moduleName, config);
			lgm::LoggerManager::getInstance().registerLogger(moduleName, m_logger);
		}

		void Logger::trace(const std::string& message)
		{
			if (m_logger) {
				m_logger->trace(message);
			}
		}

		void Logger::debug(const std::string& message)
		{
			if (m_logger) {
				m_logger->debug(message);
			}
		}

		void Logger::info(const std::string& message)
		{
			if (m_logger) {
				m_logger->info(message);
			}
		}

		void Logger::warn(const std::string& message)
		{
			if (m_logger) {
				m_logger->warn(message);
			}
		}

		void Logger::error(const std::string& message)
		{
			if (m_logger) {
				m_logger->error(message);
			}
		}

		void Logger::critical(const std::string& message)
		{
			if (m_logger) {
				m_logger->critical(message);
			}
		}

		void Logger::setLevel(LogLevel level)
		{
			if (m_logger) {
				m_logger->setLevel(level);
			}
		}

		void Logger::flush()
		{
			if (m_logger) {
				m_logger->flush();
			}
		}

		void Logger::logFormatted(LogLevel level, const std::string& message)
		{
			switch (level) {
			case lgm::LogLevel::Trace:
				trace(message);
				break;
			case lgm::LogLevel::Debug:
				debug(message);
				break;
			case lgm::LogLevel::Info:
				info(message);
				break;
			case lgm::LogLevel::Warn:
				warn(message);
				break;
			case lgm::LogLevel::Error:
				error(message);
				break;
			case lgm::LogLevel::Critical:
				critical(message);
				break;
			case LogLevel::Off:
				break;
			default:
				break;
			}
		}

	} // namespace imgPro
} // namespace rw