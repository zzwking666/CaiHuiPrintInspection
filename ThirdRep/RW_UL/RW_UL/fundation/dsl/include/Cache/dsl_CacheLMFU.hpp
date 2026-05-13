#pragma once

#include"dsl_Cache.hpp"

#include"dsl_PriorityQueue.hpp"

#include<list>
#include<unordered_map>
#include <chrono>

namespace rw {
	namespace dsl {
		template <typename Key, typename Value, bool IsLFU = true >
		class CacheLMFU final : public ICache<Key, Value> {
		private:

			/**
			 * @brief TimePoint
			 *
			 * The time point record the time when the node is inserted or updated
			 */
			using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

			/**
			 * @brief Record the node get or set times
			 *
			 * The counter record the times of the node get or set
			 */
			using Counter = size_t;
			/**
			 * @brief Priority
			 *
			 * This struct is used to record the priority of the node
			 */
			struct Priority {
				Counter counter;
				TimePoint timePoint;
				Priority(Counter counter, TimePoint timePoint)
					: counter(counter), timePoint(timePoint) {
				}

				bool operator<(const Priority& other) const {
					if (this->counter == other.counter) {
						return this->timePoint < other.timePoint;
					}
					else {
						//If the cache is LFU, the node with the lowest counter has the highest priority
						if constexpr (IsLFU) {
							return this->counter < other.counter;
						}
						//If the cache is LRU, the node with the latest timePoint has the highest priority
						else {
							return this->counter > other.counter;
						}
					}
				}

				bool operator==(const Priority& other) const {
					return this->counter == other.counter && this->timePoint == other.timePoint;
				}

				bool operator!=(const Priority& other) const {
					return !(*this == other);
				}

				bool operator>(const Priority& other) const {
					return !(*this < other) && !(*this == other);
				}

				bool operator<=(const Priority& other) const {
					return *this < other || *this == other;
				}

				bool operator>=(const Priority& other) const {
					return *this > other || *this == other;
				}
			};

			/**
			 * @brief Node
			 *
			 * This struct is used to record the node in the cache
			 */
			struct Node {
				Key key;
				Value value;
				Counter counter;
				TimePoint timePoint;
				Node(Key key, int value, Counter counter, TimePoint timePoint)
					: key(key), value(value), counter(counter), timePoint(timePoint) {
				}
			};
		public:
			explicit CacheLMFU(size_t capacity)
				: ICache<Key, Value>(capacity) {
				_heap.setCompareNodePriority([](const Priority& a, const Priority& b) {
					return a < b;
					});
				_heap.setCompareNodeEqual([](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
					return a->key == b->key;
					});
			}
			~CacheLMFU() override = default;
		public:
			std::optional<Value> get(const Key& key) override {
				// If the key is not in the cache, return nullopt
				auto it = this->_cache.find(key);
				if (it == this->_cache.end()) {
					return std::nullopt;
				}
				//Record the time when the node is get
				auto now = std::chrono::system_clock::now();

				//Update the counter and timePoint of the node
				++it->second->counter;
				Priority newPriority(it->second->counter, now);

				//Update the priority of the node in the heap
				this->_heap.update(it->second, newPriority);
				it->second->timePoint = now;
				return it->second->value;
			}

			bool set(const Key& key, const Value& value) override {
				//If the key is in the cache, update the value and counter of the node
				auto it = this->_cache.find(key);
				auto now = std::chrono::system_clock::now();
				//Record the time when the node is set
				if (it != this->_cache.end()) {
					it->second->value = value;
					++it->second->counter;
					it->second->timePoint = now;
					Priority newPriority(it->second->counter, now);
					_heap.update(it->second, newPriority);
					return false;
				}
				//Check if the cache is full,if full, remove the node with the lowest priority
				if (this->_cache.size() >= this->_capacity) {
					auto node = this->_heap.top();
					this->_cache.erase(node->key);
				}
				//Insert the node into the cache
				auto node = std::make_shared<Node>(key, value, 1, now);
				this->_cache[key] = node;
				Priority newPriority(1, now);
				this->_heap.insert(node, newPriority);
				return true;
			}

			[[nodiscard]] size_t size() const override {
				//Return the size of the cache
				return this->_cache.size();
			}
			bool resizeCapacity(size_t capacity) override {
				//Resize the cache
				this->_capacity = capacity;
				//Remove the node with the lowest priority until the size of the cache is less than the capacity
				while (this->_cache.size() > this->_capacity) {
					auto node = this->_heap.top();
					this->_cache.erase(node->key);
				}
				return true;
			}

			void clear() override {
				//Clear the cache
				this->_cache.clear();
				this->_heap.clear();
			}

		private:
			std::unordered_map<Key, std::shared_ptr<Node> > _cache;
			DHeap<std::shared_ptr<Node>, Priority> _heap;
		};
	} // namespace dsl
} // namespace rw