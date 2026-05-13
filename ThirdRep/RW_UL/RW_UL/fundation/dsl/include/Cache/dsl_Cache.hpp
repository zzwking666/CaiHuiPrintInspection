#pragma once

#include"dsl_core.hpp"

#include<optional>

FORWARD_DECLARE_TEST_CLASS(CacheLRU_Test)
FORWARD_DECLARE_TEST_CLASS(CacheLMFU_Api_Test)
FORWARD_DECLARE_TEST_CLASS(CacheMRU_Test_Api)

namespace rw {
	namespace dsl {
		enum class CachePolicy
		{
			LRU,
			LFU,
			MRU,
			MFU,
			Clock,
			FIFO,
			LIFO
		};

		template <typename Key, typename Value>
		class ICache {
		public:
			using CacheNode = std::pair<Key, Value>;
		public:
			// Constructor with capacity
			explicit ICache(size_t capacity) : _capacity(capacity) {}

			// Get the value associated with the key, if it exists
			virtual std::optional<Value> get(const Key& key) = 0;

			// Put the key-value pair in the cache
			virtual bool set(const Key& key, const Value& value) = 0;

			// Get the number of key-value pairs in the cache
			[[nodiscard]] virtual size_t size() const = 0;

			// Get the capacity of the cache
			[[nodiscard]] virtual size_t capacity() const { return _capacity; }

			virtual void clear() = 0;

			// Virtual destructor
			virtual ~ICache() = default;

			virtual bool resizeCapacity(size_t capacity) = 0;
		public:
			// Prevent move construction
			ICache(ICache&&) = delete;

			// Prevent move assignment
			ICache& operator=(ICache&&) = delete;

			// Prevent copy construction
			ICache(const ICache&) = delete;

			// Prevent copy assignment
			ICache& operator=(const ICache&) = delete;
		protected:
			size_t _capacity;
		};
	} // namespace dsl
} // namespace rw