#include"hoec_Camera_MVS_private_t.h"

#include "../../../hoec/include/hoec_Camera.hpp"

//namespace  hoec_Camera_MVS
//{
//	using namespace rw::hoec_v1;
//
//	TEST(hoec_Camera_MVS,CheckIpList)
//	{
//		auto cameraList= Camera_MVS::getCameraInfoList();
//
//		auto ipList=Camera_MVS::getCameraIpList();
//		for (const auto & item:ipList)
//		{
//			std::cout << item << std::endl;
//		}
//	}
//
//	TEST(hoec_Camera_MVS, IniSdk)
//	{
//		auto isIni = Camera_MVS::initSDK();
//		ASSERT_EQ(isIni, true);
//	}
//
//	TEST(hoec_Camera_MVS, desSdk)
//	{
//		auto isDesIni = Camera_MVS::unInitSDK();
//		ASSERT_EQ(isDesIni, true);
//	}
//
//	TEST(hoec_Camera_MVS, CameraMVSPassiveConstruct)
//	{
//
//		auto ipList = Camera_MVS::getCameraIpList();
//		if (ipList.empty())
//		{
//			return;
//		}
//
//		{
//			Camera_MVS_Passive passive;
//
//			ASSERT_EQ(Camera_MVS_Passive::_isIniSDK, true);
//
//			passive.setIP(ipList[0]);
//			auto connectResult = passive.connectCamera();
//			ASSERT_EQ(connectResult, true);
//
//			auto registerFuncResult = passive.RegisterCallBackFunc();
//			ASSERT_EQ(registerFuncResult, true);
//
//			auto startMonitorResult = passive.RegisterCallBackFunc();
//			ASSERT_EQ(startMonitorResult, true);
//
//		}
//
//		ASSERT_EQ(Camera_MVS_Passive::_isIniSDK, false);
//
//	}
//
//
//	TEST(hoec_Camera_MVS, CameraMVSActiveConstruct)
//	{
//
//		auto ipList = Camera_MVS::getCameraIpList();
//		if (ipList.empty())
//		{
//			return;
//		}
//
//		{
//			Camera_MVS_Active active;
//
//			ASSERT_EQ(Camera_MVS_Passive::_isIniSDK, true);
//
//			active.setIP(ipList[0]);
//			auto connectResult = active.connectCamera();
//			ASSERT_EQ(connectResult, true);
//
//			auto startMonitorResult = active.startMonitor();
//			ASSERT_EQ(startMonitorResult, true);
//
//			bool isget{false};
//			auto image=active.getImage(isget);
//			ASSERT_EQ(isget, true);
//
//		}
//
//		ASSERT_EQ(Camera_MVS_Passive::_isIniSDK, false);
//
//	}
//}