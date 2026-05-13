#include"utilty_ThreadPool_t.h"
namespace utilty_TreadPool {
	/**
	 * @brief Test the ThreadPool with a big quantity of tasks and the time is random
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a thread pool with the number of threads equal to the number of cores in the CPU
	 * 2. Add 20000 cheap tasks and 20 expensive tasks to the thread pool
	 * 3. Wait for all the tasks to finish
	 * 4. Print the total number of cheap tasks, the total time of cheap tasks, the total number of expensive tasks, the total time of expensive tasks, and the total time
	 *
	 * Expected Result:
	 * 1. The total number of cheap tasks should be 20000
	 * 2. The total number of expensive tasks should be 20
	 * 3. Run successfully
	 *
	 * Boundary Conditions:
	 * 1. The number of threads in the thread pool is equal to the number of cores in the CPU
	 * 2. The number of cheap tasks is 20000
	 * 3. The number of expensive tasks is 20
	 * 4. The time of cheap tasks is between 20ms and 100ms
	 */
	TEST_F(ThreadPoolTest, ThreadPoolTest_LargeQuantity_random) {
		auto start = std::chrono::high_resolution_clock::now();
		rw::ThreadPool pool(std::thread::hardware_concurrency());

		std::vector<std::future<bool>> results;

		for (int i = 0; i < 20000; ++i) {
			results.emplace_back(pool.enqueue([this] { return this->cheapOperate(std::chrono::microseconds(distMs(gen))); }));
		}

		for (int i = 0; i < 20; ++i) {
			results.emplace_back(pool.enqueue([this] { return this->expensiveOperate(std::chrono::seconds(distS(gen))); }));
		}

		for (auto&& result : results) {
			result.get();
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "Total cheap operate num:" << this->cheapOperateNum << std::endl;
		std::cout << "Total cheap time:" << this->totalCheapOperateTime.count() << "ms" << std::endl;
		std::cout << "Total expensive operate num:" << this->expensiveOperateNum << std::endl;
		std::cout << "Total expensive time:" << this->totalExpensiveOperateTime.count() << "s" << std::endl;
		std::cout << "Total time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

		SUCCEED();
	}

	/**
	 * @brief Test the ThreadPool with a big quantity of tasks and the time is fixed
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a thread pool with the number of threads equal to the number of cores in the CPU
	 * 2. Add 20000 cheap tasks and 20 expensive tasks to the thread pool
	 * 3. Wait for all the tasks to finish
	 * 4. Print the total number of cheap tasks, the total time of cheap tasks, the total number of expensive tasks, the total time of expensive tasks, and the total time
	 *
	 * Expected Result:
	 * 1. The total number of cheap tasks should be 10000
	 * 2. The total number of expensive tasks should be 20
	 * 3.. Run successfully
	 *
	 * Boundary Conditions:
	 * 1. The number of threads in the thread pool is equal to the number of cores in the CPU
	 * 2. The number of cheap tasks is 20000
	 * 3. The number of expensive tasks is 20
	 * 4. The time of cheap tasks is between 20ms and 100ms
	 */
	TEST_F(ThreadPoolTest, ThreadPoolTest_LargeQuantity) {
		auto start = std::chrono::high_resolution_clock::now();
		rw::ThreadPool pool(std::thread::hardware_concurrency());

		std::vector<std::future<bool>> results;

		for (int i = 0; i < 10000; ++i) {
			results.emplace_back(pool.enqueue([this] { return this->cheapOperate(std::chrono::microseconds(200)); }));
		}

		for (int i = 0; i < 20; ++i) {
			results.emplace_back(pool.enqueue([this] { return this->expensiveOperate(std::chrono::seconds(2)); }));
		}

		for (auto&& result : results) {
			result.get();
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "Total cheap operate num:" << this->cheapOperateNum << std::endl;
		std::cout << "Total cheap time:" << this->totalCheapOperateTime.count() << "ms" << std::endl;
		std::cout << "Total expensive operate num:" << this->expensiveOperateNum << std::endl;
		std::cout << "Total expensive time:" << this->totalExpensiveOperateTime.count() << "s" << std::endl;
		std::cout << "Total time:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

		SUCCEED();
	}

	/**
	 * @brief Test solve the problem without using the thread pool to compare the performance
	 *
	 * @details
	 * Test Steps:
	 * 1.Create the threads which numbers is equal to the number of cores in the CPU
	 * 2.Add 10000 cheap tasks and 20 expensive tasks to the threads
	 * 3.Wait for all the tasks to finish
	 * 4.Print the total number of cheap tasks, the total time of cheap tasks, the total number of expensive tasks, the total time of expensive tasks, and the total time
	 *
	 * Expected Result:
	 * 1. The total number of cheap tasks should be 10000
	 * 2. The total number of expensive tasks should be 20
	 * 3. Run successfully
	 *
	 * Boundary Conditions:
	 * 1. The number of threads is equal to the number of cores in the CPU
	 * 2. The number of cheap tasks is 10000
	 * 3. The number of expensive tasks is 20
	 */

	TEST_F(ThreadPoolTest, ThreadPoolTest_LargeQuantity_NoThreadPool) {
		auto start = std::chrono::high_resolution_clock::now();

		std::vector<std::thread> threads;
		std::mutex mtx;
		std::condition_variable cv;
		size_t activeThreads = 0;
		const size_t maxThreads = std::thread::hardware_concurrency();

		auto threadTask = [&](auto&& func) {
			{
				std::unique_lock<std::mutex> lock(mtx);
				cv.wait(lock, [&] { return activeThreads < maxThreads; });
				++activeThreads;
			}

			std::thread([&, func] {
				func();
				{
					std::lock_guard<std::mutex> lock(mtx);
					--activeThreads;
				}
				cv.notify_one();
				}).detach();
			};

		for (int i = 0; i < 10000; ++i) {
			threadTask([this] { this->cheapOperate(std::chrono::microseconds(200)); });
		}

		for (int i = 0; i < 20; ++i) {
			threadTask([this] { this->expensiveOperate(std::chrono::seconds(2)); });
		}

		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [&] { return activeThreads == 0; });
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "Total cheap operate num:" << this->cheapOperateNum << std::endl;
		std::cout << "Total cheap time:" << this->totalCheapOperateTime.count() << "ms" << std::endl;
		std::cout << "Total expensive operate num:" << this->expensiveOperateNum << std::endl;
		std::cout << "Total expensive time:" << this->totalExpensiveOperateTime.count() << "s" << std::endl;
		std::cout << "Total time without thread pool:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

		SUCCEED();
	}
}

bool ThreadPoolTest::expensiveOperate(std::chrono::seconds time)
{
	std::this_thread::sleep_for(time);
	{
		std::lock_guard<std::mutex> lockNum(mtxExpensive);
		this->expensiveOperateNum++;
		totalExpensiveOperateTime += time;
		std::lock_guard<std::mutex> lock(mtxCout);
	}
	return true;
}

bool ThreadPoolTest::cheapOperate(std::chrono::microseconds time)
{
	std::this_thread::sleep_for(time);
	{
		std::lock_guard<std::mutex> lockNum(mtxCheap);
		this->totalCheapOperateTime += time;
		this->cheapOperateNum++;
		std::lock_guard<std::mutex> lock(mtxCout);
	}
	return true;
}