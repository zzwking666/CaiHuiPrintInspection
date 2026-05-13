#pragma once

#include"actCry_utility.hpp"
#include <chrono>
#include <string>
#include <cstddef>

namespace rw
{
	namespace actCry
	{

		using ActivationCode = std::string;

		struct ActivationInfoRegistryCfg
		{
			std::string name{ "actCry" };
			std::string keyPath{ "Software\\RW\\ActCry\\" };
			std::string valueName{ "ActivationInfo"};
			std::string generateCodeKey{"Activation"};
		};

		class ActivationInfo
		{
		public:
			std::string hwid;
			std::chrono::system_clock::time_point startTime{};
			std::chrono::system_clock::time_point endTime{};
		public:
			bool isValid(const std::string &hwid) const;
		public:
			static bool save(const ActivationInfo& info,const ActivationInfoRegistryCfg & cfg);
			static ActivationInfo load(const ActivationInfoRegistryCfg& cfg);
			static ActivationInfo load(const ActivationInfoRegistryCfg& cfg,bool & isOK);
		public:
			static ActivationCode generateActivationCode(const ActivationInfo& info,const std::string & key);
			static ActivationInfo parseActivationCode(const ActivationCode& code, const std::string& key,bool & isOk);
		};

		class ActivationInfoResult
		{
		public:
			explicit ActivationInfoResult(const ActivationInfo& info, const std::string& hwid);
		public:
			std::chrono::system_clock::time_point startTime{};
			std::chrono::system_clock::time_point endTime{};
			bool isValid{ false };

		};
	}

}