#pragma once

#include "dsl_core.hpp"
#include "dsl_core.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <functional>
#include<mutex>

namespace rw
{
	namespace dsl
	{
		/**
		 * @brief: Priority Queue Interface
		 *
		 * @template variable:
		 *   - T: The type of the elements in the priority queue
		 *   - Priority: The type of the priority of the elements in the priority queue
		 *
		 * @details: Detailed description of the template class
		 *
		 */
		template <class T, class Priority = size_t>
		class IPriorityQueue
		{
		public:
			/**
			 *@Parameters:
			 *  -a: The first element to be compared
			 *  -b: The second element to be compared
			 *@Methods:
			 * Compare two elements to determine whether they are equal
			 *@Returns: bool
			 * If the two elements are equal, return true, otherwise return false
			 *@Throws:
			 *
			 */
			using CompareNodeEqual = std::function<bool(const T&, const T&)>;

			/**
			 *@Parameters:
			 *  -a: The first priority to be compared
			 *  -b: The second priority to be compared
			 *@Methods:
			 * Compare two priorities to determine the priority of the two elements
			 *@Returns: bool
			 * If the first priority is higher than the second priority, return true, otherwise return false
			 *@Throws:
			 *
			 */
			using CompareNodePriority = std::function<bool(const Priority&, const Priority&)>;

		public:
			/**
			 *@Parameters:
			 *  -CompareNodeEqual: The function to compare two elements
			 *  -CompareNodePriority: The function to compare two priorities
			 *
			 *@Methods:
			 *  Constructor
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			IPriorityQueue(
				CompareNodeEqual compareNodeEqual,
				CompareNodePriority compareNodePriority)
				:_compareNodeEqual(compareNodeEqual), _compareNodePriority(compareNodePriority) {
			}

			IPriorityQueue() = default;

			virtual ~IPriorityQueue() = default;
		protected:
			CompareNodeEqual _compareNodeEqual{ [](const T& a,const T& b) {
					return a == b;
				} };
			CompareNodePriority _compareNodePriority{ [](const Priority& a, const Priority& b) {
					return a < b;
				} };

		public:
			/**
		   *@Parameters:
		   *  void
		   *@Methods:
		   *  Get the top element of the priority queue and remove it
		   *@Returns: template variable T
		   *  The top element of the priority queue
		   *@Throws:
		   *  1. std::runtime_error: If the priority queue is empty
		   */
			virtual T top() = 0;

			virtual T top(bool& isGet) = 0;

			/**
			*@Parameters:
			*  void
			*@Methods:
			*  Get the top element of the priority queue without removing it
			*@Returns: template variable T
			*  The top element of the priority queue
			*@Throws:
			*  1. std::runtime_error: If the priority queue is empty
			*/
			virtual T peek() = 0;

			virtual T peek(bool& isGet) = 0;

			/**
			 *@Parameters:
			 *  1. T element: The element to be inserted
			 *  2. size_t priority: The priority of the element
			 *@Methods:
			 *  Insert an element into the priority queue with a priority
			 *@Returns: void
			 *@Throws: void
			 *@Warning:
			 *  1.If the insert element 's value is the same as the existing element, the new element will be inserted as a new element
			 *  2.If some elements have the same priority, the order of the elements is not guaranteed
			 */
			virtual void insert(T element, Priority priority) = 0;

			/**
			 *@Parameters:
			 *  1. T element: The element to be removed
			 *@Methods:
			 *  Remove an element from the priority queue
			 *@Returns: void
			 *@Throws: void
			 *@Warning:
			 * 1. If the element is not in the priority queue, the method will do nothing
			 * 2. If some elements in the priority queue have the same value,the element to be removed is not guaranteed
			 */
			virtual void remove(T element) = 0;

			/**
			*@Parameters:
			*  1. T element: The element to be updated
			*  2. size_t priority: The new priority of the element
			*@Methods:
			*  Update the priority of an element in the priority queue
			*@Returns: void
			*@Throws: void
			*@Warning:
			* 1. If the element is not in the priority queue, the method will do nothing
			* 2. If some elements in the priority queue have the same value,the element to be updated is not guaranteed
			*/
			virtual void update(T element, Priority priority) = 0;

			/**
			*@Parameters:
			*  void
			*@Methods:
			*  Get the size of the priority queue
			*@Returns: size_t
			*  The size of the priority queue
			*@Throws: void
			*/
			virtual size_t size() const = 0;

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Clear the priority queue
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			virtual void clear() = 0;
		public:

			/**
			 *@Parameters:
			 *  compareNodeEqual: The function to compare two elements
			 *@Methods:
			 *  Set the function to compare two elements, if the priority queue is not empty, throw an exception
			 *@Returns: void
			 *
			 *@Throws:
			 *  -std::runtime_error: If the priority queue is not empty
			 */
			void setCompareNodeEqual(CompareNodeEqual compareNodeEqual)
			{
				if (this->size()) {
					throw std::runtime_error("Can not set compareNodeEqual");
				}
				this->_compareNodeEqual = compareNodeEqual;
			}

			/**
			 *@Parameters:
			 *  -compareNodePriority: The function to compare two priorities
			 *@Methods:
			 *  Set the function to compare two priorities, if the priority queue is not empty, throw an exception
			 *@Returns: void
			 *
			 *@Throws:
			 *  -std::runtime_error: If the priority queue is not empty
			 */
			void setCompareNodePriority(CompareNodePriority compareNodePriority)
			{
				if (this->size()) {
					throw std::runtime_error("Can not set compareNodePriority");
				}
				this->_compareNodePriority = compareNodePriority;
			}
		};
	}
}