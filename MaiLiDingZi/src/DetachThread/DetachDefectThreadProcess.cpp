#include "DetachDefectThreadProcess.h"
#include <chrono>

#include "Modules.hpp"

DetachDefectThread::DetachDefectThread(QObject* parent)
{

}

DetachDefectThread::~DetachDefectThread()
{
	stopThread();
	wait(); // 等待线程安全退出
}

void DetachDefectThread::startThread()
{
	running = true;
	if (!isRunning()) {
		start(); // 启动线程
	}
}

void DetachDefectThread::stopThread()
{
	running = false; // 停止线程
}

void DetachDefectThread::processQueue1(std::unique_ptr<rw::dsl::ThreadSafeHeap<bool>>& queue)
{
	try
	{
		auto isBad = false;
		queue->tryGetTop(isBad);

		if (isBad)
		{
			auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
			auto& camera = Modules::getInstance().cameraModule.camera1;
			queue->tryPopTop(isBad);
			QThread::msleep(static_cast<unsigned long>(setConfig.shuchuxinhaoyanshi));
			// 剔废动作
			rw::rqw::OutTriggerConfig outTriggerConfig;
			outTriggerConfig.lineSelector = 2;
			outTriggerConfig.lineMode = 8;
			outTriggerConfig.lineSource = 5;
			outTriggerConfig.durationValue = static_cast<long>(setConfig.shuchuxinhaochixushijian * 1000);
			outTriggerConfig.strobeEnable = true;
			if (camera)
			{
				camera->setOutTriggerConfig(outTriggerConfig);
				camera->outTrigger();
			}
			qDebug() << "报警";
		}
	}
	catch (const std::runtime_error&)
	{
		
	}
}

void DetachDefectThread::processQueue2(std::unique_ptr<rw::dsl::ThreadSafeHeap<bool>>& queue)
{
	try
	{
		auto isBad = false;
		queue->tryGetTop(isBad);

		if (isBad)
		{
			auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
			auto& camera = Modules::getInstance().cameraModule.camera2;
			queue->tryPopTop(isBad);
			QThread::msleep(static_cast<unsigned long>(setConfig.shuchuxinhaoyanshi));
			// 剔废动作
			rw::rqw::OutTriggerConfig outTriggerConfig;
			outTriggerConfig.lineSelector = 2;
			outTriggerConfig.lineMode = 8;
			outTriggerConfig.lineSource = 5;
			outTriggerConfig.durationValue = static_cast<long>(setConfig.shuchuxinhaochixushijian * 1000);
			outTriggerConfig.strobeEnable = true;
			if (camera)
			{
				camera->setOutTriggerConfig(outTriggerConfig);
				camera->outTrigger();
			}
		}
	}
	catch (const std::runtime_error&)
	{

	}
}

void DetachDefectThread::run()
{
	auto& priorityQueue1 = Modules::getInstance().eliminateModule.productPriorityQueue1;
	auto& priorityQueue2 = Modules::getInstance().eliminateModule.productPriorityQueue2;

	while (running) {
		QThread::msleep(1);

		processQueue1(priorityQueue1);
		processQueue2(priorityQueue2);
	}
}
