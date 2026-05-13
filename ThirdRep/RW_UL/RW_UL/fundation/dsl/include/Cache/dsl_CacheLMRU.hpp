#pragma once

#include"dsl_Cache.hpp"
#include<list>
#include<unordered_map>

namespace rw {
	namespace dsl {
		template <typename Key, typename Value, bool IsLRU = true>
		class CacheLMRU final
			:public ICache<Key, Value> {
			MAKE_FRIEND_TEST_CLASS(CacheLRU_Test)
				MAKE_FRIEND_TEST_CLASS(CacheMRU_Test_Api)
		public:
			explicit CacheLMRU(size_t capacity)
				: ICache<Key, Value>(capacity) {
				_cache.reserve(capacity);
			}

			std::optional<Value> get(const Key& key) override {
				auto it = _cache.find(key);
				if (it == _cache.end()) {
					return std::nullopt;
				}
				_list.splice(_list.begin(), _list, it->second);
				return it->second->second;
			}

			bool set(const Key& key, const Value& value) override {
				// If the key is already present, update its value and move it to the front of the list
				auto it = _cache.find(key);
				if (it != _cache.end()) {
					// Update the value
					it->second->second = value;
					// Move the key to the front of the list
					_list.splice(_list.begin(), _list, it->second);
					// Return false since we did not insert a new key
					return false;
				}
				if (_cache.size() >= this->_capacity) {
					// Remove the least recently used key
					_cache.erase(_list.back().first);
					if constexpr (IsLRU) {
						_list.pop_back();
					}
					else {
						_list.pop_front();
					}
				}
				// Insert the new key at the front of the list
				_list.emplace_front(key, value);
				// Insert the new key in the cache
				_cache.insert({ key, _list.begin() });
				// Return true since we inserted a new key
				return true;
			}

			[[nodiscard]] size_t size() const override {
				return _cache.size();
			}

			bool resizeCapacity(size_t capacity)override {
				if (capacity < this->_capacity) {
					// Remove the least recently used keys until the size of the cache is less than the new capacity
					while (_cache.size() > capacity) {
						if constexpr (IsLRU) {
							_cache.erase(_list.back().first);
							_list.pop_back();
						}
						else {
							_cache.erase(_list.front().first);
							_list.pop_front();
						}
					}
				}
				this->_capacity = capacity;
				return true;
			}

			void clear() override {
				_list.clear();
				_cache.clear();
			}
		private:
			std::list<std::pair<Key, Value>> _list;
			std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> _cache;
		};
	} // namespace dsl
} // namespace rw