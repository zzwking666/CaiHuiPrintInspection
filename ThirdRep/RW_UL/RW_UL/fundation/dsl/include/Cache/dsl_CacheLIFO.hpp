#pragma once

#include"Cache/dsl_Cache.hpp"

#include <unordered_map>
#include <stack>
#include <memory>

namespace rw {
	namespace dsl {
		template <typename Key, typename Value>
		class CacheLIFO final : public ICache<Key, Value> {
		public:
			using CacheNode = typename ICache<Key, Value>::CacheNode;
		public:
			// Constructor with capacity
			explicit CacheLIFO(size_t capacity) : ICache<Key, Value>(capacity) {}
			// Get the value associated with the key, if it exists
			std::optional<Value> get(const Key& key) override {
				auto it = _cache.find(key);
				if (it == _cache.end()) {
					return std::nullopt;
				}
				return it->second;
			}
			// Put the key-value pair in the cache
			bool set(const Key& key, const Value& value) override {
				auto it = _cache.find(key);
				if (it != _cache.end()) {
					it->second = value;
					return false;
				}
				if (_cache.size() >= this->_capacity) {
					_cache.erase(_lifo.top());
					_lifo.pop();
				}
				_cache.insert({ key, value });
				_lifo.push(key);
				return true;
			}
			// Get the number of key-value pairs in the cache
			[[nodiscard]] size_t size() const override {
				return _cache.size();
			}
			void clear() override {
				_cache.clear();
				while (!_lifo.empty()) {
					_lifo.pop();
				}
			}
			bool resizeCapacity(size_t capacity) override {
				if (capacity < this->_capacity) {
					while (_cache.size() > capacity) {
						_cache.erase(_lifo.top());
						_lifo.pop();
					}
				}
				this->_capacity = capacity;
				return true;
			}
		private:
			std::unordered_map<Key, Value> _cache;
			std::stack<Key> _lifo;
		};
	} // namespace dsl
} // namespace rw