#pragma once

#ifdef BUILD_WITHOUT_HARDWARE
#include"IModule.hpp"

#include <QObject>

#include "TestImgPushThread.hpp"

class TestModule
	:public QObject, public IModule<void>
{
	Q_OBJECT
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::atomic_bool testImgPush{ false };
	std::unique_ptr<TestImgPushThread> testImgPushThread{};
};
#endif

