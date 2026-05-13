#pragma once

#include <memory>
#include <mutex>
#include <cstddef>
#include <spdlog/async.h>

namespace rw
{
	namespace lgm
	{
		class ThreadPoolManager
		{
		public:
			using thread_pool_type = decltype(spdlog::init_thread_pool(0, 0));

			// 获取单例
			static ThreadPoolManager& instance();

			// 若尚未初始化则初始化并返回线程池；已初始化则直接返回
			std::shared_ptr<spdlog::details::thread_pool> getOrCreate();

			// 返回已存在的线程池；若尚未初始化，则返回空指针
			std::shared_ptr<spdlog::details::thread_pool> get() const;

		private:
			ThreadPoolManager() = default;
			~ThreadPoolManager() = default;
			ThreadPoolManager(const ThreadPoolManager&) = delete;
			ThreadPoolManager& operator=(const ThreadPoolManager&) = delete;

			mutable std::once_flag m_initFlag;
			std::shared_ptr<spdlog::details::thread_pool> m_pool;
		};
	}
}