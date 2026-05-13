#include"actCry_ActivationCrypto.hpp"

#include<stdexcept>

#include "actCry_ActivationInfo.hpp"

namespace rw
{
	namespace actCry
	{

		bool ActivationCrypto::hwidVerify(const HWIDGenerateCfg& hwidCfg, const HWIDSaveRegistryCfg& cfg)
		{
			if (_context.productName.empty())
			{
				throw std::runtime_error("ActivationCrypto::hwidVerify: productName is empty");
			}

			auto hwid=HWID::load(cfg);
			if (hwid.empty())
			{
				hwid = HWID::generate(hwidCfg);
				auto saveResult=HWID::save(hwid, cfg);
				if (!saveResult)
				{
					return false;
				}
			}

			// verify HWID is changed
			auto currentHwid = HWID::generate(hwidCfg);
			if (hwid != currentHwid)
			{
				return false;
			}

			_context.hwid = hwid;
			return true;
		}

		bool ActivationCrypto::hwidVerify()
		{
			HWIDGenerateCfg hwidCfg;
			hwidCfg.productName = _context.productName;
			HWIDSaveRegistryCfg saveCfg;
			saveCfg.name = _context.productName;
			auto hwidVerifyResult = hwidVerify(hwidCfg, saveCfg);
			return hwidVerifyResult;
		}

		ActivationInfo ActivationCrypto::checkActivationCodeValid()
		{
			if (_context.key.empty())
			{
				throw std::runtime_error("ActivationCrypto::checkActivationCodeValid: key is empty");
			}
			ActivationInfoRegistryCfg cfg;
			cfg.name = _context.productName;
			cfg.generateCodeKey = _context.key;
			bool isOk{ false };
			auto activationInfo = ActivationInfo::load(cfg, isOk);
			if (isOk)
			{
				return activationInfo;
			}
			return ActivationInfo();
		}

		bool ActivationCrypto::inputActivationCode()
		{
			bool result{true};
			if (_context.inputActivationCodeFunc)
			{
				_context.inputActivationCode = _context.inputActivationCodeFunc(result,_context.hwid);

			}
			return result;
		}

		bool ActivationCrypto::checkInputActivationCode()
		{
			bool isParseOk{ false };
			auto info = ActivationInfo::parseActivationCode(_context.inputActivationCode, _context.key, isParseOk);
			if (!isParseOk)
			{
				return false;
			}

			auto isValid = info.isValid(_context.hwid);
			if (isValid)
			{
				ActivationInfoRegistryCfg cfg;
				cfg.name = _context.productName;
				cfg.generateCodeKey = _context.key;
				ActivationInfo::save(info, cfg);
				_context.activationInfo = info;
				return true;
			}
			else
			{

				return false;
			}

			
		}

		ActivationInfoResult ActivationCrypto::operator()()
		{
			ActivationInfo errrorResult;
			ActivationInfoResult errrorResultObj(errrorResult, _context.hwid);
			auto hwidVerifyResult = hwidVerify();
			if (!hwidVerifyResult)
			{
				return errrorResultObj;
			}

			auto activationInfo = checkActivationCodeValid();

			if (!activationInfo.isValid(_context.hwid))
			{
				auto inputActivationCodeResult = inputActivationCode();
				if (!inputActivationCodeResult)
				{
					return errrorResultObj;
				}
				else
				{
					auto checkInput = checkInputActivationCode();

					if (checkInput)
					{
						return ActivationInfoResult(_context.activationInfo, _context.hwid);
					}
					else
					{
						return errrorResultObj;
					}
				}
			}

			return ActivationInfoResult(activationInfo, _context.hwid);
		}
	}
}
