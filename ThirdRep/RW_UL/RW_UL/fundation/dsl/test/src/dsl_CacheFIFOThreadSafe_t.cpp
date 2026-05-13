#include "dsl_CacheFIFOThreadSafe.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

namespace dsl_cache {
	TEST(CacheFIFOThreadSafe, BasicFunctionality) {
		using Key = int;
		using Value = std::string;

		rw::dsl::CacheFIFOThreadSafe<Key, Value> cache(3);

		// Test insertion
		EXPECT_TRUE(cache.set(1, "One"));
		EXPECT_TRUE(cache.set(2, "Two"));
		EXPECT_TRUE(cache.set(3, "Three"));

		// Test retrieval
		EXPECT_EQ(cache.get(1).value_or(""), "One");
		EXPECT_EQ(cache.get(2).value_or(""), "Two");
		EXPECT_EQ(cache.get(3).value_or(""), "Three");

		// Test eviction
		EXPECT_TRUE(cache.set(4, "Four"));
		EXPECT_EQ(cache.get(1), std::nullopt); // Oldest entry evicted
		EXPECT_EQ(cache.get(4).value_or(""), "Four");
	}

	TEST(CacheFIFOThreadSafe, ThreadSafety) {
		using Key = int;
		using Value = int;

		rw::dsl::CacheFIFOThreadSafe<Key, Value> cache(10);

		// Concurrent insertion
		std::thread t1([&]() {
			for (int i = 0; i < 5; ++i) {
				cache.set(i, i * 10);
			}
			});

		std::thread t2([&]() {
			for (int i = 5; i < 10; ++i) {
				cache.set(i, i * 10);
			}
			});

		t1.join();
		t2.join();

		// Validate size and data
		EXPECT_EQ(cache.size(), 10);
		for (int i = 0; i < 10; ++i) {
			EXPECT_EQ(cache.get(i).value_or(-1), i * 10);
		}
	}

	TEST(CacheFIFOThreadSafeChrono, BasicFunctionality) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFOThreadSafe<Key, Value> cache(3);

		auto now = std::chrono::system_clock::now();

		// Test insertion
		EXPECT_TRUE(cache.set(now, 10));
		EXPECT_TRUE(cache.set(now + std::chrono::seconds(1), 20));
		EXPECT_TRUE(cache.set(now + std::chrono::seconds(2), 30));

		// Test retrieval
		EXPECT_EQ(cache.get(now).value_or(-1), 10);
		EXPECT_EQ(cache.get(now + std::chrono::seconds(1)).value_or(-1), 20);
		EXPECT_EQ(cache.get(now + std::chrono::seconds(2)).value_or(-1), 30);

		// Test eviction
		EXPECT_TRUE(cache.set(now + std::chrono::seconds(3), 40));
		EXPECT_EQ(cache.get(now), std::nullopt); // Oldest entry evicted
		EXPECT_EQ(cache.get(now + std::chrono::seconds(3)).value_or(-1), 40);
	}
} // namespace dsl_cache