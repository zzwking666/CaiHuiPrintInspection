#include"cla_ActivationCodeGenerator_t.hpp"

namespace cla_ActivationCodeModule
{
	TEST(ActivationCodeGenerator, construct)
	{
		rw::cla::ActivationConfig config;
		config.insert("Zipper", "010");
		config.insert("author", "zyf");
		rw::cla::ActivationCodeGenerator generator(config);
		std::cout << rw::cla::ActivationBitsConvert::switchBinaryTOHex(generator.generateActivationBinaryCode("123456"));
	}
}