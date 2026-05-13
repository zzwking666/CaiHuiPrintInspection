#include<iostream>

#include"actCry_ActivationCrypto.hpp"

int main()
{
	std::string key;
	std::string hwid;

	std::cout << "input key:" << std::endl;
	std::cin >> key;

	std::cout << "input hwid:" << std::endl;
	std::cin >> hwid;

	rw::actCry::ActivationInfo info;
	info.hwid = hwid;
	info.startTime = std::chrono::system_clock::now();
	info.endTime = info.startTime + std::chrono::hours(24 * 30);
	auto code = rw::actCry::ActivationInfo::generateActivationCode(info, key);
	std::cout << "activation code: " << code<<std::endl;

	return 0;
}