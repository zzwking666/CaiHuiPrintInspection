#include"dsl_CacheClock_t.hpp"

using namespace rw::dsl;

namespace dsl_cache {
	TEST(CacheClockTest, BasicSetAndGet) {
		CacheClock<int, std::string> cache(3);

		// 测试插入和获取
		cache.set(1, "one");
		cache.set(2, "two");
		cache.set(3, "three");

		EXPECT_EQ(cache.get(1).value(), "one");
		EXPECT_EQ(cache.get(2).value(), "two");
		EXPECT_EQ(cache.get(3).value(), "three");
	}

	TEST(CacheClockTest, OverwriteValue) {
		CacheClock<int, std::string> cache(3);

		// 测试覆盖值
		cache.set(1, "one");
		cache.set(1, "uno");

		EXPECT_EQ(cache.get(1).value(), "uno");
	}

	TEST(CacheClockTest, EvictionPolicy) {
		CacheClock<int, std::string> cache(3);

		// 测试驱逐策略
		cache.set(1, "one");
		cache.set(2, "two");
		cache.set(3, "three");

		// 访问1和2，设置它们的引用位
		cache.get(1);
		cache.get(2);

		// 插入新值，应该驱逐1
		cache.set(4, "four");

		EXPECT_EQ(cache.get(1), std::nullopt);
		EXPECT_EQ(cache.get(2).value(), "two");
		EXPECT_EQ(cache.get(3).value(), "three");
		EXPECT_EQ(cache.get(4).value(), "four");
	}

	TEST(CacheClockTest, ResizeCache) {
		CacheClock<int, std::string> cache(3);

		// 测试调整缓存大小
		cache.set(1, "one");
		cache.set(2, "two");
		cache.set(3, "three");

		cache.resizeCapacity(2);

		EXPECT_EQ(cache.size(), 2);
		EXPECT_EQ(cache.get(1), std::nullopt);
		EXPECT_EQ(cache.get(2).value(), "two");
		EXPECT_EQ(cache.get(3).value(), "three");
	}

	TEST(CacheClockTest, ClearCache) {
		CacheClock<int, std::string> cache(3);

		// 测试清空缓存
		cache.set(1, "one");
		cache.set(2, "two");
		cache.set(3, "three");

		cache.clear();

		EXPECT_EQ(cache.size(), 0);
		EXPECT_EQ(cache.get(1), std::nullopt);
		EXPECT_EQ(cache.get(2), std::nullopt);
		EXPECT_EQ(cache.get(3), std::nullopt);
	}
}