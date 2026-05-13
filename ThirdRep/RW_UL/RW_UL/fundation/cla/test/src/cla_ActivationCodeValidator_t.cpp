#include"cla_ActivationCodeValidator_t.hpp"

#include "ActivationCodeModule/cla_ActivationCodeGenerator.hpp"

namespace cla_ActivationCodeModule
{
	TEST(ActivationCodeValidator, validateActivationCode)
	{
		rw::cla::ActivationCodeGenerator generator;
		auto code = generator.generateActivationCode("temp");
		rw::cla::ActivationCodeValidator validator;
		auto result = validator.validateActivationCode(code, "temp");
		ASSERT_EQ(result, true);
	}

	TEST(ActivationCodeValidator, validateActivationCodeWithConfig)
	{
		rw::cla::ActivationConfig config;
		config.insert("Test", "1");
		config.insert("Test1", "2");
		config.insert("Test2", "3");

		rw::cla::ActivationCodeGenerator generator(config);
		auto code = generator.generateActivationCode("temp");
		rw::cla::ActivationCodeValidator validator(config);
		auto result = validator.validateActivationCode(code, "temp");

		ASSERT_EQ(result, true);

		rw::cla::ActivationCodeValidator validatorError;
		auto resultError = validatorError.validateActivationCode(code, "temp");
		ASSERT_EQ(resultError, false);
	}
}