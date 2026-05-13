#pragma once

#include <string>

namespace rw
{
	namespace cla
	{
		enum class ActivationBits;
		using UniqueIdentifier = std::string;
		using ActivationString = std::string;

		enum class ActivationBits
		{
			Hexadecimal=16,
			Octal=8,
			Binary=2
		};

		struct ActivationCodeStruct
		{
			ActivationString str;
			ActivationBits type;
		};

		struct ActivationBitsConvert
		{
			static ActivationString switchBinaryTOHex(const ActivationString& str);
			static ActivationString switchHexTOBinary(const ActivationString& str, size_t size);
			static ActivationString switchHexTOBinary(const ActivationString& str);
			static ActivationString switchOctTOBinary(const ActivationString& str,size_t size);
			static ActivationString switchOctTOBinary(const ActivationString& str);
			static ActivationString switchBinaryTOOct(const ActivationString& str);
			static ActivationString switchOctTOHex(const ActivationString& str);
			static ActivationString switchHexTOOct(const ActivationString& str);
		}; 

	}
}
