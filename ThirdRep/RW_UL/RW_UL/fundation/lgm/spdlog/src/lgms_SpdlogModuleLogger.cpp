#include"lgms_SpdlogModuleLogger.hpp"

#include"lgms_Utilty.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <filesystem>

#include"lgms_RotatingFileSinkManager.hpp"
#include"lgms_BasicFileSinkManager.hpp"
#include"lgms_DailyFileSinkManager.hpp"
#include"lgms_HourlyFileSinkManager.hpp"

#include "lgms_ConsoleSinkManager.hpp"
#include "lgms_ThreadPoolManager.hpp"

#include"spdlog/sinks/dup_filter_sink.h"


namespace rw
{
	namespace lgm
	{
		class SpdlogModuleLogger::SpdlogModuleImpl {
		public:
			explicit SpdlogModuleImpl(std::shared_ptr<spdlog::logger> logger)
				: m_logger(std::move(logger)) {
			}

			std::shared_ptr<spdlog::logger> getLogger() {
				return m_logger;
			}

		private:
			std::shared_ptr<spdlog::logger> m_logger;
		};

		std::shared_ptr<SpdlogModuleLogger> SpdlogModuleLogger::create(const std::string& moduleName, const SpdlogModuleLoggerConfig& cfg)
		{
			std::vector<spdlog::sink_ptr> sinks;

			auto dup_filter =
				std::make_shared<spdlog::sinks::dup_filter_sink_mt>(
					std::chrono::seconds(5));

			if (cfg.enableConsole) {
				auto console_sink = ConsoleSinkManager::instance().getSharedSink();
				console_sink->set_level(toSpdlogLevel(cfg.consoleSinkLevel));
				dup_filter->add_sink(console_sink);
			}

			if (cfg.enableFile) {
				spdlog::sink_ptr file_sink;

				switch (cfg.fileSinkType) {
				case FileSinkType::Basic:
					if (cfg.fileSinkID.empty()) {
						file_sink = BasicFileSinkManager::instance().getSharedSink();
					}
					else {
						file_sink = BasicFileSinkManager::instance().getOrCreateSink(cfg.fileSinkID);
					}
					break;

				case FileSinkType::Rotating:
					if (cfg.fileSinkID.empty()) {
						file_sink = RotatingFileSinkManager::instance().getSharedSink();
					}
					else {
						file_sink = RotatingFileSinkManager::instance().getOrCreateSink(cfg.fileSinkID);
					}
					break;

				case FileSinkType::Daily:
					if (cfg.fileSinkID.empty()) {
						file_sink = DailyFileSinkManager::instance().getSharedSink();
					}
					else {
						file_sink = DailyFileSinkManager::instance().getOrCreateSink(cfg.fileSinkID);
					}
					break;

				case FileSinkType::Hourly:
					if (cfg.fileSinkID.empty()) {
						file_sink = HourlyFileSinkManager::instance().getSharedSink();
					}
					else {
						file_sink = HourlyFileSinkManager::instance().getOrCreateSink(cfg.fileSinkID);
					}
					break;

				default:
					file_sink = RotatingFileSinkManager::instance().getSharedSink();
					break;
				}

				dup_filter->add_sink(file_sink);
			}

			std::shared_ptr<spdlog::logger> spd_logger;
			if (cfg.async) {
				spd_logger = std::make_shared<spdlog::async_logger>(moduleName, dup_filter, ThreadPoolManager::instance().getOrCreate());
			}
			else {

				spd_logger = std::make_shared<spdlog::logger>(moduleName, dup_filter);
			}

			spd_logger->set_level(toSpdlogLevel(cfg.level));

			spd_logger->set_pattern(cfg.pattern);

			spdlog::register_logger(spd_logger);

			return std::shared_ptr<SpdlogModuleLogger>(new SpdlogModuleLogger(std::make_unique<SpdlogModuleImpl>(spd_logger)));
		}

		SpdlogModuleLogger::~SpdlogModuleLogger() = default;

		void SpdlogModuleLogger::trace(const std::string& message)
		{
			m_pImpl->getLogger()->trace(message);
		}

		void SpdlogModuleLogger::debug(const std::string& message)
		{
			m_pImpl->getLogger()->debug(message);
		}

		void SpdlogModuleLogger::info(const std::string& message)
		{
			m_pImpl->getLogger()->info(message);
		}

		void SpdlogModuleLogger::warn(const std::string& message)
		{
			m_pImpl->getLogger()->warn(message);
		}

		void SpdlogModuleLogger::error(const std::string& message)
		{
			m_pImpl->getLogger()->error(message);
		}

		void SpdlogModuleLogger::critical(const std::string& message)
		{
			m_pImpl->getLogger()->critical(message);
		}

		void SpdlogModuleLogger::setLevel(LogLevel level)
		{
			m_pImpl->getLogger()->set_level(
				toSpdlogLevel(level)
			);
		}

		void SpdlogModuleLogger::flush()
		{
			m_pImpl->getLogger()->flush();
		}

		void SpdlogModuleLogger::logFormatted(LogLevel level, const std::string& message)
		{
			auto logger = m_pImpl->getLogger();
			switch (level) {
			case LogLevel::Trace: logger->trace(message); break;
			case LogLevel::Debug: logger->debug(message); break;
			case LogLevel::Info: logger->info(message); break;
			case LogLevel::Warn: logger->warn(message); break;
			case LogLevel::Error: logger->error(message); break;
			case LogLevel::Critical: logger->critical(message); break;
			default: break;
			}
		}

		SpdlogModuleLogger::SpdlogModuleLogger(std::unique_ptr<SpdlogModuleImpl> impl)
			: m_pImpl(std::move(impl)) {
		}
	}

}