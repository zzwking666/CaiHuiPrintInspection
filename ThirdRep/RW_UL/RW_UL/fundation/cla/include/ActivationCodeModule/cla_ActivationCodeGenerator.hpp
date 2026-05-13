#pragma once

#include "cla_ActivationConfig.hpp"
#include "cla_utility.hpp"

namespace rw
{
	namespace cla
	{
		class ActivationCodeGenerator
		{
		private:
			ActivationConfig _config;
		public:
			ActivationConfig getActivationConfig();
			void setActivationConfig(const ActivationConfig& config);
		public:
			explicit ActivationCodeGenerator(const ActivationConfig& config);
			ActivationCodeGenerator()=default;
			~ActivationCodeGenerator()=default;
		public:
			ActivationString generateActivationBinaryCode(const UniqueIdentifier & indetifier);

		public:
			ActivationCodeStruct generateActivationCode(const UniqueIdentifier& indetifier,ActivationBits type= ActivationBits::Hexadecimal);
		};
	}
}
