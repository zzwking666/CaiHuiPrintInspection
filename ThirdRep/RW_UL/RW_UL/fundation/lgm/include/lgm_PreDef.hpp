#pragma once

#include"lgm_Logger.hpp"

#define LOG_TRACE(...)    rw::lgm::Logger::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...)    rw::lgm::Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...)     rw::lgm::Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...)     rw::lgm::Logger::getInstance().warn(__VA_ARGS__)
#define LOG_ERROR(...)    rw::lgm::Logger::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) rw::lgm::Logger::getInstance().critical(__VA_ARGS__)
#define LOG_FLUSH()		  rw::lgm::Logger::getInstance().flush()