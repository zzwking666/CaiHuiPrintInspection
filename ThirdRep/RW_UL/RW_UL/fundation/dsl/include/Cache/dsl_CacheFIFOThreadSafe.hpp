#pragma once

#include "dsl_Cache.hpp"
#include "dsl_core.hpp"

#include <unordered_map>
#include <queue>
#include <memory>
#include <chrono>
#include <mutex>

namespace rw {
	namespace dsl {
		// Thread-safe version of CacheFIFO
		template <typename Key, typename Value>
		class CacheFIFOThreadSafe final : public ICache<Key, Value> {
		public:
			using CacheNode = typename ICache<Key, Value>::CacheNode;

			explicit CacheFIFOThreadSafe(size_t capacity) : ICache<Key, Value>(capacity) {}

			std::optional<Value> get(const Key& key) override {
				std::lock_guard<std::mutex> lock(_mutex);
				auto it = _cache.find(key);
				if (it == _cache.end()) {
					return std::nullopt;
				}
				return it->second;
			}

			bool set(const Key& key, const Value& value) override {
				std::lock_guard<std::mutex> lock(_mutex);
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
				std::lock_guard<std::mutex> lock(_mutex);
				return _cache.size();
			}

			void clear() override {
				std::lock_guard<std::mutex> lock(_mutex);
				_cache.clear();
				while (!_fifo.empty()) {
					_fifo.pop();
				}
			}

			bool resizeCapacity(size_t capacity) override {
				std::lock_guard<std::mutex> lock(_mutex);
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
			mutable std::mutex _mutex;
		};

		// Thread-safe specialization for std::chrono::time_point as Key
		template <typename Value>
		class CacheFIFOThreadSafe<std::chrono::time_point<std::chrono::system_clock>, Value> final
			: public ICache<std::chrono::time_point<std::chrono::system_clock>, Value> {
		public:
			using Key = std::chrono::time_point<std::chrono::system_clock>;
			using CacheNode = typename ICache<Key, Value>::CacheNode;

			explicit CacheFIFOThreadSafe(size_t capacity) : ICache<Key, Value>(capacity) {}

			std::optional<Value> get(const Key& key) override {
				std::lock_guard<std::mutex> lock(_mutex);
				auto it = _cache.find(key);
				if (it == _cache.end()) {
					return std::nullopt;
				}
				return it->second;
			}

			bool set(const Key& key, const Value& value) override {
				std::lock_guard<std::mutex> lock(_mutex);
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
				std::lock_guard<std::mutex> lock(_mutex);
				return _cache.size();
			}

			void clear() override {
				std::lock_guard<std::mutex> lock(_mutex);
				_cache.clear();
				while (!_fifo.empty()) {
					_fifo.pop();
				}
			}

			bool resizeCapacity(size_t capacity) override {
				std::lock_guard<std::mutex> lock(_mutex);
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
			mutable std::mutex _mutex;
		};
	} // namespace dsl
} // namespace rw