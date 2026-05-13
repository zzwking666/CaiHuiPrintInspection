#pragma once

#include"cla_utility.hpp"
#include <string>

namespace rw {
	namespace cla {
		struct SymmetricEncryptorContext
		{
		public:
			ActivationBits bits{};
		public:
			std::string encryptString{ };
		public:
			std::string sourceString{};
		public:
			//注意这里的密钥最好是32位的字符串，AES-256需要32字节的密钥
			std::string key{};
		};

		class KeyGenerator {
		public:
			// Function to map any string to a 32-bit key
			static std::string generate32BitKey(const std::string& input);
		};
	}
}