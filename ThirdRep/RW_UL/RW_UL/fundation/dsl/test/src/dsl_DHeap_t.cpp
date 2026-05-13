#include "dsl_pch_t.h"
#include "dsl_DHeap_t.hpp"

namespace dsl_PriorityQueue {
	/**
 * @brief Test the top method can throw exception when the heap is empty
 *
 * @details
 * Test Steps:
 * 1. Create a heap
 * 2. Call the top method
 * 3. Check if the exception is thrown
 *
 * Expected Result:
 * 1. The exception should be thrown
 * 2. The isException should be true
 *
 * Boundary Conditions:
 * 1. The heap is empty
 */
	TEST_F(DHeap_int_Test, topCanTrow) {
		bool isException = false;
		try {
			testObj->top();
		}
		catch (std::runtime_error& e) {
			isException = true;
		}
		ASSERT_TRUE(isException);
	}

	/**
	 * @brief Test the top method can get the value from the heap and delete it
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements into the heap
	 * 3. Call the top method
	 * 4. Check if the value is correct
	 *
	 * Expected Result:
	 * 1. The value should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap is not empty
	 */
	TEST_F(DHeap_int_Test, topCanGetValueAndDeleteFromHeap) {
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		testObj->setCompareNodePriority(compareNodePriority);
		testObj->insert(1, 1);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);

