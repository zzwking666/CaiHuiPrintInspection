#include"hoec_Camera_DS_private_t.h"

#include <gtest/gtest.h>

#include"hoec_Camera_DS_v1_private.hpp"

namespace  hoec_Camera_DS
{
	using namespace rw::hoec_v1;

	TEST(hoec_Camera_MVS, CheckIpList)
	{
		auto ipList = Camera_DS::getCameraIpList();
		auto infoList = Camera_DS::getCameraInfoList();
		for (const auto& item : ipList)
		{
			std::cout << item << std::endl;
		}
	}

	TEST(hoec_Camera_MVS, CameraMVSPassiveMonitor)
	{
		auto ipList = Camera_DS::getCameraIpList();
		if (ipList.empty())
		{
			return;
		}

		{
			Camera_DS_Passive passive;

			passive.setIP(ipList[0]);
			auto connectResult = passive.connectCamera();
			ASSERT_EQ(connectResult, true);

			bool isGet{ false };
			auto isConnectResult = passive.getConnectState(isGet);
			ASSERT_EQ(isGet, true);
			ASSERT_EQ(isConnectResult, true);

			auto isStartMonitor = passive.startMonitor();
			ASSERT_EQ(isStartMonitor, true);

			/*passive.setIoOut(4, true);

			 passive.setIoOut(5,true);*/
			bool state = false;
			passive.getIOIn(6, state);

			ASSERT_EQ(isStartMonitor, true);

			auto isStopMonitor = passive.stopMonitor();
			ASSERT_EQ(isStartMonitor, true);
		}
	}

	TEST(hoec_Camera_MVS, CameraMVSPassiveSetExposureTime)
	{
		auto ipList = Camera_DS::getCameraIpList();
		if (ipList.empty())
		{
			return;
		}

		{
			Camera_DS_Passive passive;

			passive.setIP(ipList[0]);
			auto connectResult = passive.connectCamera();
			ASSERT_EQ(connectResult, true);

			bool isGet{ false };
			auto isConnectResult = passive.getConnectState(isGet);
			ASSERT_EQ(isGet, true);
			ASSERT_EQ(isConnectResult, true);

			auto isSet = passive.setExposureTime(100);
			ASSERT_EQ(isSet, true);
			bool isGetExposure{ false };
			auto exposureTime = passive.getExposureTime(isGetExposure);
			ASSERT_EQ(exposureTime, 100);
			ASSERT_EQ(isGetExposure, true);
		}
	}

	TEST(hoec_Camera_MVS, CameraMVSPassiveSetFrameTrigger)
	{
		auto ipList = Camera_DS::getCameraIpList();
		if (ipList.empty())
		{
			return;
		}

		{
			Camera_DS_Passive passive;

			passive.setIP(ipList[0]);
			auto connectResult = passive.connectCamera();
			ASSERT_EQ(connectResult, true);

			bool isGet{ false };
			auto isConnectResult = passive.getConnectState(isGet);
			ASSERT_EQ(isGet, true);
			ASSERT_EQ(isConnectResult, true);

			auto isSet = passive.setExposureTime(100);
			ASSERT_EQ(isSet, true);
			bool isGetExposure{ false };
			auto exposureTime = passive.getExposureTime(isGetExposure);
			ASSERT_EQ(exposureTime, 100);
			ASSERT_EQ(isGetExposure, true);

			auto state = passive.setFrameTriggered(true);

			bool isGetTrigger1{ false };
			auto isGetTrigger = passive.getFrameTriggered(isGetTrigger1);
			ASSERT_EQ(isGetTrigger, true);
			ASSERT_EQ(state, true);
		}
	}

	TEST(hoec_Camera_MVS, CameraMVSPassiveSetLineTrigger)
	{
		auto ipList = Camera_DS::getCameraIpList();
		if (ipList.empty())
		{
			return;
		}

		{
			Camera_DS_Passive passive([](MatInfo mat) {
				std::cout << mat.mat.size << std::endl;
				return;
				});

			passive.setIP(ipList[0]);
			auto connectResult = passive.connectCamera();
			ASSERT_EQ(connectResult, true);

			bool isGet{ false };
			auto isConnectResult = passive.getConnectState(isGet);
			ASSERT_EQ(isGet, true);
			ASSERT_EQ(isConnectResult, true);

			auto isSet = passive.setExposureTime(100);
			ASSERT_EQ(isSet, true);
			bool isGetExposure{ false };
			auto exposureTime = passive.getExposureTime(isGetExposure);
			ASSERT_EQ(exposureTime, 100);
			ASSERT_EQ(isGetExposure, true);

			auto state = passive.setLineTriggered(true);

			bool isGetTrigger1{ false };
			auto isGetTrigger = passive.getLineTriggered(isGetTrigger1);
			ASSERT_EQ(isGetTrigger, true);
			ASSERT_EQ(state, true);

			passive.setLineTriggered(false);
			passive.setFrameTriggered(false);
			auto res = passive.RegisterCallBackFunc();
			passive.startMonitor();

			std::this_thread::sleep_for(std::chrono::seconds(300));
		}
	}
}