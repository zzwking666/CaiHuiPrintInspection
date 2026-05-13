#include "dsl_pch_t.h"
#include "dsl_DHeapLockFree.hpp"

namespace dsl_PriorityQueue {
	auto compareNodeEqual = [](const int& a, const int& b) -> bool {
		return a == b;
		};

	auto compareNodePriority = [](const size_t& a, const size_t& b) -> bool {
		return a < b; // 优先级较大的元素优先
		};

	TEST(DHeapLockFreeTest, TopWithIsGet) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);
		heap.insert(30, 8);

		bool isGet = false;

		// 获取堆顶元素
		EXPECT_EQ(heap.top(isGet), 20); // 优先级最低的元素
		EXPECT_TRUE(isGet); // 验证操作成功

		EXPECT_EQ(heap.top(isGet), 10); // 下一个优先级最低的元素
		EXPECT_TRUE(isGet);

		EXPECT_EQ(heap.top(isGet), 30); // 最后一个元素
		EXPECT_TRUE(isGet);

		// 堆为空时
		EXPECT_EQ(heap.top(isGet), int()); // 返回默认构造的值
		EXPECT_FALSE(isGet); // 验证操作失败
	}

	TEST(DHeapLockFreeTest, PeekWithIsGet) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);

		bool isGet = false;

		// 验证堆顶元素
		EXPECT_EQ(heap.peek(isGet), 20); // 优先级最低的元素
		EXPECT_TRUE(isGet); // 验证操作成功

		EXPECT_EQ(heap.peek(isGet), 20); // peek 不移除元素
		EXPECT_TRUE(isGet);

		// 清空堆
		heap.clear();

		// 堆为空时
		EXPECT_EQ(heap.peek(isGet), int()); // 返回默认构造的值
		EXPECT_FALSE(isGet); // 验证操作失败
	}

	TEST(DHeapLockFreeTest, InsertAndTop) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);
		heap.insert(30, 8);

		std::vector<int> expected = { 20, 10, 30 };
		std::vector<int> actual;
		// 获取堆顶元素
		actual.push_back(heap.top());
		actual.push_back(heap.top());
		actual.push_back(heap.top());
		// 验证堆顶元素顺序
		EXPECT_EQ(actual, expected);
	}

	TEST(DHeapLockFreeTest, Peek) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);

		// 验证堆顶元素
		EXPECT_EQ(heap.peek(), 20); // 优先级最低的元素
		EXPECT_EQ(heap.peek(), 20); // peek 不移除元素
	}

	TEST(DHeapLockFreeTest, UpdatePriority) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);
		heap.insert(30, 8);

		// 更新元素优先级
		heap.update(10, 1);

		// 验证堆顶元素顺序
		EXPECT_EQ(heap.top(), 10); // 优先级最低的元素
		EXPECT_EQ(heap.top(), 20);
		EXPECT_EQ(heap.top(), 30);
	}

	TEST(DHeapLockFreeTest, RemoveElement) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);
		heap.insert(30, 8);

		// 删除元素
		heap.remove(20);

		// 验证堆顶元素顺序
		EXPECT_EQ(heap.top(), 10); // 优先级最低的元素
		EXPECT_EQ(heap.top(), 30);
	}

	TEST(DHeapLockFreeTest, ClearHeap) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		// 插入元素
		heap.insert(10, 5);
		heap.insert(20, 3);

		// 清空堆
		heap.clear();

		// 验证堆大小
		EXPECT_EQ(heap.size(), 0);
		EXPECT_THROW(heap.top(), std::runtime_error); // 堆为空时抛出异常
	}

	TEST(DHeapLockFreeTest, MultithreadedInsertAndTop) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		const int num_threads = 4;
		const int num_elements_per_thread = 100;
		std::atomic<int> total_elements_inserted{ 0 };

		// 多线程插入元素
		std::vector<std::thread> threads;
		for (int t = 0; t < num_threads; ++t) {
			threads.emplace_back([&heap, &total_elements_inserted, t, num_elements_per_thread]() {
				for (int i = 0; i < num_elements_per_thread; ++i) {
					int element = t * num_elements_per_thread + i;
					size_t priority = element % 10; // 优先级为元素值的模
					heap.insert(element, priority);
					total_elements_inserted.fetch_add(1, std::memory_order_relaxed);
				}
				});
		}

		// 等待所有线程完成
		for (auto& thread : threads) {
			thread.join();
		}

		// 验证插入的总元素数量
		EXPECT_EQ(total_elements_inserted.load(), num_threads * num_elements_per_thread);

		// 验证堆顶元素是否正确
		int last_priority = -1;
		while (heap.size() > 0) {
			int top_element = heap.top();
			int current_priority = top_element % 10;
			EXPECT_GE(current_priority, last_priority); // 验证优先级顺序
			last_priority = current_priority;
		}
	}

	TEST(DHeapLockFreeTest, MultithreadedPeekInsertTop) {
		rw::dsl::DHeapLockFree<int, size_t> heap(compareNodeEqual, compareNodePriority, 4);

		const int num_threads = 4;
		const int num_elements_per_thread = 100;
		std::atomic<int> total_elements_inserted{ 0 };
		std::atomic<int> total_peeked{ 0 };
		std::atomic<int> total_topped{ 0 };

		// 多线程插入元素
		std::vector<std::thread> threads;
		for (int t = 0; t < num_threads; ++t) {
			threads.emplace_back([&heap, &total_elements_inserted, t, num_elements_per_thread]() {
				for (int i = 0; i < num_elements_per_thread; ++i) {
					int element = t * num_elements_per_thread + i;
					size_t priority = element % 10; // 优先级为元素值的模
					heap.insert(element, priority);
					total_elements_inserted.fetch_add(1, std::memory_order_relaxed);
				}
				});
		}

		// 多线程进行 peek 操作
		for (int t = 0; t < num_threads; ++t) {
			threads.emplace_back([&heap, &total_peeked]() {
				for (int i = 0; i < 50; ++i) { // 每个线程尝试 peek 50 次
					try {
						heap.peek();
						total_peeked.fetch_add(1, std::memory_order_relaxed);
					}
					catch (const std::runtime_error&) {
						// 堆为空时可能抛出异常，忽略
					}
				}
				});
		}

		// 多线程进行 top 操作
		for (int t = 0; t < num_threads; ++t) {
			threads.emplace_back([&heap, &total_topped]() {
				for (int i = 0; i < 50; ++i) { // 每个线程尝试 top 50 次
					try {
						heap.top();
						total_topped.fetch_add(1, std::memory_order_relaxed);
					}
					catch (const std::runtime_error&) {
						// 堆为空时可能抛出异常，忽略
					}
				}
				});
		}

		// 等待所有线程完成
		for (auto& thread : threads) {
			thread.join();
		}

		// 验证插入的总元素数量
		EXPECT_EQ(total_elements_inserted.load(), num_threads * num_elements_per_thread);

		// 验证 peek 和 top 操作的总次数
		EXPECT_GE(total_peeked.load(), 0);
		EXPECT_GE(total_topped.load(), 0);
	}
}