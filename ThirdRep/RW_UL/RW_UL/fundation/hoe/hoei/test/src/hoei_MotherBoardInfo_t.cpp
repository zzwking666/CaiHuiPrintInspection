#include"hoei_utilty_t.hpp"

#include"hoei_MotherBoardInfo.hpp"

namespace hoei_HardwareInfo
{
	TEST(MotherBoardInfo, getMotherBoardInfo)
	{
		rw::hoei::MotherBoardInfo info;
		info.getCurrentContextInfo();
		std::cout << "MotherboardUniqueID:" << info.UUID << std::endl;
	}
}