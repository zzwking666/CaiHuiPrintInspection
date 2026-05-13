#include"cla_utility.hpp"

#include <iomanip>
#include <sstream>
#include <vector>
#include <bitset>

namespace rw
{
	namespace cla
	{
		ActivationString ActivationBitsConvert::switchBinaryTOHex(const ActivationString& str)
		{
			// 检查输入是否为有效的二进制字符串
			if (str.empty() || str.find_first_not_of("01") != std::string::npos)
			{
				throw std::invalid_argument("输入的字符串不是有效的二进制字符串");
			}

			// 将二进制字符串分割为每 8 位一组
			std::ostringstream hexStream;
			for (size_t i = 0; i < str.size(); i += 8)
			{
				// 提取 8 位二进制子串
				std::string byteStr = str.substr(i, 8);

				// 将二进制子串转换为整数
				unsigned char byte = static_cast<unsigned char>(std::bitset<8>(byteStr).to_ulong());

				// 将整数转换为十六进制字符串
				hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
			}

			// 返回十六进制字符串
			return hexStream.str();
		}

		ActivationString ActivationBitsConvert::switchHexTOBinary(const ActivationString& str, size_t size)
		{
			if (str.empty() || str.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
			{
				throw std::invalid_argument("输入的字符串不是有效的十六进制字符串");
			}

			std::ostringstream binaryStream;
			for (char hexChar : str)
			{
				unsigned char byte = static_cast<unsigned char>(std::stoi(std::string(1, hexChar), nullptr, 16));
				binaryStream << std::bitset<4>(byte);
			}
			std::string binaryStr = binaryStream.str();

			// 去掉前导多余的0，只保留size位
			if (binaryStr.length() > size) {
				binaryStr = binaryStr.substr(binaryStr.length() - size, size);
			}
			else if (binaryStr.length() < size) {
				// 如果不足size位，前面补0
				binaryStr = std::string(size - binaryStr.length(), '0') + binaryStr;
			}
			return binaryStr;
		}

		ActivationString ActivationBitsConvert::switchHexTOBinary(const ActivationString& str)
		{
			if (str.empty() || str.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
			{
				throw std::invalid_argument("输入的字符串不是有效的十六进制字符串");
			}

			std::ostringstream binaryStream;
			for (char hexChar : str)
			{
				unsigned char byte = static_cast<unsigned char>(std::stoi(std::string(1, hexChar), nullptr, 16));

				binaryStream << std::bitset<4>(byte);
			}

			return binaryStream.str();
		}

		ActivationString ActivationBitsConvert::switchOctTOBinary(const ActivationString& str, size_t size)
		{
			if (str.empty() || str.find_first_not_of("01234567") != std::string::npos)
				throw std::invalid_argument("输入的字符串不是有效的八进制字符串");

			std::ostringstream binaryStream;
			for (char octChar : str) {
				unsigned char byte = static_cast<unsigned char>(std::stoi(std::string(1, octChar), nullptr, 8));
				binaryStream << std::bitset<3>(byte);
			}
			std::string binaryStr = binaryStream.str();

			// 去掉前面的多余的0，只保留size位
			if (binaryStr.length() > size) {
				binaryStr = binaryStr.substr(binaryStr.length() - size, size);
			}
			else if (binaryStr.length() < size) {
				// 如果不足size位，前面补0
				binaryStr = std::string(size - binaryStr.length(), '0') + binaryStr;
			}
			return binaryStr;
		}

		ActivationString ActivationBitsConvert::switchOctTOBinary(const ActivationString& str)
		{
			// 检查输入是否为有效的八进制字符串
			if (str.empty() || str.find_first_not_of("01234567") != std::string::npos)
				throw std::invalid_argument("输入的字符串不是有效的八进制字符串");

			std::ostringstream binaryStream;
			for (char octChar : str) {
				// 将每个八进制字符转换为3位二进制
				unsigned char byte = static_cast<unsigned char>(std::stoi(std::string(1, octChar), nullptr, 8));
				binaryStream << std::bitset<3>(byte);
			}
			return binaryStream.str();
		}

		ActivationString ActivationBitsConvert::switchBinaryTOOct(const ActivationString& str)
		{
			if (str.empty() || str.find_first_not_of("01") != std::string::npos)
				throw std::invalid_argument("输入的字符串不是有效的二进制字符串");

			// 先补齐到3的倍数
			std::string bin = str;
			while (bin.size() % 3 != 0) {
				bin = "0" + bin;
			}

			std::ostringstream octStream;
			for (size_t i = 0; i < bin.size(); i += 3) {
				std::string bitStr = bin.substr(i, 3);
				unsigned char byte = static_cast<unsigned char>(std::bitset<3>(bitStr).to_ulong());
				octStream << static_cast<char>('0' + byte);
			}

			// 不去除前导0，直接返回
			return octStream.str();
		}

		ActivationString ActivationBitsConvert::switchOctTOHex(const ActivationString& str)
		{
			// 检查输入是否为有效的八进制字符串
			if (str.empty() || str.find_first_not_of("01234567") != std::string::npos)
			{
				throw std::invalid_argument("输入的字符串不是有效的八进制字符串");
			}

			// 八进制转二进制
			ActivationString binaryStr = switchOctTOBinary(str);
			// 记录原始长度
			size_t origLen = binaryStr.size();
			// 补齐到8的倍数
			if (binaryStr.size() % 8 != 0) {
				binaryStr = std::string(8 - (binaryStr.size() % 8), '0') + binaryStr;
			}
			// 二进制转十六进制
			ActivationString hexStr = switchBinaryTOHex(binaryStr);

			return hexStr;
		}

		ActivationString ActivationBitsConvert::switchHexTOOct(const ActivationString& str)
		{
			// 检查输入是否为有效的十六进制字符串
			if (str.empty() || str.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
			{
				throw std::invalid_argument("输入的字符串不是有效的十六进制字符串");
			}

			// 将十六进制字符串转换为二进制字符串
			ActivationString binaryStr = switchHexTOBinary(str);

			// 将二进制字符串转换为八进制字符串
			ActivationString octStr = switchBinaryTOOct(binaryStr);

			// 返回生成的八进制字符串
			return octStr;
		}
	}
}