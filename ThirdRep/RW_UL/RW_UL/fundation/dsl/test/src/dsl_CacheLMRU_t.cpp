#include"dsl_CacheLMRU_t.hpp"

#include"dsl_pch_t.h"
namespace dsl_cache {
	TEST_F(CacheLRU_Test, ImplementProtocolTesting) {
		testObj->set(1, "one");
		testObj->set(2, "two");
		testObj->set(3, "three");
		testObj->set(4, "four");
		testObj->set(5, "five");
		testObj->set(6, "six");
		testObj->set(7, "seven");
		testObj->set(8, "eight");
		testObj->set(9, "nine");
		testObj->set(10, "ten");

		auto expectList = std::vector<std::pair<int, std::string>>{
			{10, "ten"},
			{9, "nine"},
			{8, "eight"},
			{7, "seven"},
			{6, "six"}
		};

		auto it = cacheList->begin();
		for (const auto& [key, value] : expectList) {
			ASSERT_EQ(key, it->first);
			ASSERT_EQ(value, it->second);
			++it;
		}

		ASSERT_EQ(5, testObj->size());
	}

	TEST_F(CacheLRU_Test_Api, SetAndGet) {
		auto insertNum = generateRandomNumber(50, 200);
		std::list<std::pair<int, std::string>> expectList;
		for (int i = 0; i < insertNum; ++i) {
			auto key = generateRandomNumber(0, 100);
			auto value = std::to_string(key);
			testObj->set(key, value);
			expectList.push_front({ key, value });
		}
		int checkNum = 0;
		for (const auto& item : expectList) {
			auto [key, value] = item;
			auto result = testObj->get(key);
			ASSERT_EQ(value, result);
			++checkNum;
			if (checkNum == capacity) {
				break;
			}
		}
	}

	TEST_F(CacheLRU_Test_Api, SetAndGetCapacity) {
		auto insertNum = generateRandomNumber(50, 200);
		std::list<std::pair<int, std::string>> expectList;
		for (int i = 0; i < insertNum; ++i) {
			auto key = i;
			auto value = std::to_string(key);
			testObj->set(key, value);
			expectList.push_front({ key, value });
		}
		int checkNum = 0;
		for (const auto& item : expectList) {
			auto [key, value] = item;
			auto result = testObj->get(key);
			ASSERT_EQ(value, result);
			++checkNum;
			if (checkNum == capacity) {
				break;
			}
		}
		ASSERT_EQ(capacity, testObj->capacity());
	}

	TEST_F(CacheLRU_Test_Api, ResizeCapacity) {
		auto insertNum = generateRandomNumber(50, 200);
		std::list<std::pair<int, std::string>> expectList;
		for (int i = 0; i < insertNum; ++i) {
			auto key = i;
			auto value = std::to_string(key);
			testObj->set(key, value);
			expectList.push_front({ key, value });
		}
		int checkNum = 0;
		for (const auto& item : expectList) {
			auto [key, value] = item;
			auto result = testObj->get(key);
			ASSERT_EQ(value, result);
			++checkNum;
			if (checkNum == capacity) {
				break;
			}
		}

		ASSERT_EQ(capacity, testObj->capacity());
		auto newCapacity = generateRandomNumber(200, 300);
		testObj->resizeCapacity(newCapacity);
		ASSERT_EQ(newCapacity, testObj->capacity());
		auto currentCapacity = testObj->capacity();
		if (currentCapacity >= capacity) {
			int willInsertNum = currentCapacity - capacity + generateRandomNumber(50, 200);
			for (int i = 0; i < willInsertNum; ++i) {
				auto key = i;
				auto value = std::to_string(key);
				testObj->set(key, value);
				expectList.push_front({ key, value });
			}
			checkNum = 0;
			for (const auto& item : expectList) {
				auto [key, value] = item;
				auto result = testObj->get(key);
				ASSERT_EQ(value, result);
				++checkNum;
				if (checkNum == currentCapacity) {
					break;
				}
			}
		}
		else {
			checkNum = 0;
			for (const auto& item : expectList) {
				auto [key, value] = item;
				auto result = testObj->get(key);
				ASSERT_EQ(value, result);
				++checkNum;
				if (checkNum == currentCapacity) {
					break;
				}
			}
		}
	}

	TEST_F(CacheLRU_Test_Api, getWhenItIsNotExist) {
		testObj->set(1, "one");
		testObj->set(2, "two");
		auto getResult = testObj->get(3);
		EXPECT_EQ(std::nullopt, getResult);
	}
}