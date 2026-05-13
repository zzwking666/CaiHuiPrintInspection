#include"dsl_CacheFIFO_t.hpp"
#include "dsl_CacheFIFO.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>

namespace dsl_cache {
	TEST(CacheFIFOTimeSpecialization, GetMethod) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFO<Key, Value> cache(3);

		auto now = std::chrono::system_clock::now();
		cache.set(now, 42);

		EXPECT_EQ(cache.get(now).value_or(-1), 42);
		EXPECT_EQ(cache.get(now + std::chrono::seconds(1)), std::nullopt); // Non-existent key
	}

	TEST(CacheFIFOTimeSpecialization, SetMethod) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFO<Key, Value> cache(2);

		auto now = std::chrono::system_clock::now();
		auto later = now + std::chrono::seconds(1);

		EXPECT_TRUE(cache.set(now, 10));
		EXPECT_TRUE(cache.set(later, 20));
		EXPECT_EQ(cache.size(), 2);

		// Test eviction
		auto evenLater = now + std::chrono::seconds(2);
		EXPECT_TRUE(cache.set(evenLater, 30));
		EXPECT_EQ(cache.size(), 2);
		EXPECT_EQ(cache.get(now), std::nullopt); // Oldest entry evicted
	}

	TEST(CacheFIFOTimeSpecialization, SizeMethod) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFO<Key, Value> cache(3);

		auto now = std::chrono::system_clock::now();
		cache.set(now, 10);
		cache.set(now + std::chrono::seconds(1), 20);

		EXPECT_EQ(cache.size(), 2);
	}

	TEST(CacheFIFOTimeSpecialization, ClearMethod) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFO<Key, Value> cache(3);

		auto now = std::chrono::system_clock::now();
		cache.set(now, 10);
		cache.set(now + std::chrono::seconds(1), 20);

		cache.clear();
		EXPECT_EQ(cache.size(), 0);
		EXPECT_EQ(cache.get(now), std::nullopt);
	}

	TEST(CacheFIFOTimeSpecialization, ResizeCapacityMethod) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFO<Key, Value> cache(3);

		auto now = std::chrono::system_clock::now();
		auto later = now + std::chrono::seconds(1);
		auto evenLater = now + std::chrono::seconds(2);

		cache.set(now, 10);
		cache.set(later, 20);
		cache.set(evenLater, 30);

		EXPECT_TRUE(cache.resizeCapacity(2));
		EXPECT_EQ(cache.size(), 2);
		EXPECT_EQ(cache.get(now), std::nullopt); // Oldest entry evicted
		EXPECT_EQ(cache.get(later).value_or(-1), 20);
		EXPECT_EQ(cache.get(evenLater).value_or(-1), 30);
	}

	TEST(CacheFIFOTimeSpecialization, RapidInsertionsAndEvictions) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = int;

		rw::dsl::CacheFIFO<Key, Value> cache(5);

		auto now = std::chrono::system_clock::now();

		// Rapidly insert keys
		for (int i = 0; i < 10; ++i) {
			cache.set(now + std::chrono::seconds(i), i);
		}

		// Validate eviction
		EXPECT_EQ(cache.size(), 5);
		EXPECT_EQ(cache.get(now), std::nullopt); // Oldest entries evicted
		EXPECT_EQ(cache.get(now + std::chrono::seconds(5)).value_or(-1), 5);
		EXPECT_EQ(cache.get(now + std::chrono::seconds(9)).value_or(-1), 9);
	}

	TEST(CacheFIFOTimeSpecialization, MixedDataTypes) {
		using Key = std::chrono::time_point<std::chrono::system_clock>;
		using Value = std::string;

		rw::dsl::CacheFIFO<Key, Value> cache(3);

		auto now = std::chrono::system_clock::now();

		// Insert mixed data types
		cache.set(now, "First");
		cache.set(now + std::chrono::seconds(1), "Second");
		cache.set(now + std::chrono::seconds(2), "Third");

		// Validate data
		EXPECT_EQ(cache.get(now).value_or(""), "First");
		EXPECT_EQ(cache.get(now + std::chrono::seconds(1)).value_or(""), "Second");
		EXPECT_EQ(cache.get(now + std::chrono::seconds(2)).value_or(""), "Third");

		// Test eviction
		cache.set(now + std::chrono::seconds(3), "Fourth");
		EXPECT_EQ(cache.get(now), std::nullopt); // Oldest entry evicted
		EXPECT_EQ(cache.get(now + std::chrono::seconds(3)).value_or(""), "Fourth");
	}
} // namespace dsl_cache