#include "RuntimeInfoModule.hpp"

bool RuntimeInfoModule::build()
{
	detachUtiltyThread = std::make_unique<DetachUtiltyThread>(this);
	return true;
}

void RuntimeInfoModule::destroy()
{
	detachUtiltyThread.reset();
}

void RuntimeInfoModule::start()
{
	detachUtiltyThread->startThread();
}

void RuntimeInfoModule::stop()
{
	detachUtiltyThread->stopThread();
	detachUtiltyThread->wait();
}
