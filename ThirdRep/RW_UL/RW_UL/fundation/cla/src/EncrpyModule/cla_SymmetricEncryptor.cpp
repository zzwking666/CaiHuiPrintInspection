#include"EncrpyModule/cla_SymmetricEncryptor.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace rw
{
	namespace cla
	{
		// Helper function to convert binary to target ActivationBits
		std::string SymmetricEncryptor::convertFromBinary(const std::string& binaryStr, ActivationBits bits) const
		{
			switch (bits)
			{
			case ActivationBits::Hexadecimal:
				return ActivationBitsConvert::switchBinaryTOHex(binaryStr);
			case ActivationBits::Octal:
				return ActivationBitsConvert::switchBinaryTOOct(binaryStr);
			case ActivationBits::Binary:
				return binaryStr; // No conversion needed
			default:
				throw std::invalid_argument("Unsupported ActivationBits type.");
			}
		}

		// Helper function to convert target ActivationBits to binary
		std::string SymmetricEncryptor::convertToBinary(const std::string& str, ActivationBits bits) const
		{
			switch (bits)
			{
			case ActivationBits::Hexadecimal:
				return ActivationBitsConvert::switchHexTOBinary(str);
			case ActivationBits::Octal:
				return ActivationBitsConvert::switchOctTOBinary(str);
			case ActivationBits::Binary:
				return str; // No conversion needed
			default:
				throw std::invalid_argument("Unsupported ActivationBits type.");
			}
		}

		std::string SymmetricEncryptor::encryptBinary(const std::string& plainText, const std::string& key) const
		{
			if (key.size() != 32)
			{
				throw std::invalid_argument("Key length must be exactly 32 bytes for AES-256 encryption.");
			}

			EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
			if (!ctx)
				throw std::runtime_error("Failed to create OpenSSL cipher context.");

			std::vector<unsigned char> keyBytes(32, 0);
			std::memcpy(keyBytes.data(), key.data(), std::min(key.size(), keyBytes.size()));

			std::vector<unsigned char> iv(EVP_CIPHER_iv_length(EVP_aes_256_cbc()));
			if (!RAND_bytes(iv.data(), iv.size()))
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to generate random IV.");
			}

			if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, keyBytes.data(), iv.data()) != 1)
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to initialize encryption.");
			}

			std::vector<unsigned char> encrypted(plainText.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
			int encryptedLen = 0;
			if (EVP_EncryptUpdate(ctx, encrypted.data(), &encryptedLen,
				reinterpret_cast<const unsigned char*>(plainText.data()), plainText.size()) != 1)
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to encrypt data.");
			}

			int finalLen = 0;
			if (EVP_EncryptFinal_ex(ctx, encrypted.data() + encryptedLen, &finalLen) != 1)
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to finalize encryption.");
			}

			encryptedLen += finalLen;
			encrypted.resize(encryptedLen);

			EVP_CIPHER_CTX_free(ctx);

			encrypted.insert(encrypted.end(), iv.begin(), iv.end());

			// Convert encrypted data to binary string (0 and 1 array)
			std::string binaryStr;
			for (unsigned char byte : encrypted)
			{
				for (int i = 7; i >= 0; --i)
				{
					binaryStr += ((byte >> i) & 1) ? '1' : '0';
				}
			}

			return binaryStr;
		}

		std::string SymmetricEncryptor::decryptBinary(const std::string& binaryStr, const std::string& key) const
		{
			// Check if the binary string is valid
			if (binaryStr.empty() || binaryStr.size() % 8 != 0 || binaryStr.find_first_not_of("01") != std::string::npos)
			{
				throw std::invalid_argument("Invalid binary string. It must be non-empty, a multiple of 8, and contain only '0' or '1'.");
			}

			// Check if the key length is exactly 32 bytes
			if (key.size() != 32)
			{
				throw std::invalid_argument("Key length must be exactly 32 bytes for AES-256 decryption.");
			}

			// Convert binary string (0 and 1 array) back to bytes
			std::vector<unsigned char> encrypted;
			for (size_t i = 0; i < binaryStr.size(); i += 8)
			{
				unsigned char byte = 0;
				for (int j = 0; j < 8; ++j)
				{
					byte = (byte << 1) | (binaryStr[i + j] - '0');
				}
				encrypted.push_back(byte);
			}

			EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
			if (!ctx)
				throw std::runtime_error("Failed to create OpenSSL cipher context.");

			std::vector<unsigned char> keyBytes(32, 0);
			std::memcpy(keyBytes.data(), key.data(), std::min(key.size(), keyBytes.size()));

			std::vector<unsigned char> iv(EVP_CIPHER_iv_length(EVP_aes_256_cbc()));
			if (encrypted.size() < iv.size())
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Invalid encrypted data.");
			}

			std::memcpy(iv.data(), encrypted.data() + encrypted.size() - iv.size(), iv.size());

			if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, keyBytes.data(), iv.data()) != 1)
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to initialize decryption.");
			}

			std::vector<unsigned char> decrypted(encrypted.size());
			int decryptedLen = 0;
			if (EVP_DecryptUpdate(ctx, decrypted.data(), &decryptedLen,
				encrypted.data(), encrypted.size() - iv.size()) != 1)
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to decrypt data.");
			}

			int finalLen = 0;
			if (EVP_DecryptFinal_ex(ctx, decrypted.data() + decryptedLen, &finalLen) != 1)
			{
				EVP_CIPHER_CTX_free(ctx);
				throw std::runtime_error("Failed to finalize decryption.");
			}

			decryptedLen += finalLen;
			decrypted.resize(decryptedLen);

			EVP_CIPHER_CTX_free(ctx);

			return std::string(decrypted.begin(), decrypted.end());
		}

		void SymmetricEncryptor::encrypt()
		{
			std::string binaryStr = encryptBinary(_context.sourceString, _context.key);
			_context.encryptString = convertFromBinary(binaryStr, _context.bits);
		}

		void SymmetricEncryptor::decrypt()
		{
			std::string binaryStr = convertToBinary(_context.encryptString, _context.bits);
			_context.sourceString = decryptBinary(binaryStr, _context.key);
		}
	}
}