#include"actCry_HWID.hpp"

#include "cla_ActivationCodeGenerator.hpp"
#include "hoei_HardwareInfo.hpp"
#include "actCry_RegistryManager.hpp"

#include <windows.h>


namespace rw
{
	namespace actCry
	{
		std::string HWID::generate(const HWIDGenerateCfg& cfg)
		{
			auto hardwareInfo = hoei::HardwareInfoFactory::createHardwareInfo();
			auto HWID = hardwareInfo.motherBoard.UUID;

			cla::ActivationConfig config;
			config.insert("ProductName", cfg.productName);
			config.insert(HWID, HWID);

			rw::cla::ActivationCodeGenerator generator(config);
			auto code = generator.generateActivationCode(HWID);

			return code.str;
		}

		bool HWID::save(const std::string& hwid, const HWIDSaveRegistryCfg& cfg)
		{
			RegistryManager regMgr;
			std::wstring keyPath = utf8_to_wstring(cfg.keyPath+cfg.name);
			std::wstring valueName = utf8_to_wstring(cfg.valueName);
			std::wstring wvalue = utf8_to_wstring(hwid);
			return regMgr.SetString(keyPath, valueName, wvalue);
		}

		std::string HWID::load(const HWIDSaveRegistryCfg& cfg)
		{
			bool isOk = false;
			return load(cfg, isOk);
		}

		std::string HWID::load(const HWIDSaveRegistryCfg& cfg, bool& isOk)
		{
			RegistryManager regMgr;
			std::wstring keyPath = utf8_to_wstring(cfg.keyPath + cfg.name);
			std::wstring valueName = utf8_to_wstring(cfg.valueName);
			std::wstring wvalue;
			isOk = regMgr.GetString(keyPath, valueName, wvalue);
			if (!isOk)
			{
				return {};
			}
			return wstring_to_utf8(wvalue);
		}
	}
}
