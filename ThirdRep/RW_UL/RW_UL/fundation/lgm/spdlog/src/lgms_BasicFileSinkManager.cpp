#include "lgms_BasicFileSinkManager.hpp"
#include "lgms_Utilty.hpp"
#include <filesystem>

namespace rw {
	namespace lgm {

		BasicFileSinkManager& BasicFileSinkManager::instance() {
			static BasicFileSinkManager inst;
			return inst;
		}

		BasicFileSinkManager::BasicFileSinkManager() {
			// 使用默认配置初始化
		}

		BasicFileSinkManager::~BasicFileSinkManager() = default;

		std::shared_ptr<spdlog::sinks::basic_file_sink_mt> BasicFileSinkManager::getSharedSink() {
			return getOrCreateSink("default");
		}

		std::shared_ptr<spdlog::sinks::basic_file_sink_mt> BasicFileSinkManager::getOrCreateSink(const std::string& name) {
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_sinks.find(name);
			if (it != m_sinks.end()) {
				return it->second;
			}

			// 创建日志文件路径的父目录
			std::filesystem::path p(m_defaultConfig.filePath);
			if (p.has_parent_path()) {
				std::filesystem::create_directories(p.parent_path());
			}

			// 使用默认配置创建新的 sink
			auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
				m_defaultConfig.filePath,
				m_defaultConfig.truncate);
			sink->set_pattern(m_defaultConfig.pattern);
			sink->set_level(toSpdlogLevel(m_defaultConfig.level));
			m_sinks[name] = sink;
			return sink;
		}

		std::shared_ptr<spdlog::sinks::basic_file_sink_mt> BasicFileSinkManager::createOrReplaceSink(
			const std::string& name,
			const BasicFileSinkConfig& config)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			// 创建日志文件路径的父目录
			std::filesystem::path p(config.filePath);
			if (p.has_parent_path()) {
				std::filesystem::create_directories(p.parent_path());
			}

			// 使用指定配置创建新的 sink
			auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
				config.filePath,
				config.truncate);
			sink->set_pattern(config.pattern);
			sink->set_level(toSpdlogLevel(config.level));
			m_sinks[name] = sink;
			return sink;
		}

		void BasicFileSinkManager::setDefaultConfig(const BasicFileSinkConfig& config) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig = config;
		}

		void BasicFileSinkManager::setDefaultPattern(const std::string& pattern) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig.pattern = pattern;
		}

		void BasicFileSinkManager::setDefaultLevel(LogLevel level) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_defaultConfig.level = level;
		}

	} // namespace lgm
} // namespace rw