#pragma once

#include <functional>

#include "actCry_ActivationInfo.hpp"
#include "actCry_utility.hpp"

#include"actCry_HWID.hpp"

namespace rw
{
	namespace actCry
	{
		class ActivationCrypto;

		using InputActivationCodeFunc = std::function<std::string(bool&, const std::string &)>;


		struct ActivationCryptoContext
		{
			friend ActivationCrypto;
		public:
			std::string productName;
			std::string key;
		private:
			std::string hwid;
			ActivationInfo activationInfo{};

		public:
			InputActivationCodeFunc inputActivationCodeFunc{[](bool& isOk,const std::string& hwid)
			{
				isOk = false;
				return "";
			}};
			std::string inputActivationCode{};
		};

		class ActivationCrypto
		{
		private:
			ActivationCryptoContext _context{};
		public:
			const ActivationCryptoContext& getContext() const
			{
				return _context;
			}

			ActivationCryptoContext& context()
			{
				return _context;
			}
		public:
			bool hwidVerify(const HWIDGenerateCfg & hwidCfg, const HWIDSaveRegistryCfg& cfg);
		private:
			bool hwidVerify();
			ActivationInfo checkActivationCodeValid();
			bool inputActivationCode();
			bool checkInputActivationCode();
		public:
			ActivationInfoResult operator()();

		};
	}
}


