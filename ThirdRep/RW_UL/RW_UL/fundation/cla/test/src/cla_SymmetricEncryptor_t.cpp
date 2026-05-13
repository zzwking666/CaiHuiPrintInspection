#include "cla_SymmetricEncryptor.hpp"
#include"cla_utility_t.hpp"

namespace cla_EncryptModule
{
	using namespace rw::cla;

	TEST(a, b)
	{
		// Generate key using KeyGenerator
		std::string generatedKey = KeyGenerator::generate32BitKey("testKeyInput");

		rw::cla::SymmetricEncryptorContext encryptorContext;
		encryptorContext.key = generatedKey;
		encryptorContext.sourceString = "Hello, World!";
		encryptorContext.bits = rw::cla::ActivationBits::Binary;
		rw::cla::SymmetricEncryptor symmetric(encryptorContext);
		symmetric.encrypt();

		auto encrpyStr = symmetric.context().encryptString;

		rw::cla::SymmetricEncryptorContext dEncryptorContextContext;
		dEncryptorContextContext.key = generatedKey;
		dEncryptorContextContext.encryptString = encrpyStr;
		dEncryptorContextContext.bits = rw::cla::ActivationBits::Binary;
		rw::cla::SymmetricEncryptor symmetric2(dEncryptorContextContext);
		symmetric2.decrypt();

		auto decryptStr = symmetric2.context().sourceString;
	}

	class SymmetricEncryptorTest : public ::testing::Test
	{
	protected:
		SymmetricEncryptorContext context;
		std::string testKey; // Key will be generated dynamically
		std::string testSourceString = "Hello, Symmetric Encryption!";
		std::string binaryString = "01010101010101010101010101010101"; // Example binary string

		void SetUp() override
		{
			// Generate key using KeyGenerator
			testKey = KeyGenerator::generate32BitKey("testKeyInput");
			context.key = testKey;
			context.sourceString = testSourceString;
		}
	};

	TEST_F(SymmetricEncryptorTest, EncryptDecryptBinary)
	{
		context.bits = ActivationBits::Binary;
		SymmetricEncryptor encryptor(context);

		// Encrypt
		encryptor.encrypt();
		std::string encryptedString = encryptor.context().encryptString;
		EXPECT_FALSE(encryptedString.empty()) << "Encrypted string should not be empty.";

		// Decrypt
		encryptor.decrypt();
		std::string decryptedString = encryptor.context().sourceString;
		EXPECT_EQ(decryptedString, testSourceString) << "Decrypted string should match the original source string.";
	}

	TEST_F(SymmetricEncryptorTest, EncryptDecryptHexadecimal)
	{
		context.bits = ActivationBits::Hexadecimal;
		SymmetricEncryptor encryptor(context);

		// Encrypt
		encryptor.encrypt();
		std::string encryptedString = encryptor.context().encryptString;
		EXPECT_FALSE(encryptedString.empty()) << "Encrypted string should not be empty.";

		// Check if encrypted string is valid hexadecimal
		for (char c : encryptedString)
		{
			EXPECT_TRUE(std::isxdigit(c)) << "Encrypted string should contain only hexadecimal characters.";
		}

		// Decrypt
		encryptor.decrypt();
		std::string decryptedString = encryptor.context().sourceString;
		EXPECT_EQ(decryptedString, testSourceString) << "Decrypted string should match the original source string.";
	}

	TEST_F(SymmetricEncryptorTest, EncryptDecryptOctal)
	{
		context.bits = ActivationBits::Octal;
		SymmetricEncryptor encryptor(context);

		// Encrypt
		encryptor.encrypt();
		std::string encryptedString = encryptor.context().encryptString;
		EXPECT_FALSE(encryptedString.empty()) << "Encrypted string should not be empty.";

		// Check if encrypted string is valid octal
		for (char c : encryptedString)
		{
			EXPECT_TRUE(c >= '0' && c <= '7') << "Encrypted string should contain only octal characters.";
		}

		// Decrypt
		encryptor.decrypt();
		std::string decryptedString = encryptor.context().sourceString;
		EXPECT_EQ(decryptedString, testSourceString) << "Decrypted string should match the original source string.";
	}

	TEST_F(SymmetricEncryptorTest, InvalidBitsType)
	{
		context.bits = static_cast<ActivationBits>(999); // Invalid ActivationBits type
		SymmetricEncryptor encryptor(context);

		// Encrypt should throw an exception
		EXPECT_THROW(encryptor.encrypt(), std::invalid_argument);

		// Decrypt should throw an exception
		EXPECT_THROW(encryptor.decrypt(), std::invalid_argument);
	}

	TEST_F(SymmetricEncryptorTest, EmptySourceString)
	{
		context.sourceString = "";
		context.bits = ActivationBits::Binary;
		SymmetricEncryptor encryptor(context);

		// Encrypt
		encryptor.encrypt();
		std::string encryptedString = encryptor.context().encryptString;
		EXPECT_FALSE(encryptedString.empty()) << "Encrypted string should not be empty.";

		// Decrypt
		encryptor.decrypt();
		std::string decryptedString = encryptor.context().sourceString;
		EXPECT_EQ(decryptedString, "") << "Decrypted string should match the original empty source string.";
	}

	TEST_F(SymmetricEncryptorTest, InvalidKeyLength)
	{
		// Generate an invalid key (shorter than 32 bytes)
		context.key = KeyGenerator::generate32BitKey("shortKeyInput").substr(0, 8); // Short key
		context.bits = ActivationBits::Binary;
		SymmetricEncryptor encryptor(context);

		// Encrypt should throw an exception
		EXPECT_THROW(encryptor.encrypt(), std::invalid_argument);

		// Decrypt should throw an exception
		EXPECT_THROW(encryptor.decrypt(), std::invalid_argument);
	}
}