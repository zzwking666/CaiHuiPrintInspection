#include"actCry_ActivationCrypto_t.hpp"

#include"cla_ActivationCodeValidator.hpp"
#include"actCry_ActivationCrypto.hpp"

namespace actCryTest
{
	TEST(ActivationCryptoClass,a)
	{
		rw::actCry::ActivationCrypto ac;
		ac.context().productName = "jkasjd";
		ac.context().key = "asdkjasd";
		auto result = ac();
	}
}
