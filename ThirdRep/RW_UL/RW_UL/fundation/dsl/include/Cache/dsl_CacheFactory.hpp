#pragma once

#include"dsl_Cache.hpp"
#include"dsl_CacheLMRU.hpp"
#include"dsl_CacheLMFU.hpp"
#include"dsl_CacheClock.hpp"
#include"dsl_CacheFIFO.hpp"
#include"dsl_CacheLIFO.hpp"

namespace rw {
	namespace dsl {
		class CacheFactory {
		public:
			template <typename Key, typename Value>
			static std::shared_ptr<ICache<Key, Value>> createCache(CachePolicy policy, size_t capacity) {
				switch (policy) {
				case CachePolicy::LRU:
					return std::make_shared<CacheLMRU<Key, Value>>(capacity);
				case CachePolicy::LFU:
					return std::make_shared<CacheLMFU<Key, Value>>(capacity);
				case CachePolicy::MRU:
					return std::make_shared<CacheLMRU<Key, Value, false>>(capacity);
				case CachePolicy::MFU:
					return std::make_shared<CacheLMFU<Key, Value, false>>(capacity);
				case CachePolicy::Clock:
					return std::make_shared<CacheClock<Key, Value>>(capacity);
				case CachePolicy::FIFO:
					return std::make_shared<CacheFIFO<Key, Value>>(capacity);
				case CachePolicy::LIFO:
					return std::make_shared<CacheLIFO<Key, Value>>(capacity);
				default:
					return nullptr;
				}
			}
		};
	} // namespace dsl
} // namespace rw