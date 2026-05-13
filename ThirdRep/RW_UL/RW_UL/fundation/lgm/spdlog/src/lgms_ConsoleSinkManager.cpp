#include "lgms_ConsoleSinkManager.hpp"
#include "lgms_Utilty.hpp" 



namespace rw {
	namespace lgm {

		ConsoleSinkManager& ConsoleSinkManager::instance() {
			static ConsoleSinkManager inst;
			return inst;
		}

		ConsoleSinkManager::ConsoleSinkManager()
			: m_defaultPattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [thread %t] %v"),
			m_defaultLevel(LogLevel::Info) {
		}

		ConsoleSinkManager::~ConsoleSinkManager() = default;

		std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> ConsoleSinkManager::getSharedSink() {
			return getOrCreateSink("default");
		}

		std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> ConsoleSinkManager::getOrCreateSink(const std::string& name) {
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_sinks.find(name);
			if (it != m_sinks.end()) {
				return it->second;
			}

			// create new sink with defaults
			auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			sink->set_pattern(m_defaultPattern);
			sink->set_level(toSpdlogLevel(m_defaultLevel));
			m_sinks[name] = sink;
			return sink;
		}

		std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> ConsoleSinkManager::createOrReplaceSink(
			const std::string& name,
			LogLevel level,
			const std::string& pattern)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			sink->set_pattern(pattern.empty() ? m_defaultPattern : pattern);
			sink->set_level(toSpdlogLevel(level));
			m_sinks[name] = sink;
			return sink;
		}

		void ConsoleSinkManager::setDefaultPattern(const std::string& pattern) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultPattern = pattern;
		}

		void ConsoleSinkManager::setDefaultLevel(LogLevel level) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultLevel = level;
		}

	} // namespace lgm
} // namespace rw