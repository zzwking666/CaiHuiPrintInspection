#include "actCry_ActivationInfo.hpp"

#include <iomanip>

#include"cla_StrEncryption.hpp"
#include"actCry_RegistryManager.hpp"


namespace rw
{
	namespace actCry
	{
		static cla::StrConfig ActivationInfoToStrConfig(const ActivationInfo& info)
		{
			cla::StrConfig result{};
			result.insert(info.hwid, info.hwid);
			result.insert("startTime", std::to_string(std::chrono::duration_cast<std::chrono::seconds>(info.startTime.time_since_epoch()).count()));
			result.insert("endTime", std::to_string(std::chrono::duration_cast<std::chrono::seconds>(info.endTime.time_since_epoch()).count()));
			return result;
		}


		bool ActivationInfo::isValid(const std::string& hwid) const
		{
			auto now = std::chrono::system_clock::now();
			if (hwid != this->hwid)
			{
				return false;
			}
			if (now < this->startTime || now > this->endTime)
			{
				return false;
			}
			return true;
		}

		bool ActivationInfo::save( const ActivationInfo& info,
		                           const ActivationInfoRegistryCfg& cfg)
		{
			RegistryManager regMgr;
			std::wstring keyPath = utf8_to_wstring(cfg.keyPath+cfg.name);
			std::wstring valueName = utf8_to_wstring(cfg.valueName);
			ActivationCode code = generateActivationCode(info, cfg.generateCodeKey);
			std::wstring wvalue = utf8_to_wstring(code);
			return regMgr.SetString(keyPath, valueName, wvalue);
		}

		ActivationInfo ActivationInfo::load(const ActivationInfoRegistryCfg& cfg)
		{
			bool isOK = false;
			return load(cfg, isOK);
		}

		ActivationInfo ActivationInfo::load(const ActivationInfoRegistryCfg& cfg, bool& isOK)
		{
			RegistryManager regMgr;
			std::wstring keyPath = utf8_to_wstring(cfg.keyPath + cfg.name);
			std::wstring valueName = utf8_to_wstring(cfg.valueName);
			std::wstring wvalue;
			ActivationInfo result{};
			isOK = false;
			if (!regMgr.GetString(keyPath, valueName, wvalue))
			{
				return result;
			}
			std::string code = rw::actCry::wstring_to_utf8(wvalue);
			result = parseActivationCode(code, cfg.generateCodeKey, isOK);
			if (!isOK)
			{
				return ActivationInfo{};
			}
			return result;
		}

		ActivationCode ActivationInfo::generateActivationCode(const ActivationInfo& info, const std::string& key)
		{
			cla::StrConfig config = ActivationInfoToStrConfig(info);
			return cla::StrEncryption::StrConfigToHex(config, key);
		}

		ActivationInfo ActivationInfo::parseActivationCode(const ActivationCode& code, const std::string& key, bool& isOk)
		{
			ActivationInfo result{};
			isOk = false;
			bool parseOk = false;
			cla::StrConfig config = cla::StrEncryption::StrConfigFromHex(code, key, parseOk);
			if (!parseOk)
			{
				return result;
			}
			result.hwid = config.deserialize(config).configs.begin()->first;
			try
			{
				auto startTimeStr = config.deserialize(config).configs.at("startTime");
				auto endTimeStr = config.deserialize(config).configs.at("endTime");
				auto startTimeSec = std::stoll(startTimeStr);
				auto endTimeSec = std::stoll(endTimeStr);
				result.startTime = std::chrono::system_clock::time_point{ std::chrono::seconds{startTimeSec} };
				result.endTime = std::chrono::system_clock::time_point{ std::chrono::seconds{endTimeSec} };
			}
			catch (const std::exception&)
			{
				return result;
			}
			isOk = true;
			return result;
		}

		ActivationInfoResult::ActivationInfoResult(const ActivationInfo& info, const std::string& hwid)
		{
			startTime = info.startTime;
			endTime = info.endTime;
			isValid = info.isValid(hwid);
		}
	}
}