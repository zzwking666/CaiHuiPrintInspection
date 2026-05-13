#include"ActivationCodeModule/cla_ActivationCodeValidator.hpp"

#include"ActivationCodeModule/cla_ActivationCodeGenerator.hpp"

namespace rw
{
	namespace cla
	{
		ActivationConfig ActivationCodeValidator::getActivationConfig()
		{
			return _config;
		}

		void ActivationCodeValidator::setActivationConfig(const ActivationConfig& config)
		{
			_config = config;
		}

		ActivationCodeValidator::ActivationCodeValidator(const ActivationConfig& config)
			:_config(config)
		{
		}

		bool ActivationCodeValidator::validateActivationCode(const ActivationCodeStruct& code,
			const UniqueIdentifier& indetifier)
		{
			ActivationCodeGenerator generator(_config);
			auto validationCode = generator.generateActivationBinaryCode(indetifier);
			switch (code.type)
			{
			case ActivationBits::Hexadecimal:
				return ActivationBitsConvert::switchHexTOBinary(code.str) == validationCode;
			case ActivationBits::Octal:
				return ActivationBitsConvert::switchOctTOBinary(code.str, code.str.size()) == validationCode;
			case ActivationBits::Binary:
				return code.str == validationCode;
			}
		}
	}
}