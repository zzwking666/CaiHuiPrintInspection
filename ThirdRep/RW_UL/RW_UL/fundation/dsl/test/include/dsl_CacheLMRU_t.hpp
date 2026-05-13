#pragma once

#include"dsl_pch_t.h"

#include"dsl_CacheLMRU.hpp"

class CacheLRU_Test : public ::testing::Test {
protected:
	void SetUp() override {
		testObj = new rw::dsl::CacheLMRU<int, std::string>(5);
		cacheList = &testObj->_list;
	}
	void TearDown() override {
		if (testObj) {
			delete testObj;
		}
	}
	rw::dsl::CacheLMRU<int, std::string>* testObj;
	std::list<std::pair<int, std::string>>* cacheList{ nullptr };
};

class CacheLRU_Test_Api :public ::testing::Test {
protected:
	void SetUp() override {
		capacity = generateRandomNumber(50, 200);
		testObj = new rw::dsl::CacheLMRU<int, std::string>(capacity);
	}
	void TearDown() override {
		if (testObj) {
			delete testObj;
		}
	};
	rw::dsl::ICache<int, std::string>* testObj;
public:
	int generateRandomNumber(int min, int max) {
		std::random_device rd; // 用于生成种子
		std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器
		std::uniform_int_distribution<> dis(min, max); // 均匀分布
		return dis(gen);
	}
public:
	const bool isLRU{ true };
	const bool isMRU{ false };
	void changePolicy(bool isLRU) {
		delete testObj;
		if (isLRU) {
			testObj = new rw::dsl::CacheLMRU<int, std::string, true>(capacity);
		}
		else {
			testObj = new rw::dsl::CacheLMRU<int, std::string, false>(capacity);
		}
	}
public:
	size_t capacity{ 0 };
};

class CacheMRU_Test_Api :public CacheLRU_Test_Api {
protected:
	void SetUp() override {
	}
	void TearDown() override {
	};
};