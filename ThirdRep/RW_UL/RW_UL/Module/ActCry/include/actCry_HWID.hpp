#pragma once

#include"actCry_utility.hpp"

namespace rw
{
	namespace actCry
	{
		struct HWIDGenerateCfg
		{
			std::string productName;
		};

		struct HWIDSaveRegistryCfg
		{
			std::string name{"actCry"};
			std::string keyPath{"Software\\RW\\ActCry\\"};
			std::string valueName{"HWID"};
		};

		class HWID
		{
		public:
			static std::string	generate(const HWIDGenerateCfg& cfg);
			static bool save(const std::string & hwid,const HWIDSaveRegistryCfg & cfg);
			static std::string load(const HWIDSaveRegistryCfg& cfg);
			static std::string load(const HWIDSaveRegistryCfg& cfg,bool & isOk);

		};

	}

}
