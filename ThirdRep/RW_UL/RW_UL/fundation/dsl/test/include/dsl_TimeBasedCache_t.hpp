#pragma once

#include <random>

#include"gtest/gtest.h"
#include"dsl_TimeBasedCache.hpp"

class TimeBasedCache_T
	: public ::testing::Test {
protected:
	void SetUp() override {
		testObj = std::make_unique<rw::dsl::TimeBasedCache<double, double>>(50);
		insertSomeData();
	}
	void TearDown() override {
		testObj.reset();
	}

	void insertSomeData()
	{
		testObj->insert(120, 120);
		testObj->insert(121, 121);
		testObj->insert(122, 122);
		testObj->insert(123, 123);
		testObj->insert(124, 124);
		testObj->insert(125, 125);
		testObj->insert(126, 126);
		testObj->insert(127, 127);
		testObj->insert(128, 128);
	}
public:
	int generateRandomNumber(int min, int max) {
		std::random_device rd; // 用于生成种子
		std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器
		std::uniform_int_distribution<> dis(min, max); // 均匀分布
		return dis(gen);
	}
public:
	std::unique_ptr<rw::dsl::TimeBasedCache<double, double>> testObj = nullptr;
};