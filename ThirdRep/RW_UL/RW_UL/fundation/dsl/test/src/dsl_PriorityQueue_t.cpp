#include"dsl_pch_t.h"

#include"dsl_PriorityQueue_t.h"

namespace dsl_PriorityQueue
{
	auto compareNodeEqual = [](const int& a, const int& b) -> bool {
		return a == b;
		};

	auto compareNodePriority = [](const size_t& a, const size_t& b) -> bool {
		return a < b; // 优先级较大的元素优先
		};

	TEST(PriorityQueueFactoryTest, CreateDHeap) {
		auto queue = rw::dsl::PriorityQueueFactory<int, size_t>::createPriorityQueue(
			rw::dsl::PriorityQueueType::DHeap,
			compareNodeEqual,
			compareNodePriority,
			4);

		ASSERT_NE(queue, nullptr); // 验证队列是否成功创建
		EXPECT_EQ(queue->size(), 0); // 初始队列大小应为 0

		queue->insert(10, 5);
		queue->insert(20, 3);
		queue->insert(30, 8);

		EXPECT_EQ(queue->top(), 20); // 验证堆顶元素
		EXPECT_EQ(queue->top(), 10);
		EXPECT_EQ(queue->top(), 30);
	}

	TEST(PriorityQueueFactoryTest, CreateDHeapLockFree) {
		auto queue = rw::dsl::PriorityQueueFactory<int, size_t>::createPriorityQueue(
			rw::dsl::PriorityQueueType::DHeapLockFree,
			compareNodeEqual,
			compareNodePriority,
			4);

		ASSERT_NE(queue, nullptr); // 验证队列是否成功创建
		EXPECT_EQ(queue->size(), 0); // 初始队列大小应为 0

		queue->insert(10, 5);
		queue->insert(20, 3);
		queue->insert(30, 8);

		EXPECT_EQ(queue->top(), 20); // 验证堆顶元素
		EXPECT_EQ(queue->top(), 10);
		EXPECT_EQ(queue->top(), 30);
	}
}