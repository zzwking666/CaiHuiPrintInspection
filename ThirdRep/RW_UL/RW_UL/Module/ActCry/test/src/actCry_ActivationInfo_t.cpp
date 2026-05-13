#include"actCry_ActivationInfo_t.hpp"

#include <gtest/gtest.h>




namespace ActivationInfo
{
	TEST(ActivationInfoAPI, activationInfoToHex)
	{
		rw::actCry::ActivationInfo info;
		info.hwid = "TEST-HWID-567";
		info.startTime = std::chrono::system_clock::from_time_t(1622505600); // 2021-06-01 00:00:00
		info.endTime = std::chrono::system_clock::from_time_t(1654041600);   // 2022-06-01 00:00:00
		auto hex = rw::actCry::ActivationInfo::generateActivationCode(info, "test");
		EXPECT_FALSE(hex.empty());

		rw::actCry::ActivationInfo temp;
		bool isOk{false};
		temp=rw::actCry::ActivationInfo::parseActivationCode(hex, "test", isOk);
		EXPECT_TRUE(isOk);
		EXPECT_EQ(temp.hwid, info.hwid);
		EXPECT_EQ(temp.startTime, info.startTime);
		EXPECT_EQ(temp.endTime, info.endTime);

	}

	TEST(ActivationInfoAPI, saveAndLoad)
	{
		rw::actCry::ActivationInfo info;
		info.hwid = "TEST-HWID-890";
		info.startTime = std::chrono::system_clock::from_time_t(1633046400); // 2021-10-01 00:00:00
		info.endTime = std::chrono::system_clock::from_time_t(1664582400);   // 2022-10-01 00:00:00
		rw::actCry::ActivationInfoRegistryCfg cfg;
		cfg.name = "ActCryTest";
		cfg.keyPath = "Software\\RW\\ActCry\\Test\\";
		cfg.valueName = "ActivationInfo";

		bool saveResult = rw::actCry::ActivationInfo::save( info, cfg);
		EXPECT_TRUE(saveResult);
		rw::actCry::ActivationInfo loadedInfo = rw::actCry::ActivationInfo::load(cfg);

		EXPECT_EQ(loadedInfo.hwid, info.hwid);
		EXPECT_EQ(loadedInfo.startTime, info.startTime);
		EXPECT_EQ(loadedInfo.endTime, info.endTime);

	}


}
