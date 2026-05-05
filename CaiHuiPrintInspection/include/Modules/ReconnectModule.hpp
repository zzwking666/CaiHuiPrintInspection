#pragma once

#include<QObject>

#include"IModule.hpp"
#include "CameraAndCardStateThread.h"

class ReconnectModule
	:public QObject, public IModule<void>
{
	Q_OBJECT
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<CameraAndCardStateThread> monitorCameraAndCardStateThread{ nullptr };
};
