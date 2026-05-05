#pragma once

#include"IModule.hpp"
#include <QObject>
#include "DetachDefectThreadProcess.h"

class EliminateModule
	: public QObject, public IModule<bool>
{
	Q_OBJECT
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<rw::dsl::ThreadSafeHeap<bool>> productPriorityQueue1;
	std::unique_ptr<rw::dsl::ThreadSafeHeap<bool>> productPriorityQueue2;
public:
	std::unique_ptr<DetachDefectThread> detachDefectThreadPreProcess{ nullptr };

};