#include "lgms_DailyFileSinkManager.hpp"
#include "lgms_Utilty.hpp"
#include <filesystem>

namespace rw {
	namespace lgm {

		DailyFileSinkManager& DailyFileSinkManager::instance() {
			static DailyFileSinkManager inst;
			return inst;
		}

		DailyFileSinkManager::DailyFileSinkManager() {
			// 使用默认配置初始化
		}

		DailyFileSinkManager::~DailyFileSinkManager() = default;

		std::shared_ptr<spdlog::sinks::daily_file_sink_mt> DailyFileSinkManager::getSharedSink() {
			return getOrCreateSink("default");
		}

		std::shared_ptr<spdlog::sinks::daily_file_sink_mt> DailyFileSinkManager::getOrCreateSink(const std::string& name) {
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_sinks.find(name);
			if (it != m_sinks.end()) {
				return it->second;
			}

			// 创建日志文件路径的父目录
			std::filesystem::path p(m_defaultConfig.baseFilePath);
			if (p.has_parent_path()) {
				std::filesystem::create_directories(p.parent_path());
			}

			// 使用默认配置创建新的 sink
			auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
				m_defaultConfig.baseFilePath,
				m_defaultConfig.rotationHour,
				m_defaultConfig.rotationMinute,
				m_defaultConfig.truncate,
				m_defaultConfig.maxFiles);
			sink->set_pattern(m_defaultConfig.pattern);
			sink->set_level(toSpdlogLevel(m_defaultConfig.level));
			m_sinks[name] = sink;
			return sink;
		}

		std::shared_ptr<spdlog::sinks::daily_file_sink_mt> DailyFileSinkManager::createOrReplaceSink(
			const std::string& name,
			const DailyFileSinkConfig& config)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			// 创建日志文件路径的父目录
			std::filesystem::path p(config.baseFilePath);
			if (p.has_parent_path()) {
				std::filesystem::create_directories(p.parent_path());
			}

			// 使用指定配置创建新的 sink
			auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
				config.baseFilePath,
				config.rotationHour,
				config.rotationMinute,
				config.truncate,
				config.maxFiles);
			sink->set_pattern(config.pattern);
			sink->set_level(toSpdlogLevel(config.level));
			m_sinks[name] = sink;
			return sink;
		}

		void DailyFileSinkManager::setDefaultConfig(const DailyFileSinkConfig& config) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig = config;
		}

		void DailyFileSinkManager::setDefaultPattern(const std::string& pattern) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig.pattern = pattern;
		}

		void DailyFileSinkManager::setDefaultLevel(LogLevel level) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig.level = level;
		}

	} // namespace lgm
} // namespace rw