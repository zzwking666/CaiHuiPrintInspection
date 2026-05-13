#include"utilty_ObjectPool_t.h"

namespace utilty_ObjectPool {
	TEST(ObjectPoolTest_threadSafeFalse, allocateTime) {
		const size_t NumberOfIteration{ 50000 };
		std::cout << "Starting loop using pool.." << std::endl;
		MyPool pool;
		auto start1{ std::chrono::steady_clock::now() };
		for (size_t i{ 0 }; i < NumberOfIteration; ++i) {
			auto object{ getExpensiveObject(pool) };
			processExpensiveObject(*object.get());
		}
		auto end1{ std::chrono::steady_clock::now() };
		auto diff1{ end1 - start1 };
		std::cout << "done in " << std::chrono::duration<double, std::milli>(diff1).count() << " ms\n";
		std::cout << "Starting loop using new/delete.." << std::endl;
		auto start2{ std::chrono::steady_clock::now() };
		for (size_t i{ 0 }; i < NumberOfIteration; ++i) {
			auto object{ std::make_shared<ExpensiveObject>() };
			processExpensiveObject(*object.get());
		}
		auto end2{ std::chrono::steady_clock::now() };
		auto diff2{ end2 - start2 };
		std::cout << "done in " << std::chrono::duration<double, std::milli>(diff2).count() << " ms\n";
	}

	TEST(ObjectPoolTest_threadSafeFalse, AcquireObject) {
		rw::ObjectPool<TestObject> pool;
		auto obj = pool.acquireObject(42);
		ASSERT_NE(obj, nullptr);
		EXPECT_EQ(obj->getValue(), 42);
	}

	TEST(ObjectPoolTest_threadSafeFalse, AcquireMultipleObjects) {
		rw::ObjectPool<TestObject> pool;
		auto obj1 = pool.acquireObject(1);
		auto obj2 = pool.acquireObject(2);
		ASSERT_NE(obj1, nullptr);
		ASSERT_NE(obj2, nullptr);
		EXPECT_EQ(obj1->getValue(), 1);
		EXPECT_EQ(obj2->getValue(), 2);
		std::cout << pool.getTotalAllocatedMemory();
	}

	TEST(ObjectPoolTest_threadSafeFalse, ReleaseObject) {
		rw::ObjectPool<TestObject> pool;
		auto obj = pool.acquireObject(42);
		ASSERT_NE(obj, nullptr);
		EXPECT_EQ(obj.use_count(), 1);
		obj.reset();
		EXPECT_EQ(obj, nullptr);
		auto obj2 = pool.acquireObject(43);
		ASSERT_NE(obj2, nullptr);
		EXPECT_EQ(obj2->getValue(), 43);
	}

	TEST(ObjectPoolTest_threadSafeFalse, Destructor) {
		rw::ObjectPool<TestObject>* pool = new rw::ObjectPool<TestObject>();
		auto obj = pool->acquireObject(42);
		ASSERT_NE(obj, nullptr);
		obj.reset();
		delete pool;
		// No explicit check here, just ensuring no crash on delete
	}

	TEST(ObjectPoolTest_threadSafeTrue, AcquireObject) {
		rw::ObjectPool<TestObject, std::allocator<TestObject>, rw::gThreadSafe> pool;
		auto obj = pool.acquireObject(42);
		ASSERT_NE(obj, nullptr);
		EXPECT_EQ(obj->getValue(), 42);
	}

	TEST(ObjectPoolTest_threadSafeTrue, AcquireMultipleObjects) {
		rw::ObjectPool<TestObject, std::allocator<TestObject>, rw::gThreadSafe> pool;
		auto obj1 = pool.acquireObject(1);
		auto obj2 = pool.acquireObject(2);
		ASSERT_NE(obj1, nullptr);
		ASSERT_NE(obj2, nullptr);
		EXPECT_EQ(obj1->getValue(), 1);
		EXPECT_EQ(obj2->getValue(), 2);
		std::cout << pool.getTotalAllocatedMemory();
	}

	TEST(ObjectPoolTest_threadSafeTrue, ReleaseObject) {
		rw::ObjectPool<TestObject, std::allocator<TestObject>, rw::gThreadSafe> pool;
		auto obj = pool.acquireObject(42);
		ASSERT_NE(obj, nullptr);
		EXPECT_EQ(obj.use_count(), 1);
		obj.reset();
		EXPECT_EQ(obj, nullptr);
		auto obj2 = pool.acquireObject(43);
		ASSERT_NE(obj2, nullptr);
		EXPECT_EQ(obj2->getValue(), 43);
	}

	TEST(ObjectPoolTest_threadSafeTrue, Destructor) {
		rw::ObjectPool<TestObject, std::allocator<TestObject>, rw::gThreadSafe>* pool = new rw::ObjectPool<TestObject, std::allocator<TestObject>, true>();
		auto obj = pool->acquireObject(42);
		ASSERT_NE(obj, nullptr);
		obj.reset();
		delete pool;
		// No explicit check here, just ensuring no crash on delete
	}

	TEST(ObjectPoolTest_threadSafeTrue, ConcurrentAccess) {
		rw::ObjectPool<TestObject, std::allocator<TestObject>, rw::gThreadSafe> pool;
		const int numThreads = 10;
		const int numIterations = 1000;
		std::vector<std::thread> threads;

		for (int i = 0; i < numThreads; ++i) {
			threads.emplace_back([&pool, numIterations, i]() {
				for (int j = 0; j < numIterations; ++j) {
					auto obj = pool.acquireObject(i * numIterations + j);
					ASSERT_NE(obj, nullptr);
					EXPECT_EQ(obj->getValue(), i * numIterations + j);
				}
				});
		}

		for (auto& thread : threads) {
			thread.join();
		}
	}
}