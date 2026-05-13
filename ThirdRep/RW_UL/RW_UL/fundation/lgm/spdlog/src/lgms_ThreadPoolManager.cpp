#include "lgms_ThreadPoolManager.hpp"

#include <spdlog/async.h>

namespace rw
{
	namespace lgm
	{
		ThreadPoolManager& ThreadPoolManager::instance()
		{
			static ThreadPoolManager s_instance;
			return s_instance;
		}

		std::shared_ptr<spdlog::details::thread_pool> ThreadPoolManager::getOrCreate()
		{
			std::call_once(m_initFlag, [&]() {
				m_pool = std::make_shared<spdlog::details::thread_pool>(8192, 1);
				});
			return m_pool;
		}

		std::shared_ptr<spdlog::details::thread_pool> ThreadPoolManager::get() const
		{
			return m_pool;
		}
	}
}
