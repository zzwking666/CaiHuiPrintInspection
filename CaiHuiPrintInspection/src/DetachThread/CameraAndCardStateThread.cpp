#include "CameraAndCardStateThread.h"
#include "Modules.hpp"

size_t CameraAndCardStateThread::runtimeCounts = 0;

CameraAndCardStateThread::CameraAndCardStateThread(QObject* parent)
	: QThread(parent), running(false) {
}

CameraAndCardStateThread::~CameraAndCardStateThread()
{
	stopThread();
	wait();
}

void CameraAndCardStateThread::startThread()
{
	running = true;
	if (!isRunning()) {
		start();
	}
}

void CameraAndCardStateThread::stopThread()
{
	running = false;
}

void CameraAndCardStateThread::run()
{
	while (running) {
		QThread::msleep(2000);

		check_cameraState();

		runtimeCounts++;
		if (runtimeCounts == 4) {
			runtimeCounts = 0;
		}
	}
}

void CameraAndCardStateThread::check_cameraState()
{
	check_cameraState1();
	check_cameraState2();
}

void CameraAndCardStateThread::check_cameraState1()
{
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	static bool isUpdateState = false;

	if (runtimeCounts != 0) {
		return;
	}

	if (camera1) {
		if (camera1->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(1, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyCamera(1);
			emit updateCameraLabelState(1, false);
		}
	}
	else {
		emit buildCamera(1);
		emit startMonitor(1);
		emit updateCameraLabelState(1, false);
		isUpdateState = false;
	}
}

void CameraAndCardStateThread::check_cameraState2()
{
	auto& camera2 = Modules::getInstance().cameraModule.camera2;
	static bool isUpdateState = false;

	if (runtimeCounts != 0) {
		return;
	}

	if (camera2) {
		if (camera2->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(2, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyCamera(2);
			emit updateCameraLabelState(2, false);
		}
	}
	else {
		emit buildCamera(2);
		emit startMonitor(2);
		emit updateCameraLabelState(2, false);
		isUpdateState = false;
	}
}