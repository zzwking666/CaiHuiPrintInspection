#pragma once

#include"dsl_IPriorityQueue.hpp"

namespace rw
{
	namespace dsl
	{
		template <class T, class Priority = size_t>
		class DHeap : public IPriorityQueue<T, Priority>
		{
		public:
			/**
			 *@Parameters:
			 *  -d: The number of children of each node
			 *     note: According to test,when d=4 or d=5, the performance of the heap is better
			 *@Methods:
			 *  Constructor
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			DHeap(size_t d = 4) :_d(d) {}

			/**
			*@Parameters:
			*  -compareNodeEqual: The function to compare two elements
			*  -compareNodePriority: The function to compare two priorities
			*  -d: The number of children of each node
			*@Methods:
			*  Constructor
			*@Returns: void
			*
			*@Throws:
			*
			*/
			DHeap(std::function<bool(const T&, const T&)> compareNodeEqual,
				std::function<bool(const Priority&, const Priority&)> compareNodePriority,
				size_t d = 4) :_d(d) {
				this->_compareNodeEqual = compareNodeEqual;
				this->_compareNodePriority = compareNodePriority;
			}
			~DHeap() = default;

		public:
			T top() override {
				if (this->_heap_array.empty()) {
					throw std::runtime_error("Heap is empty");
				}
				T top_element = this->_heap_array.front().first;
				std::swap(this->_heap_array.front(), this->_heap_array.back());
				this->_heap_array.pop_back();
				if (!this->_heap_array.empty()) {
					push_down(0);
				}
				return top_element;
			}

			T peek() override {
				if (this->_heap_array.empty()) {
					throw std::runtime_error("Heap is empty");
				}
				return this->_heap_array.front().first;
			}

			T top(bool& isGet) override
			{
				if (this->_heap_array.empty()) {
					isGet = false; // 堆为空，无法获取元素
					return T(); // 返回默认构造的 T 对象
				}
				isGet = true; // 成功获取元素
				T top_element = this->_heap_array.front().first;
				std::swap(this->_heap_array.front(), this->_heap_array.back());
				this->_heap_array.pop_back();
				if (!this->_heap_array.empty()) {
					push_down(0);
				}
				return top_element;
			}

			T peek(bool& isGet) override
			{
				if (this->_heap_array.empty()) {
					isGet = false; // 堆为空，无法查看元素
					return T(); // 返回默认构造的 T 对象
				}
				isGet = true; // 成功查看元素
				return this->_heap_array.front().first;
			}

			void insert(T element, Priority priority) override {
				this->_heap_array.emplace_back(element, priority);
				bubble_up(this->_heap_array.size() - 1);
			}
			void remove(T element) override {
				auto it = std::find_if(this->_heap_array.begin(), this->_heap_array.end(),
					[element, this](const std::pair<T, Priority>& p) { return  this->_compareNodeEqual(p.first, element); });
				if (it != this->_heap_array.end()) {
					std::swap(*it, this->_heap_array.back());
					this->_heap_array.pop_back();
					if (it != this->_heap_array.end()) {
						bubble_up(std::distance(this->_heap_array.begin(), it));
						push_down(std::distance(this->_heap_array.begin(), it));
					}
				}
			}
			void update(T element, Priority priority) override {
				auto it = std::find_if(this->_heap_array.begin(), this->_heap_array.end(),
					[element, this](const std::pair<T, Priority>& p) { return  this->_compareNodeEqual(p.first, element); });
				if (it != this->_heap_array.end()) {
					it->second = priority;
					bubble_up(std::distance(this->_heap_array.begin(), it));
					push_down(std::distance(this->_heap_array.begin(), it));
				}
			}
			size_t size() const override {
				return this->_heap_array.size();
			}

			void clear() override {
				this->_heap_array.clear();
			}

		private:
			void bubble_up(size_t index) {
				while (index > 0) {
					size_t p = parent(index);
					if (compare(index, p)) {
						std::swap(this->_heap_array[index], this->_heap_array[p]);
						index = p;
					}
					else {
						break;
					}
				}
			}
			void push_down(size_t index) {
				size_t child_index = child(index, 1);
				while (child_index < this->_heap_array.size()) {
					size_t min_index = index;
					for (size_t i = 0; i < _d; i++) {
						if (child_index + i < this->_heap_array.size() && compare(child_index + i, min_index)) {
							min_index = child_index + i;
						}
					}
					if (min_index != index) {
						std::swap(this->_heap_array[index], this->_heap_array[min_index]);
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
				return this->_compareNodePriority(this->_heap_array[i].second, this->_heap_array[j].second);
			}

		private:
			size_t _d;
			std::vector<std::pair<T, Priority>> _heap_array;
		};
	}
}