#pragma once

#include "dsl_Cache.hpp"
#include"dsl_core.hpp"

#include <unordered_map>
#include <queue>
#include <memory>
#include <chrono> // For time-related types

namespace rw {
	namespace dsl {
		template <typename Key, typename Value>
		class CacheFIFO final : public ICache<Key, Value> {
		public:
			using CacheNode = typename ICache<Key, Value>::CacheNode;
		public:
			// Constructor with capacity
			explicit CacheFIFO(size_t capacity) : ICache<Key, Value>(capacity) {}
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
				// If the key is already present, update its value
				auto it = _cache.find(key);
				if (it != _cache.end()) {
					it->second = value;
					return false;
				}
				// If the cache is full, remove the least recently used key
				if (_cache.size() >= this->_capacity) {
					_cache.erase(_fifo.front());
					_fifo.pop();
				}
				// Insert the new key-value pair
				_cache.insert({ key, value });
				_fifo.push(key);
				return true;
			}
			// Get the number of key-value pairs in the cache
			[[nodiscard]] size_t size() const override {
				return _cache.size();
			}
			void clear() override {
				_cache.clear();
				while (!_fifo.empty()) {
					_fifo.pop();
				}
			}
			bool resizeCapacity(size_t capacity) override {
				if (capacity < this->_capacity) {
					while (_cache.size() > capacity) {
						_cache.erase(_fifo.front());
						_fifo.pop();
					}
				}
				this->_capacity = capacity;
				return true;
			}
		private:
			std::unordered_map<Key, Value> _cache;
			std::queue<Key> _fifo;
		};

		// Specialization for std::chrono::time_point as Key
		template <typename Value>
		class CacheFIFO<std::chrono::time_point<std::chrono::system_clock>, Value> final
			: public ICache<std::chrono::time_point<std::chrono::system_clock>, Value> {
		public:
			using Key = std::chrono::time_point<std::chrono::system_clock>;
			using CacheNode = typename ICache<Key, Value>::CacheNode;

			explicit CacheFIFO(size_t capacity) : ICache<Key, Value>(capacity) {}

			std::optional<Value> get(const Key& key) override {
				auto it = _cache.find(key);
				if (it == _cache.end()) {
					return std::nullopt;
				}
				return it->second;
			}

			bool set(const Key& key, const Value& value) override {
				auto it = _cache.find(key);
				if (it != _cache.end()) {
					it->second = value;
					return false;
				}
				if (_cache.size() >= this->_capacity) {
					_cache.erase(_fifo.front());
					_fifo.pop();
				}
				_cache.insert({ key, value });
				_fifo.push(key);
				return true;
			}

			[[nodiscard]] size_t size() const override {
				return _cache.size();
			}

			void clear() override {
				_cache.clear();
				while (!_fifo.empty()) {
					_fifo.pop();
				}
			}

			bool resizeCapacity(size_t capacity) override {
				if (capacity < this->_capacity) {
					while (_cache.size() > capacity) {
						_cache.erase(_fifo.front());
						_fifo.pop();
					}
				}
				this->_capacity = capacity;
				return true;
			}

		private:
			std::unordered_map<Key, Value> _cache;
			std::queue<Key> _fifo;
		};
	} // namespace dsl
} // namespace rw