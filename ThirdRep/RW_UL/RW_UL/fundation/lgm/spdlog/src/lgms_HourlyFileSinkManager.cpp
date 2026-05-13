#include "lgms_HourlyFileSinkManager.hpp"
#include "lgms_Utilty.hpp"
#include <filesystem>

namespace rw {
	namespace lgm {

		HourlyFileSinkManager& HourlyFileSinkManager::instance() {
			static HourlyFileSinkManager inst;
			return inst;
		}

		HourlyFileSinkManager::HourlyFileSinkManager() {
			// 使用默认配置初始化
		}

		HourlyFileSinkManager::~HourlyFileSinkManager() = default;

		std::shared_ptr<spdlog::sinks::hourly_file_sink_mt> HourlyFileSinkManager::getSharedSink() {
			return getOrCreateSink("default");
		}

		std::shared_ptr<spdlog::sinks::hourly_file_sink_mt> HourlyFileSinkManager::getOrCreateSink(const std::string& name) {
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
			auto sink = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(
				m_defaultConfig.baseFilePath,
				m_defaultConfig.truncate,
				m_defaultConfig.maxFiles);
			sink->set_pattern(m_defaultConfig.pattern);
			sink->set_level(toSpdlogLevel(m_defaultConfig.level));
			m_sinks[name] = sink;
			return sink;
		}

		std::shared_ptr<spdlog::sinks::hourly_file_sink_mt> HourlyFileSinkManager::createOrReplaceSink(
			const std::string& name,
			const HourlyFileSinkConfig& config)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			// 创建日志文件路径的父目录
			std::filesystem::path p(config.baseFilePath);
			if (p.has_parent_path()) {
				std::filesystem::create_directories(p.parent_path());
			}

			// 使用指定配置创建新的 sink
			auto sink = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(
				config.baseFilePath,
				config.truncate,
				config.maxFiles);
			sink->set_pattern(config.pattern);
			sink->set_level(toSpdlogLevel(config.level));
			m_sinks[name] = sink;
			return sink;
		}

		void HourlyFileSinkManager::setDefaultConfig(const HourlyFileSinkConfig& config) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig = config;
		}

		void HourlyFileSinkManager::setDefaultPattern(const std::string& pattern) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig.pattern = pattern;
		}

		void HourlyFileSinkManager::setDefaultLevel(LogLevel level) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig.level = level;
		}

	} // namespace lgm
} // namespace rw