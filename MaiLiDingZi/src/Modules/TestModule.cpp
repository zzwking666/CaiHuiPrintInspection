#include "TestModule.hpp"
#ifdef BUILD_WITHOUT_HARDWARE
void TestModule::build()
{
	testImgPushThread = std::make_unique<TestImgPushThread>();
}

void TestModule::destroy()
{
	testImgPushThread.reset();
}

void TestModule::start()
{
	testImgPushThread->startThread();
}

void TestModule::stop()
{
	testImgPushThread->stopThread();
}
#endif
