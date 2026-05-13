#include <gtest/gtest.h>
#include "EncrpyModule/cla_EncrpyUtilty.hpp"

namespace rw {
	namespace cla {
		class KeyGeneratorTest : public ::testing::Test {
		protected:
			void validateKeyLength(const std::string& key) {
				EXPECT_EQ(key.size(), 32) << "Generated key should be 8 characters long.";
			}
		};

		TEST_F(KeyGeneratorTest, GenerateKeyFromEmptyString) {
			std::string input = "";
			std::string key = KeyGenerator::generate32BitKey(input);
			validateKeyLength(key);
			EXPECT_FALSE(key.empty()) << "Generated key should not be empty.";
		}

		TEST_F(KeyGeneratorTest, GenerateKeyFromShortString) {
			std::string input = "abc";
			std::string key = KeyGenerator::generate32BitKey(input);
			validateKeyLength(key);
			EXPECT_FALSE(key.empty()) << "Generated key should not be empty.";
		}

		TEST_F(KeyGeneratorTest, GenerateKeyFromLongString) {
			std::string input = "This is a very long string used for testing the key generation algorithm.";
			std::string key = KeyGenerator::generate32BitKey(input);
			validateKeyLength(key);
			EXPECT_FALSE(key.empty()) << "Generated key should not be empty.";
		}

		TEST_F(KeyGeneratorTest, GenerateKeyFromSpecialCharacters) {
			std::string input = "!@#$%^&*()_+-=[]{}|;':\",.<>?/`~";
			std::string key = KeyGenerator::generate32BitKey(input);
			validateKeyLength(key);
			EXPECT_FALSE(key.empty()) << "Generated key should not be empty.";
		}

		TEST_F(KeyGeneratorTest, GenerateKeyFromUnicodeString) {
			std::string input = "你好，世界！";
			std::string key = KeyGenerator::generate32BitKey(input);
			validateKeyLength(key);
			EXPECT_FALSE(key.empty()) << "Generated key should not be empty.";
		}

		TEST_F(KeyGeneratorTest, ConsistentKeyGeneration) {
			std::string input = "consistent";
			std::string key1 = KeyGenerator::generate32BitKey(input);
			std::string key2 = KeyGenerator::generate32BitKey(input);
			EXPECT_EQ(key1, key2) << "Generated keys for the same input should be consistent.";
		}

		TEST_F(KeyGeneratorTest, DifferentKeysForDifferentInputs) {
			std::string input1 = "input1";
			std::string input2 = "input2";
			std::string key1 = KeyGenerator::generate32BitKey(input1);
			std::string key2 = KeyGenerator::generate32BitKey(input2);
			EXPECT_NE(key1, key2) << "Generated keys for different inputs should be different.";
		}
	}
}