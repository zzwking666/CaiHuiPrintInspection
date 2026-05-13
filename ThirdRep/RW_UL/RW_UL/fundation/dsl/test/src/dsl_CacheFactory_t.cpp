#include"dsl_CacheFactory_t.hpp"

namespace dsl_cache {
	TEST_P(CacheFactoryTest, createCache) {
		auto& [policy] = GetParam();
		auto cache = rw::dsl::CacheFactory::createCache<int, int>(policy, 10);
		ASSERT_TRUE(cache);
	}

	TEST_P(CacheFactoryTest, SetAndGet) {
		auto& [policy] = GetParam();
		auto cache = rw::dsl::CacheFactory::createCache<int, int>(policy, 10);
		ASSERT_TRUE(cache);

		std::vector<int> keys = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		std::vector<int> values = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

		for (size_t i = 0; i < keys.size(); i++) {
			cache->set(keys[i], values[i]);
		}

		for (size_t i = 0; i < keys.size(); i++) {
			auto value = cache->get(keys[i]);
			ASSERT_EQ(value, values[i]);
		}
	}

	TEST_P(CacheFactoryTest, SetAndGetWithResizeCapacity) {
		auto& [policy] = GetParam();
		auto cache = rw::dsl::CacheFactory::createCache<int, int>(policy, 10);
		ASSERT_TRUE(cache);
		std::vector<int> keys = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		std::vector<int> values = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
		for (size_t i = 0; i < keys.size(); i++) {
			cache->set(keys[i], values[i]);
		}
		for (size_t i = 0; i < keys.size(); i++) {
			auto value = cache->get(keys[i]);
			ASSERT_EQ(value, values[i]);
		}
		cache->resizeCapacity(5);
		int count{ 0 };
		for (size_t i = 0; i < keys.size(); i++) {
			auto value = cache->get(keys[i]);
			if (value.has_value()) {
				ASSERT_EQ(value, values[i]);
			}
			else {
				count++;
			}
		}
		ASSERT_EQ(count, 5);
		ASSERT_EQ(cache->capacity(), 5);
	}

	TEST_P(CacheFactoryTest, SizeAndResizeCapacity) {
		auto& [policy] = GetParam();
		auto cache = rw::dsl::CacheFactory::createCache<int, int>(policy, 10);
		ASSERT_TRUE(cache);
		std::vector<int> keys = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		std::vector<int> values = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
		for (size_t i = 0; i < keys.size(); i++) {
			cache->set(keys[i], values[i]);
		}
		ASSERT_EQ(cache->size(), 10);

		cache->resizeCapacity(15);
		ASSERT_EQ(cache->capacity(), 15);
		ASSERT_EQ(cache->size(), 10);

		cache->resizeCapacity(5);
		ASSERT_EQ(cache->capacity(), 5);
		ASSERT_EQ(cache->size(), 5);
	}

	TEST_P(CacheFactoryTest, Clear) {
		auto& [policy] = GetParam();
		auto cache = rw::dsl::CacheFactory::createCache<int, int>(policy, 10);
		ASSERT_TRUE(cache);
		std::vector<int> keys = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		std::vector<int> values = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
		for (size_t i = 0; i < keys.size(); i++) {
			cache->set(keys[i], values[i]);
		}
		ASSERT_EQ(cache->size(), 10);
		ASSERT_EQ(cache->capacity(), 10);
		cache->clear();
		ASSERT_EQ(cache->size(), 0);
		ASSERT_EQ(cache->capacity(), 10);
	}
}