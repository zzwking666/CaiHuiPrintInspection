#include"dsl_CacheLMFU_t.hpp"

#include<unordered_map>
#include<vector>

namespace dsl_cache {
	TEST_F(CacheLMFU_Api_Test, ifKeepSize) {
		testObj->set("a", 1);
		testObj->set("b", 2);
		testObj->set("c", 3);
		testObj->set("d", 4);
		testObj->set("e", 5);
		EXPECT_EQ(testObj->size(), 5);
		testObj->set("f", 6);
		EXPECT_EQ(testObj->size(), 5);
	}

	TEST_F(CacheLMFU_Api_Test, ifGet) {
		testObj->set("a", 1);
		testObj->set("b", 2);
		testObj->set("c", 3);
		testObj->set("d", 4);
		testObj->set("e", 5);
		EXPECT_EQ(testObj->get("a"), 1);
		EXPECT_EQ(testObj->get("b"), 2);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
		EXPECT_EQ(testObj->get("f"), std::nullopt);
	}

	TEST_F(CacheLMFU_Api_Test, ifSet) {
		testObj->set("a", 1);
		testObj->set("b", 2);
		testObj->set("c", 3);
		testObj->set("d", 4);
		testObj->set("e", 5);
		EXPECT_EQ(testObj->get("a"), 1);
		EXPECT_EQ(testObj->get("b"), 2);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
		testObj->set("a", 6);
		testObj->set("b", 7);
		testObj->set("c", 8);
		testObj->set("d", 9);
		testObj->set("e", 10);
		EXPECT_EQ(testObj->get("a"), 6);
		EXPECT_EQ(testObj->get("b"), 7);
		EXPECT_EQ(testObj->get("c"), 8);
		EXPECT_EQ(testObj->get("d"), 9);
		EXPECT_EQ(testObj->get("e"), 10);
	}

	TEST_F(CacheLMFU_Api_Test, ifResize) {
		testObj->set("a", 1);
		testObj->set("b", 2);
		testObj->set("c", 3);
		testObj->set("d", 4);
		testObj->set("e", 5);
		EXPECT_EQ(testObj->size(), 5);
		testObj->resizeCapacity(3);
		EXPECT_EQ(testObj->size(), 3);
		EXPECT_EQ(testObj->get("a"), std::nullopt);
		EXPECT_EQ(testObj->get("b"), std::nullopt);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
	}

	TEST_F(CacheLMFU_Api_Test, ifResize2) {
		testObj->set("a", 1);
		testObj->set("b", 2);
		testObj->set("c", 3);
		testObj->set("d", 4);
		testObj->set("e", 5);
		EXPECT_EQ(testObj->size(), 5);
		testObj->resizeCapacity(7);
		EXPECT_EQ(testObj->size(), 5);
		EXPECT_EQ(testObj->get("a"), 1);
		EXPECT_EQ(testObj->get("b"), 2);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
	}

	TEST_F(CacheLMFU_Api_Test, ifResize3) {
		testObj->set("a", 1);
		testObj->set("b", 2);
		testObj->set("c", 3);
		testObj->set("d", 4);
		testObj->set("e", 5);
		EXPECT_EQ(testObj->size(), 5);
		testObj->resizeCapacity(3);
		EXPECT_EQ(testObj->size(), 3);
		testObj->resizeCapacity(7);
		EXPECT_EQ(testObj->size(), 3);
		EXPECT_EQ(testObj->get("a"), std::nullopt);
		EXPECT_EQ(testObj->get("b"), std::nullopt);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
	}

	TEST_F(CacheLMFU_Api_Test, CacheReplacementRoleCountAndTime) {
		//4
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);

		//3
		testObj->set("b", 2);
		testObj->set("b", 2);
		testObj->set("b", 2);

		//2
		testObj->set("c", 3);
		testObj->set("c", 3);

		//1
		testObj->set("d", 4);

		//2
		testObj->set("e", 5);
		testObj->set("e", 5);

