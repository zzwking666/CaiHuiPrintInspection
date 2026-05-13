#pragma once

#include"imgPro_Logger.hpp"

#define LOG_TRACE(...)    Logger::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...)    Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...)     Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...)     Logger::getInstance().warn(__VA_ARGS__)
#define LOG_ERROR(...)    Logger::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::getInstance().critical(__VA_ARGS__)
#define LOG_FLUSH() Logger::getInstance().flush()