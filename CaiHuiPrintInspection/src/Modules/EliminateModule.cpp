#include "EliminateModule.hpp"

bool EliminateModule::build()
{
	detachDefectThreadPreProcess = std::make_unique<DetachDefectThread>(this);
	productPriorityQueue1 = std::make_unique<rw::dsl::ThreadSafeHeap<bool, rw::dsl::HeapType::MinHeap>>(50);
	productPriorityQueue2 = std::make_unique<rw::dsl::ThreadSafeHeap<bool, rw::dsl::HeapType::MinHeap>>(50);

	return true;
}

void EliminateModule::destroy()
{
	if (detachDefectThreadPreProcess)
	{
		detachDefectThreadPreProcess.reset();
	}
	if (productPriorityQueue1)
	{
		productPriorityQueue1.reset();
	}
	if (productPriorityQueue2)
	{
		productPriorityQueue2.reset();
	}
}

void EliminateModule::start()
{
	if (detachDefectThreadPreProcess)
	{
		detachDefectThreadPreProcess->startThread();
	}
}

void EliminateModule::stop()
{
	if (detachDefectThreadPreProcess)
	{
		detachDefectThreadPreProcess->stopThread();
		detachDefectThreadPreProcess->wait();
	}
}
