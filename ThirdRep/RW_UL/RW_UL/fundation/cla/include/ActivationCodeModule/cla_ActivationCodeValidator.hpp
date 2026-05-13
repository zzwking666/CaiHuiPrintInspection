#pragma once
#include "cla_ActivationConfig.hpp"
#include "cla_utility.hpp"

namespace rw
{
	namespace cla
	{
		class ActivationCodeValidator
		{
		private:
			ActivationConfig _config;
		public:
			ActivationConfig getActivationConfig();
			void setActivationConfig(const ActivationConfig& config);
		public:
			explicit ActivationCodeValidator(const ActivationConfig& config);
			ActivationCodeValidator() = default;
			~ActivationCodeValidator() = default;
		public:
			bool validateActivationCode(const ActivationCodeStruct& code, const UniqueIdentifier& indetifier);
		};
	}
}
