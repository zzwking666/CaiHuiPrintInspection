#pragma once

#include <shared_mutex>

#include"dsl_IPriorityQueue.hpp"

namespace rw {
	namespace dsl {
		template <class T, class Priority = size_t>
		class DHeapLockFree : public IPriorityQueue<T, Priority> {
		public:
			explicit DHeapLockFree(size_t d = 4) : _d(d) {}

			DHeapLockFree(std::function<bool(const T&, const T&)> compareNodeEqual,
				std::function<bool(const Priority&, const Priority&)> compareNodePriority,
				size_t d = 4)
				: _d(d), _compareNodeEqual(compareNodeEqual), _compareNodePriority(compareNodePriority) {
			}

			~DHeapLockFree() override = default;

		public:
			T top() override
			{
				std::unique_lock<std::shared_mutex> lock(_mutex); // 使用独占锁以确保线程安全
				if (_heap_array.empty()) {
					throw std::runtime_error("Heap is empty");
				}
				T top_element = _heap_array.front().first; // 获取堆顶元素
				std::swap(_heap_array.front(), _heap_array.back()); // 将堆顶元素与最后一个元素交换
				_heap_array.pop_back(); // 删除最后一个元素（即原堆顶）
				if (!_heap_array.empty()) {
					push_down(0); // 调整堆以保持堆性质
				}
				return top_element; // 返回堆顶元素
			}

			T peek() override
			{
				std::shared_lock<std::shared_mutex> lock(_mutex);
				if (_heap_array.empty()) {
					throw std::runtime_error("Heap is empty");
				}
				return _heap_array.front().first;
			}

			T top(bool& isGet) override
			{
				std::unique_lock<std::shared_mutex> lock(_mutex); // 使用独占锁以确保线程安全
				if (_heap_array.empty()) {
					isGet = false; // 堆为空，无法获取元素
					return T(); // 返回默认构造的 T 对象
				}
				isGet = true; // 成功获取元素
				T top_element = _heap_array.front().first; // 获取堆顶元素
				std::swap(_heap_array.front(), _heap_array.back()); // 将堆顶元素与最后一个元素交换
				_heap_array.pop_back(); // 删除最后一个元素（即原堆顶）
				if (!_heap_array.empty()) {
					push_down(0); // 调整堆以保持堆性质
				}
				return top_element; // 返回堆顶元素
			}

			T peek(bool& isGet) override
			{
				std::shared_lock<std::shared_mutex> lock(_mutex); // 使用共享锁以确保线程安全
				if (_heap_array.empty()) {
					isGet = false; // 堆为空，无法查看元素
					return T(); // 返回默认构造的 T 对象
				}
				isGet = true; // 成功查看元素
				return _heap_array.front().first; // 返回堆顶元素
			}

			void insert(T element, Priority priority) override
			{
				std::unique_lock<std::shared_mutex> lock(_mutex);
				_heap_array.emplace_back(element, priority);
				bubble_up(_heap_array.size() - 1);
			}

			void remove(T element) override
			{
				std::unique_lock<std::shared_mutex> lock(_mutex);
				auto it = std::find_if(_heap_array.begin(), _heap_array.end(),
					[element, this](const std::pair<T, Priority>& p) {
						return _compareNodeEqual(p.first, element);
					});
				if (it != _heap_array.end()) {
					std::swap(*it, _heap_array.back());
					_heap_array.pop_back();
					if (it != _heap_array.end()) {
						bubble_up(std::distance(_heap_array.begin(), it));
						push_down(std::distance(_heap_array.begin(), it));
					}
				}
			}

			void update(T element, Priority priority) override
			{
				std::unique_lock<std::shared_mutex> lock(_mutex);
				auto it = std::find_if(_heap_array.begin(), _heap_array.end(),
					[element, this](const std::pair<T, Priority>& p) {
						return _compareNodeEqual(p.first, element);
					});
				if (it != _heap_array.end()) {
					it->second = priority;
					bubble_up(std::distance(_heap_array.begin(), it));
					push_down(std::distance(_heap_array.begin(), it));
				}
			}

			size_t size() const override {
				std::shared_lock<std::shared_mutex> lock(_mutex);
				return _heap_array.size();
			}

			void clear() override
			{
				std::unique_lock<std::shared_mutex> lock(_mutex);
				_heap_array.clear();
			}

		private:
			void bubble_up(size_t index) {
				while (index > 0) {
					size_t p = parent(index);
					if (compare(index, p)) {
						std::swap(_heap_array[index], _heap_array[p]);
						index = p;
					}
					else {
						break;
					}
				}
			}

			void push_down(size_t index) {
				size_t child_index = child(index, 1);
				while (child_index < _heap_array.size()) {
					size_t min_index = index;
					for (size_t i = 0; i < _d; i++) {
						if (child_index + i < _heap_array.size() && compare(child_index + i, min_index)) {
							min_index = child_index + i;
						}
					}
					if (min_index != index) {
						std::swap(_heap_array[index], _heap_array[min_index]);
						index = min_index;
						child_index = child(index, 1);
					}
					else {
						break;
					}
				}
			}

			size_t parent(size_t index) const {
				return (index - 1) / _d;
			}

			size_t child(size_t index, size_t k) const {
				return index * _d + k;
			}

			bool compare(size_t i, size_t j) {
				return _compareNodePriority(_heap_array[i].second, _heap_array[j].second);
			}

		private:
			size_t _d;
			std::vector<std::pair<T, Priority>> _heap_array;
			mutable std::shared_mutex _mutex;
			std::function<bool(const T&, const T&)> _compareNodeEqual;
			std::function<bool(const Priority&, const Priority&)> _compareNodePriority;
		};
	}
}