		ASSERT_EQ(testObj->top(), 5);
		ASSERT_EQ(testObj->top(), 4);
		ASSERT_EQ(testObj->top(), 3);
		ASSERT_EQ(testObj->top(), 2);
		ASSERT_EQ(testObj->top(), 1);
	}

	/**
	 * @brief Test the peek method can throw exception when the heap is empty
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Call the peek method
	 * 3. Check if the exception is thrown
	 *
	 * Expected Result:
	 * 1. The exception should be thrown
	 * 2. The isException should be true
	 *
	 * Boundary Conditions:
	 * 1. The heap is empty
	 */
	TEST_F(DHeap_int_Test, peekCanTrow) {
		bool isException = false;
		try {
			testObj->peek();
		}
		catch (std::runtime_error& e) {
			isException = true;
		}
		ASSERT_TRUE(isException);
	}

	/**
	 * @brief Test the peek method can get the value from the heap without deleting it
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements into the heap
	 * 3. Call the peek method
	 * 4. Check if the value is correct
	 *
	 * Expected Result:
	 * 1. The value should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap is not empty
	 */
	TEST_F(DHeap_int_Test, peekCanGetValueWithoutDeleteFromHeap) {
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		testObj->setCompareNodePriority(compareNodePriority);
		testObj->insert(1, 1);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);

		ASSERT_EQ(testObj->peek(), 5);
		ASSERT_EQ(testObj->peek(), 5);
	}

	/**
	 * @brief Test the insert method can insert an element into the heap
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements into the heap
	 * 3. Check if the size is correct
	 *
	 * Expected Result:
	 * 1. The size should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap is empty
	 */
	TEST_F(DHeap_int_Test, insertCanInsertElement) {
		testObj->insert(1, 1);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);
		ASSERT_EQ(testObj->size(), 5);
	}

	/**
	 * @brief Test the insert method can insert an element with the same priority into the heap
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements with the same priority into the heap
	 * 3. Check if the size is correct
	 *
	 * Expected Result:
	 * 1. The size should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap is empty
	 */
	TEST_F(DHeap_int_Test, insertCanInsertElementWithSamePriority) {
		testObj->insert(1, 1);
		testObj->insert(2, 1);
		testObj->insert(3, 1);
		testObj->insert(4, 1);
		testObj->insert(5, 1);
		ASSERT_EQ(testObj->size(), 5);
	}

	/**
	 * @brief Test the insert method can insert an element with the same value into the heap
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements with the same value into the heap
	 * 3. Check if the size is correct
	 *
	 * Expected Result:
	 * 1. The size should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap insert element 's value is the same as the existing element
	 */
	TEST_F(DHeap_int_Test, insertCanInsertElementWithSameValue) {
		testObj->insert(1, 1);
		testObj->insert(1, 2);
		testObj->insert(1, 3);
		testObj->insert(1, 4);
		testObj->insert(1, 5);
		ASSERT_EQ(testObj->size(), 5);
	}

	/**
	 * @brief Test the remove method can remove an element from the heap which is not existing
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements into the heap
	 * 3. Call the remove method
	 * 4. Check if the size is correct
	 *
	 * Expected Result:
	 * 1. The size should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap is not empty
	 */
	TEST_F(DHeap_int_Test, removeCanRemoveTheElementsWhichIsNotExisting) {
		testObj->insert(1, 1);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);
		testObj->remove(6);
		ASSERT_EQ(testObj->size(), 5);
	}

	/**
	 * @brief Test the heap which has some elements with the same priority and update the heap
	 *whether the order of the elements is correct
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements with the same priority into the heap
	 * 3. Insert some elements with the different priority into the heap
	 * 4. Update the elements with the same priority
	 * 5. Check the order of the elements
	 *
	 * Expected Result:
	 * 1. The order of the elements should be correct
	 * 2. The size should be correct
	 *
	 * Boundary Conditions:
	 * 1. Boundary conditions for the test
	 */
	TEST_F(DHeap_int_Test, insertSameElementAndUpdateHeap) {
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		testObj->setCompareNodePriority(compareNodePriority);
		testObj->insert(1, 1);
		testObj->insert(4, 4);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);
		testObj->insert(4, 4);
		testObj->insert(4, 4);
		testObj->insert(6, 6);
		testObj->insert(7, 7);
		testObj->insert(8, 8);
		testObj->insert(9, 9);
		testObj->insert(1, 1);
		testObj->insert(10, 10);
		testObj->insert(11, 11);
		std::vector<int> expected = { 11, 10, 9, 8, 7, 6, 5, 4, 4, 4, 4, 3, 2, 1, 1 };
		for (size_t i = 0; i < expected.size(); ++i) {
			auto top = testObj->top();
			ASSERT_EQ(top, expected[i]);
		}
	}

	/**
	 * @brief Test the update method can update the element in the heap
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements into the heap
	 * 3. Update the elements
	 * 4. Check the order of the elements
	 *
	 * Expected Result:
	 * 1. The order of the elements should be correct
	 * 2. The size should be correct
	 * 3. The elements should be updated
	 *
	 * Boundary Conditions:
	 * 1. The heap is empty
	 */
	TEST_F(DHeap_int_Test, updateCanUpdateElement) {
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		testObj->setCompareNodePriority(compareNodePriority);
		testObj->insert(1, 1);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);
		testObj->update(1, 5);
		testObj->update(2, 4);
		testObj->update(3, 3);
		testObj->update(4, 2);
		testObj->update(5, 1);
		std::vector<int> expected = { 1, 2, 3, 4, 5 };
		for (size_t i = 0; i < expected.size(); ++i) {
			ASSERT_EQ(testObj->top(), expected[i]);
		}
	}

	/**
	 * @brief Test the update method can update the element in the heap which is not existing
	 *
	 * @details
	 * Test Steps:
	 * 1. Create a heap
	 * 2. Insert some elements into the heap
	 * 3. Update the elements which is not existing
	 * 4. Check the size
	 * 5. Check the order of the elements
	 *
	 * Expected Result:
	 * 1. The order of the elements should be correct
	 * 2. The size should be correct
	 *
	 * Boundary Conditions:
	 * 1. The heap is empty
	 */
	TEST_F(DHeap_int_Test, updateElementWhichIsNotExisting) {
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		testObj->setCompareNodePriority(compareNodePriority);
		testObj->insert(1, 1);
		testObj->insert(2, 2);
		testObj->insert(3, 3);
		testObj->insert(4, 4);
		testObj->insert(5, 5);
		testObj->update(6, 6);
		std::vector<int> expected = { 5, 4, 3, 2, 1 };
		ASSERT_EQ(testObj->size(), 5);
		for (size_t i = 0; i < expected.size(); ++i) {
			ASSERT_EQ(testObj->top(), expected[i]);
		}
	}

	/**
  * @brief Test the heap which template is AccountEnity
  *
  * @details
  * Test Steps:
  * 1. Steps to perform the test
  *
  * Expected Result:
  * 1. Expected result of the test
  *
  * Boundary Conditions:
  * 1. Boundary conditions for the test
  */
	TEST_F(DHeap_AccountEnity_Test, getOrderByDefaultCompare) {
		std::function<bool(const AccountEnity&, const AccountEnity&)> compareNodeEqual = [](const AccountEnity& a, const AccountEnity& b) {
			return a.AccountName == b.AccountName;
			};
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		testObj->setCompareNodeEqual(compareNodeEqual);
		testObj->setCompareNodePriority(compareNodePriority);
		testObj->insert({ 1, 1, 1 }, 1);
		testObj->insert({ 2, 2, 2 }, 2);
		testObj->insert({ 3, 3, 3 }, 3);
		testObj->insert({ 4, 4, 4 }, 4);
		testObj->insert({ 5, 5, 5 }, 5);
		std::vector<AccountEnity> expected = { {5, 5, 5}, {4, 4, 4}, {3, 3, 3}, {2, 2, 2}, {1, 1, 1} };
		for (size_t i = 0; i < expected.size(); ++i) {
			ASSERT_EQ(testObj->top(), expected[i]);
		}
	}

	TEST(DHeapRefactor_PriorityNode, testThePriorityTem) {
		DHeap<int, PriorityNode> testObj;
		std::function<bool(const PriorityNode&, const PriorityNode&)> compareNodePriority = [](const PriorityNode& a, const PriorityNode& b) {
			if (a.priorityFirst == b.priorityFirst) {
				return a.prioritySecond > b.prioritySecond;
			}
			else {
				return a.priorityFirst > b.priorityFirst;
			}
			};
		testObj.setCompareNodePriority(compareNodePriority);
		testObj.insert(1, { 1, 1 });
		testObj.insert(2, { 1, 2 });
		testObj.insert(3, { 1, 3 });
		testObj.insert(4, { 2, 4 });
		testObj.insert(5, { 2, 5 });
		testObj.insert(6, { 2, 6 });
		testObj.insert(7, { 3, 7 });
		testObj.insert(8, { 3, 8 });
		testObj.insert(9, { 3, 9 });
		testObj.insert(10, { 4, 10 });

		std::vector<int> expectedValue = { 10,9,8,7,6,5,4,3,2,1 };
		for (size_t i = 0; i < expectedValue.size(); ++i) {
			auto top = testObj.top();
			ASSERT_EQ(top, expectedValue[i]);
		}
	}

	TEST(DHeapRefactor_PriorityNode, testTheExpectionWhenSetCompare) {
		DHeap<int, size_t> testObj;
		testObj.insert(1, 1);
		testObj.insert(2, 2);
		testObj.insert(3, 3);
		std::function<bool(const size_t&, const size_t&)> compareNodePriority = [](const size_t& a, const size_t& b) {
			return a > b;
			};
		bool isException = false;
		try
		{
			testObj.setCompareNodePriority(compareNodePriority);
		}
		catch (const std::exception&)
		{
			isException = true;
		}
		EXPECT_EQ(isException, true);

		std::function<bool(const size_t&, const size_t&)> compareNodeEqual = [](const size_t& a, const size_t& b) {
			return a == b;
			};
		isException = false;
		try
		{
			testObj.setCompareNodeEqual(compareNodeEqual);
		}
		catch (const std::exception&)
		{
			isException = true;
		}
		EXPECT_EQ(isException, true);
	}
}