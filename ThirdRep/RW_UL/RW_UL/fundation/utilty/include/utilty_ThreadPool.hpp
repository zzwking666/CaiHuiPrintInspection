#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>
#include <type_traits>
#include <memory>
#include <exception>
#include <atomic>
#include <chrono>

namespace rw {
	/**
	 * @brief ThreadPool is a simple thread pool implementation
	 *
	 * This class provides a simple thread pool implementation that allows you to add tasks
	 * to the thread pool and have them executed by the worker threads.
	 *
	 * @warning
	 *  1.NotThreadSafe : The ThreadPool class is not thread-safe. You should not access the
	 * ThreadPool object from multiple threads at the same time.
	 *  2.NotHandleException : The ThreadPool class does not handle exceptions thrown by the
	 * tasks. If a task throws an exception, the exception will be propagated to the caller
	 * of the get() method.
	 *  Suggestions: You can use a try-catch block inside the task to catch the exception and
	 * handle it appropriately.
	 *
	 * @example
	 *      //create a thread pool with the number of threads equal to the number of cores in the CPU
	 *      ThreadPool pool(std::thread::hardware_concurrency());
	 *      auto result = pool.enqueue([] { return 42; });//add a task to the thread pool
	 *      std::cout << result.get() << std::endl;//get the result of the task
	 */
	class ThreadPool {
	public:

		/**
		 *@Parameters:
		 *  - numThreads: The number of threads in the thread pool,it usually equals to the
		 *number of cores in the CPU
		 *@Methods:
		 *  constructor: Create a thread pool with the specified number of threads
		 *@Returns: void
		 *
		 *@Throws:
		 *
		 */
		explicit ThreadPool(size_t numThreads);

		/**
		 * @brief Destructor
		 *
		 * The destructor will stop the worker threads and join them
		 */
		~ThreadPool();

		/**
		 * @brief Terminate all pending tasks immediately
		 *
		 * This method will:
		 *  - stop accepting new tasks,
		 *  - cancel all pending (queued) tasks and set an exception on their futures,
		 *  - notify all worker threads and join them with timeout.
		 *  - if timeout, detach the threads to avoid deadlock
		 *
		 * @param timeout_ms Maximum time to wait for threads to finish (in milliseconds)
		 *                   Default is 2000ms (2 seconds)
		 *
		 * @note Running tasks cannot be forcefully interrupted in standard C++; this method
		 *       will cancel only the tasks that are still queued. Callers waiting on futures
		 *       of cancelled tasks will receive an exception.
		 */
		void terminate_now(int timeout_ms = 2000);

		/**
		 *@Parameters:
		 *  - f: The function to be executed
		 *  - args: The arguments to be passed to the function
		 *@Methods:
		 *  enqueue: Add a task to the task queue
		 *@Returns: std::future<typename std::invoke_result<F, Args...>::type>
		 *  return a future object that can be used to get the result of the function
		 *@Throws:
		 *
		 */
		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;

	private:

		/**
		 * @brief Type-erased task wrapper that supports running and cancellation
		 */
		struct TaskWrapper {
			std::function<void()> run;
			std::function<void()> cancel;
		};

		/**
		 * @brief The worker threads
		 *
		 * The worker threads will pull tasks from the task queue and execute
		 */
		std::vector<std::thread> workers;

		/**
		 * @brief The task queue
		 *
		 * The task queue is a queue of tasks that need to be executed by the worker threads
		 */
		std::queue<TaskWrapper> tasks;

		/**
		 * @brief The mutex to protect the task queue
		 *
		 * The mutex is used to protect the task queue from being accessed by multiple threads
		 * at the same time
		 */
		std::mutex queueMutex;

		/**
		 * @brief The condition variable to notify the worker threads
		 *
		 * The condition variable is used to notify the worker threads that there is a new
		 * task in the task queue
		 */
		std::condition_variable condition;

		/**
		 * @brief A flag to indicate that the thread pool should stop
		 *
		 * The stop flag is used to indicate that the thread pool should stop and that the
		 * worker threads should exit
		 */
		std::atomic<bool> stop;

		/**
		 * @brief A flag to indicate immediate termination
		 *
		 * When this flag is set, worker threads should exit immediately
		 * even if there are tasks in the queue
		 */
		std::atomic<bool> force_stop;
	};

	/*------------------------------------------------------------------------------------
	 *                          Inline implementation of ThreadPool
	 *-----------------------------------------------------------------------------------*/