		//3
		testObj->set("f", 6);
		testObj->set("f", 6);
		testObj->set("f", 6);

		//4
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);

		EXPECT_EQ(testObj->get("a"), 1);
		EXPECT_EQ(testObj->get("b"), 2);
		EXPECT_EQ(testObj->get("c"), std::nullopt);
		EXPECT_EQ(testObj->get("d"), std::nullopt);
		EXPECT_EQ(testObj->get("e"), 5);
		EXPECT_EQ(testObj->get("f"), 6);
		EXPECT_EQ(testObj->get("g"), 7);
	}

	TEST_F(CacheLMFU_Api_Test, CacheReplacementRoleCountAndTime2) {
		//4
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);

		//3
		testObj->set("b", 2);
		testObj->set("b", 2);
		testObj->set("b", 2);

		//2
		testObj->set("c", 3);
		testObj->set("c", 3);

		//1
		testObj->set("d", 4);

		//2
		testObj->set("e", 5);
		testObj->set("e", 5);

		//3
		testObj->set("f", 6);
		testObj->set("f", 6);
		testObj->set("f", 6);

		//4
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);

		//5
		testObj->set("h", 8);
		testObj->set("h", 8);
		testObj->set("h", 8);
		testObj->set("h", 8);
		testObj->set("h", 8);

		EXPECT_EQ(testObj->get("a"), 1);
		EXPECT_EQ(testObj->get("b"), 2);
		EXPECT_EQ(testObj->get("c"), std::nullopt);
		EXPECT_EQ(testObj->get("d"), std::nullopt);
		EXPECT_EQ(testObj->get("e"), std::nullopt);
		EXPECT_EQ(testObj->get("f"), 6);
		EXPECT_EQ(testObj->get("g"), 7);
		EXPECT_EQ(testObj->get("h"), 8);
	}

	TEST_F(CacheLMFU_Api_Test, MFUCacheReplacementRoleCountAndTime) {
		changePolicy(MFU);
		//4
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);

		//3
		testObj->set("b", 2);
		testObj->set("b", 2);
		testObj->set("b", 2);

		//2
		testObj->set("c", 3);
		testObj->set("c", 3);

		//1
		testObj->set("d", 4);

		//2
		testObj->set("e", 5);
		testObj->set("e", 5);

		//3
		testObj->set("f", 6);
		testObj->set("f", 6);
		testObj->set("f", 6);

		//4
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);

		EXPECT_EQ(testObj->get("a"), std::nullopt);
		EXPECT_EQ(testObj->get("b"), std::nullopt);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
		EXPECT_EQ(testObj->get("f"), 6);
		EXPECT_EQ(testObj->get("g"), 7);
	}

	TEST_F(CacheLMFU_Api_Test, MFUCacheReplacementRoleCountAndTime2) {
		changePolicy(MFU);
		//4
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);
		testObj->set("a", 1);

		//3
		testObj->set("b", 2);
		testObj->set("b", 2);
		testObj->set("b", 2);

		//2
		testObj->set("c", 3);
		testObj->set("c", 3);

		//1
		testObj->set("d", 4);

		//2
		testObj->set("e", 5);
		testObj->set("e", 5);

		//3
		testObj->set("f", 6);
		testObj->set("f", 6);
		testObj->set("f", 6);

		//4
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);
		testObj->set("g", 7);

		//5
		testObj->set("h", 8);
		testObj->set("h", 8);
		testObj->set("h", 8);
		testObj->set("h", 8);
		testObj->set("h", 8);

		EXPECT_EQ(testObj->get("a"), std::nullopt);
		EXPECT_EQ(testObj->get("b"), std::nullopt);
		EXPECT_EQ(testObj->get("c"), 3);
		EXPECT_EQ(testObj->get("d"), 4);
		EXPECT_EQ(testObj->get("e"), 5);
		EXPECT_EQ(testObj->get("f"), 6);
		EXPECT_EQ(testObj->get("g"), std::nullopt);
		EXPECT_EQ(testObj->get("h"), 8);
	}
}