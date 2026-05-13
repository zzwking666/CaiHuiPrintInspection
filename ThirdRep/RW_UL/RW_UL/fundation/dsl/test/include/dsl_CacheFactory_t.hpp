#pragma once

#include"dsl_pch_t.h"
#include"dsl_CacheFactory.hpp"

class CacheFactoryTest
	:public ::testing::TestWithParam<std::tuple<rw::dsl::CachePolicy>> {
	void SetUp()override {}
	void TearDown()override {}
};

INSTANTIATE_TEST_SUITE_P(
	CacheFactoryTests,
	CacheFactoryTest,
	::testing::Values(
		std::make_tuple(rw::dsl::CachePolicy::LRU),
		std::make_tuple(rw::dsl::CachePolicy::LFU),
		std::make_tuple(rw::dsl::CachePolicy::MRU),
		std::make_tuple(rw::dsl::CachePolicy::MFU),
		std::make_tuple(rw::dsl::CachePolicy::Clock),
		std::make_tuple(rw::dsl::CachePolicy::FIFO),
		std::make_tuple(rw::dsl::CachePolicy::LIFO)
	)
);