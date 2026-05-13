#include "lgm_ILogger.hpp"
#include "lgms_SpdlogModuleLogger.hpp"

#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <iostream>

int main()
{
	// 初始化管理器（如有全局初始化需求）
	rw::lgm::LoggerManager::getInstance();

	// 配置：使用异步 logger 写入文件（避免同步控制台成为瓶颈）
	rw::lgm::SpdlogModuleLoggerConfig cfg;
	cfg.async = true;
	cfg.enableConsole = true;
	cfg.enableFile = true;
	//cfg.filePath = "logs/highfreq.log";
	cfg.level = rw::lgm::LogLevel::Info;

	// 创建并注册日志器
	auto log = rw::lgm::SpdlogModuleLogger::create("123", cfg);
	rw::lgm::LoggerManager::getInstance().registerLogger("log", log);


	for (int i=0;i<1113;i++)
	{
		log->info("1111");
	}

	log->flush();
	return 0;
}