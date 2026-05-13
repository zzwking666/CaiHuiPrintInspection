#include"cla_EncrpyUtilty.hpp"

#include <vector>
#include <openssl/sha.h>

namespace rw
{
	namespace cla
	{
		std::string rw::cla::KeyGenerator::generate32BitKey(const std::string& input)
		{
			// Step 1: Compute SHA-256 hash of the input string
			unsigned char hash[SHA256_DIGEST_LENGTH];
			SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

			// Step 2: Convert the hash to a hexadecimal string
			std::string keyHex;
			for (unsigned char byte : hash) {
				char hex[3];
				snprintf(hex, sizeof(hex), "%02x", byte);
				keyHex += hex;
			}

			// Step 3: Extract the first 32 characters
			return keyHex.substr(0, 32); // Return the first 32 characters
		}
	}
}