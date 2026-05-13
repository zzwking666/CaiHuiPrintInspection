#include "lgm_Logger.hpp"

#include"lgms_DailyFileSinkManager.hpp"
#include "lgms_SpdlogModuleLogger.hpp"

namespace rw
{
	namespace lgm
	{
		void iniLogger(const LoggerCfg& cfg)
		{
			loggerCfg = cfg;
			Logger::getInstance();
		}

		Logger& Logger::getInstance()
		{
			static Logger instance;
			return instance;
		}

		Logger::Logger()
		{
			lgm::DailyFileSinkConfig dailyFileSinkConfig;
			dailyFileSinkConfig.level = loggerCfg.fileSinkLevel;
			dailyFileSinkConfig.baseFilePath = loggerCfg.baseFilePath;
			dailyFileSinkConfig.rotationHour = loggerCfg.rotationHour;
			dailyFileSinkConfig.rotationMinute = loggerCfg.rotationMinute;
			dailyFileSinkConfig.truncate = loggerCfg.truncate;
			dailyFileSinkConfig.maxFiles = loggerCfg.maxFiles;
			lgm::DailyFileSinkManager::instance().createOrReplaceSink(moduleName, dailyFileSinkConfig);


			lgm::SpdlogModuleLoggerConfig config;
			config.enableConsole = loggerCfg.enableConsole;
			config.consoleSinkLevel = loggerCfg.consoleSinkLevel;
			config.enableFile = loggerCfg.enableFile;
			config.async = loggerCfg.async;
			config.level = loggerCfg.level;
			config.pattern = loggerCfg.pattern;
			config.fileSinkID = moduleName;
			config.fileSinkType = lgm::FileSinkType::Daily;

			//TODO:这里直接注册moduleName可能有多种类型的logger冲突风险，他们的名字可能是一样的，所以需要区分不同类型的logger
			m_logger = lgm::SpdlogModuleLogger::create(moduleName, config);
			lgm::LoggerManager::getInstance().registerLogger(moduleName, m_logger);
		}

		Logger::~Logger()
		{
			if (m_logger) {
				m_logger->flush();
			}
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