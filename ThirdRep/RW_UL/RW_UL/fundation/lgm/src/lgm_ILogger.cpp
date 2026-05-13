#include "lgm_ILogger.hpp"

namespace rw {
	namespace lgm {
		LoggerManager& LoggerManager::getInstance()
		{
			static LoggerManager instance;
			return instance;
		}

		void LoggerManager::registerLogger(const std::string& name, std::shared_ptr<ILogger> logger) {
			std::unique_lock lock(m_mutex);
			m_loggers[name] = std::move(logger);  
		}

		std::shared_ptr<ILogger> LoggerManager::getLogger(const std::string& name) const {
			std::shared_lock lock(m_mutex);
			auto it = m_loggers.find(name);
			return (it != m_loggers.end()) ? it->second : nullptr;
		}

		void LoggerManager::removeLogger(const std::string& name) {
			std::unique_lock lock(m_mutex);
			m_loggers.erase(name);
		}

		void LoggerManager::removeAllLoggers() {
			std::unique_lock lock(m_mutex);
			m_loggers.clear();
		}

		bool LoggerManager::hasLogger(const std::string& name) const {
			std::shared_lock lock(m_mutex);
			return m_loggers.find(name) != m_loggers.end();
		}

	} // namespace lgm
} // namespace rw