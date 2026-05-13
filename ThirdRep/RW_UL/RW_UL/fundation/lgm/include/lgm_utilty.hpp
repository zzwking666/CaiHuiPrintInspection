#pragma once
#include <string>

namespace rw
{
	namespace lgm
	{
		struct RW_UL_DefaultLoggerName
		{
			static std::string imgProName;
			
		};

		inline std::string RW_UL_LogsPath = "./logs/RW_UL";
		inline std::string RW_UL_Module_logsPath = RW_UL_LogsPath + "/modules";
		inline std::string RW_UL_LogsFileExt = ".log";
	}

}