	 // 构造函数：创建并启动工作线程
	inline ThreadPool::ThreadPool(size_t numThreads)
		: stop(false), force_stop(false)
	{
		for (size_t i = 0; i < numThreads; ++i) {
			workers.emplace_back([this] {
				for (;;) {
					TaskWrapper task;
					{
						std::unique_lock<std::mutex> lock(this->queueMutex);
						this->condition.wait(lock, [this] {
							return this->force_stop.load() || this->stop.load() || !this->tasks.empty();
							});

						// 如果收到强制停止信号,立即退出
						if (this->force_stop.load()) {
							return;
						}

						// 如果收到普通停止信号且队列为空,退出
						if (this->stop.load() && this->tasks.empty()) {
							return;
						}

						task = std::move(this->tasks.front());
						this->tasks.pop();
					}
					// 执行任务前再次检查是否需要强制停止
					if (this->force_stop.load()) {
						return;
					}
					// 执行任务
					task.run();
				}
				});
		}
	}

	// 析构函数：确保所有线程被正确停止并 join（等待正在执行的任务完成）
	inline ThreadPool::~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop.store(true);
		}
		condition.notify_all();
		for (std::thread& worker : workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
		workers.clear();
	}

	// 立即终止所有待处理任务（取消队列中任务并为其 future 设置异常），并等待线程退出
	inline void ThreadPool::terminate_now(int timeout_ms)
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			// 设置强制停止标志
			force_stop.store(true);
			stop.store(true);

			// 取消所有队列中的任务：调用每个 task 的 cancel()，以便其 future 能够被通知
			while (!tasks.empty()) {
				TaskWrapper& t = tasks.front();
				try {
					if (t.cancel) {
						t.cancel();
					}
				}
				catch (...) {
					// cancel 回调不应抛异常；这里吞掉以保证继续取消其余任务
				}
				tasks.pop();
			}
		}

		// 唤醒所有工作线程
		condition.notify_all();

		// 尝试在超时时间内等待线程结束
		auto start_time = std::chrono::steady_clock::now();
		for (auto& worker : workers) {
			if (worker.joinable()) {
				auto elapsed = std::chrono::steady_clock::now() - start_time;
				auto remaining = std::chrono::milliseconds(timeout_ms) - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

				if (remaining.count() <= 0) {
					// 超时,直接 detach 剩余线程以避免死锁
					worker.detach();
				}
				else {
					// 使用简单的轮询方式实现带超时的 join
					auto wait_until = std::chrono::steady_clock::now() + remaining;
					while (worker.joinable() && std::chrono::steady_clock::now() < wait_until) {
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						// 如果线程还在运行,继续等待
					}

					if (worker.joinable()) {
						// 超时后仍未结束,detach 该线程
						worker.detach();
					}
				}
			}
		}

		workers.clear();
	}

	// 添加任务到线程池（使用 promise + run/cancel 包装以支持取消未执行的任务）
	template<class F, class... Args>
	auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
		using returnType = typename std::invoke_result<F, Args...>::type;

		// 将调用目标绑定为无参可调用对象
		auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		// 使用 promise 来对结果进行控制（这样可以在取消时设置异常）
		auto promise = std::make_shared<std::promise<returnType>>();
		std::future<returnType> res = promise->get_future();

		// 创建 run 和 cancel 回调
		std::function<void()> run_fn;
		std::function<void()> cancel_fn;

		if constexpr (std::is_void_v<returnType>) {
			run_fn = [bound = std::move(bound), promise]() {
				try {
					bound();
					promise->set_value();
				}
				catch (...) {
					promise->set_exception(std::current_exception());
				}
				};
		}
		else {
			run_fn = [bound = std::move(bound), promise]() {
				try {
					promise->set_value(bound());
				}
				catch (...) {
					promise->set_exception(std::current_exception());
				}
				};
		}

		cancel_fn = [promise]() {
			try {
				promise->set_exception(std::make_exception_ptr(std::runtime_error("task cancelled")));
			}
			catch (...) {
				// promise 可能已被设置（极少数竞态），忽略异常
			}
			};

		{
			std::unique_lock<std::mutex> lock(queueMutex);

			if (stop.load()) {
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}

			tasks.emplace(TaskWrapper{ std::move(run_fn), std::move(cancel_fn) });
		}
		condition.notify_one();
		return res;
	}


}