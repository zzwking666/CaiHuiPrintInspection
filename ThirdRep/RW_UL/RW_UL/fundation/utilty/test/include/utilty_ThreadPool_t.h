#pragma once

#include"pch.h"

#include"utilty_ThreadPool.hpp"

#include<random>

class ThreadPoolTest
	: public ::testing::Test {
protected:
	void SetUp() override {
		expensiveOperateNum = 0;
		cheapOperateNum = 0;
		totalExpensiveOperateTime = std::chrono::seconds(0);
		totalCheapOperateTime = std::chrono::microseconds(0);
	}

	void TearDown() override {
	}
public:
	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> distS{ 1, 5 };
	std::uniform_int_distribution<> distMs{ 20, 100 };
protected:
	std::mutex mtxExpensive;
	std::mutex mtxCheap;
	std::mutex mtxCout;
	std::atomic<size_t> expensiveOperateNum;
	std::atomic<size_t> cheapOperateNum;
	std::chrono::seconds totalExpensiveOperateTime;
	std::chrono::microseconds totalCheapOperateTime;
public:
	bool expensiveOperate(std::chrono::seconds time);
	bool cheapOperate(std::chrono::microseconds time);
};
