#ifndef DSL_PRIORITYQUEUE_H
#define DSL_PRIORITYQUEUE_H

#include"dsl_IPriorityQueue.hpp"

#include"dsl_DHeap.hpp"
#include"dsl_DHeapLockFree.hpp"

namespace rw
{
	namespace dsl
	{
		template <class T, class Priority = size_t>
		class PriorityQueueFactory {
		public:
			static Shared_ptr<IPriorityQueue<T, Priority>> createPriorityQueue(
				PriorityQueueType type,
				typename IPriorityQueue<T, Priority>::CompareNodeEqual compareNodeEqual = [](const T& a, const T& b) { return a == b; },
				typename IPriorityQueue<T, Priority>::CompareNodePriority compareNodePriority = [](const Priority& a, const Priority& b) { return a < b; },
				size_t d = 4) {
				switch (type) {
				case PriorityQueueType::DHeap:
					return std::make_shared<DHeap<T, Priority>>(compareNodeEqual, compareNodePriority, d);
				case PriorityQueueType::DHeapLockFree:
					return std::make_shared<DHeapLockFree<T, Priority>>(compareNodeEqual, compareNodePriority, d);
				default:
					throw std::invalid_argument("Invalid PriorityQueueType");
				}
			}
		};
	}
}
#endif //DSL_PRIORITY_QUEUE_H