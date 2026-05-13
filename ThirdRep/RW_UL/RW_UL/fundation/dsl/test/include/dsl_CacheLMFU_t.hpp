#pragma once

#include"dsl_pch_t.h"
#include"dsl_CacheLMFU.hpp"

int generateRandomNumber(int min, int max) {
	std::random_device rd; // 用于生成种子
	std::mt19937 gen(rd()); // Mersenne Twister 19937 生成器
	std::uniform_int_distribution<> dis(min, max); // 均匀分布
	return dis(gen);
}

class CacheLMFU_Api_Test : public testing::Test {
protected:
	void SetUp() override {
		testObj = new  rw::dsl::CacheLMFU<std::string, int, true>(5);
		_size = 5;
	}
	void TearDown() override {
		delete testObj;
	}
public:
	const bool  LFU = true;
	const bool  MFU = false;
	void refreshSize() {
		delete testObj;
		_size = generateRandomNumber(5, 20);
		testObj = new rw::dsl::CacheLMFU<std::string, int, true>(_size);
	}

	void changePolicy(bool isLFU) {
		delete testObj;
		if (isLFU) {
			testObj = new rw::dsl::CacheLMFU<std::string, int, true>(_size);
		}
		else {
			testObj = new rw::dsl::CacheLMFU<std::string, int, false>(_size);
		}
	}
private:
	size_t _size;
public:
	rw::dsl::ICache<std::string, int>* testObj;
};