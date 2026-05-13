#pragma once

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "cla_EncrpyUtilty.hpp"
#include "cla_utility.hpp"

namespace rw
{
	namespace cla
	{
		class SymmetricEncryptor
		{
		private:
			SymmetricEncryptorContext _context;

			// Helper function to convert binary to target ActivationBits
			std::string convertFromBinary(const std::string& binaryStr, ActivationBits bits) const;

			// Helper function to convert target ActivationBits to binary
			std::string convertToBinary(const std::string& str, ActivationBits bits) const;

			// Perform encryption using OpenSSL
			std::string encryptBinary(const std::string& plainText, const std::string& key) const;

			// Perform decryption using OpenSSL
			std::string decryptBinary(const std::string& cipherText, const std::string& key) const;

		public:
			explicit SymmetricEncryptor(SymmetricEncryptorContext context) : _context(std::move(context)) {}
			// Encrypt function
			void encrypt();
			// Decrypt function
			void decrypt();

			SymmetricEncryptorContext& context() { return _context; }
			const SymmetricEncryptorContext& context() const { return _context; }
		};
	}
}