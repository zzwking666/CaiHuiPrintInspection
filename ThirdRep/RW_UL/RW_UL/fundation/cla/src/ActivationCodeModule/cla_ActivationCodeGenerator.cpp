#include"ActivationCodeModule/cla_ActivationCodeGenerator.hpp"

#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <vector>
#include <bitset>

namespace rw
{
	namespace cla
	{
		ActivationConfig ActivationCodeGenerator::getActivationConfig()
		{
			return _config;
		}

		void ActivationCodeGenerator::setActivationConfig(const ActivationConfig& config)
		{
			_config = config;
		}

		ActivationCodeGenerator::ActivationCodeGenerator(const ActivationConfig& config)
			:_config(config)
		{
		}

		ActivationString ActivationCodeGenerator::generateActivationBinaryCode(const UniqueIdentifier& indetifier)
		{
			// 序列化配置字符串
			auto configStr = ActivationConfig::serialize(_config);

			// 将 UniqueIdentifier 和 configStr 组合
			std::string combinedData = indetifier + configStr;

			// 使用 SHA256 生成哈希值
			unsigned char hash[SHA256_DIGEST_LENGTH];
			SHA256(reinterpret_cast<const unsigned char*>(combinedData.c_str()), combinedData.size(), hash);

			// 将哈希值转换为二进制字符串
			std::ostringstream binaryActivationCodeStream;
			for (unsigned char byte : hash)
			{
				// 使用 std::bitset 将每个字节转换为二进制字符串
				binaryActivationCodeStream << std::bitset<8>(byte);
			}

			// 返回生成的二进制激活码
			return binaryActivationCodeStream.str();
		}

		ActivationCodeStruct ActivationCodeGenerator::generateActivationCode(const UniqueIdentifier& indetifier,
			ActivationBits type)
		{
			auto binaryStr = generateActivationBinaryCode(indetifier);
			ActivationCodeStruct result;

			switch (type)
			{
			case ActivationBits::Binary:
				result.type = ActivationBits::Binary;
				result.str = binaryStr;
				break;
			case ActivationBits::Hexadecimal:
				result.type = ActivationBits::Hexadecimal;
				result.str = ActivationBitsConvert::switchBinaryTOHex(binaryStr);
				break;
			case ActivationBits::Octal:
				result.type = ActivationBits::Octal;
				result.str = ActivationBitsConvert::switchBinaryTOOct(binaryStr);
				break;
			}
			return result;
		}
	}
}