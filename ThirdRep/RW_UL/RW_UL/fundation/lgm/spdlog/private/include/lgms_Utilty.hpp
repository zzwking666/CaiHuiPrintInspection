#pragma once

#include "lgm_ILogger.hpp"
#include"spdlog/spdlog.h"

namespace rw
{
	namespace lgm
	{
		inline spdlog::level::level_enum toSpdlogLevel(LogLevel level) {
			switch (level) {
			case LogLevel::Trace: return spdlog::level::trace;
			case LogLevel::Debug: return spdlog::level::debug;
			case LogLevel::Info: return spdlog::level::info;
			case LogLevel::Warn: return spdlog::level::warn;
			case LogLevel::Error: return spdlog::level::err;
			case LogLevel::Critical: return spdlog::level::critical;
			case LogLevel::Off: return spdlog::level::off;
			default: return spdlog::level::info;
			}
		}
	}
}
