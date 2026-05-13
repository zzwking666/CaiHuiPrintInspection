#pragma once

#include"cla_utility.hpp"

#include"gtest/gtest.h"
#include"ActivationCodeModule/cla_ActivationCodeGenerator.hpp"

#include <random>
#include <string>

class ActivationBitsConvert_T
	: public ::testing::Test {
protected:
	void SetUp() override {
		generate();
	}
	void TearDown() override {
	}

private:
	std::string getRandom()
	{
		// 定义随机字符串的长度
		const size_t length = 16;

		// 定义字符集
		const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

		// 随机数生成器
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<size_t> distribution(0, charset.size() - 1);

		// 生成随机字符串
		std::string randomString;
		for (size_t i = 0; i < length; ++i)
		{
			randomString += charset[distribution(generator)];
		}

		return randomString;
	}
public:
	void generate()
	{
		rw::cla::ActivationCodeGenerator generator;
		testObj = generator.generateActivationBinaryCode(getRandom());
	}

public:
	rw::cla::ActivationString testObj;
};