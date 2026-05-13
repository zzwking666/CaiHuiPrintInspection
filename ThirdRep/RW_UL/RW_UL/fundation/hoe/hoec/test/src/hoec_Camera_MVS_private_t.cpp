#include"hoec_Camera_MVS_private_t.h"
#include"hoec_CameraException.hpp"

#include"opencv2/opencv.hpp"

#include<iostream>
#include <chrono>
#include <future>

namespace  hoec_Camera_MVS
{
	TEST(hoec_Camera_MVS, sdk)
	{
		try
		{
			rw::hoec::Camera_MVS::initSDK();
		}
		catch (const CameraInitError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		try
		{
			rw::hoec::Camera_MVS::unInitSDK();
		}
		catch (const CameraInitError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		SUCCEED();
	}

	TEST(hoec_Camera_MVS, CheckIplist)
	{
		Camera_MVS::getCameraIpList();
	}

	TEST(heoc_Camera_MVS, PassiveActiveConstruct)
	{
		Camera_MVS_Active cameraActive;
		Camera_MVS_Passive cameraPassive;
	}

	TEST(heoc_Camera_MVS, PassiveActiveConnect)
	{
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		{
			Camera_MVS_Active cameraActive;
			cameraActive.setIP(ipList[0]);

			try
			{
				cameraActive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			auto cameraInfo = cameraActive.getCameraInfo();
			std::cout << cameraInfo.ip << '\n';
			std::cout << cameraInfo.name << '\n';
			std::cout << cameraInfo.mac << '\n';
		}

		{
			Camera_MVS_Passive cameraPassive;
			cameraPassive.setIP(ipList[0]);

			try
			{
				cameraPassive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			auto cameraInfo = cameraPassive.getCameraInfo();
			std::cout << cameraInfo.ip << '\n';
			std::cout << cameraInfo.name << '\n';
			std::cout << cameraInfo.mac << '\n';
		}

		SUCCEED();
	}

	TEST(heoc_Camera_MVS, PassiveActiveMonitor)
	{
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		{
			Camera_MVS_Active cameraActive;
			cameraActive.setIP(ipList[0]);

			try
			{
				cameraActive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraActive.startMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				cameraActive.stopMonitor();
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		{
			Camera_MVS_Passive cameraPassive;
			cameraPassive.setIP(ipList[0]);

			try
			{
				cameraPassive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraPassive.startMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				cameraPassive.stopMonitor();
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}

	TEST(heoc_Camera_MVS, PassiveActiveMonitorError)
	{
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		{
			Camera_MVS_Active cameraActive;
			cameraActive.setIP(ipList[0]);

			try
			{
				cameraActive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraActive.startMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				try
				{
					cameraActive.startMonitor();
					FAIL() << "Do not allow startMonitor twice";
				}
				catch (const CameraMonitorError& e)
				{
					SUCCEED();
				}
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		{
			Camera_MVS_Passive cameraPassive;
			cameraPassive.setIP(ipList[0]);

			try
			{
				cameraPassive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraPassive.startMonitor();
				try
				{
					cameraPassive.startMonitor();
					FAIL() << "Do not allow startMonitor twice";
				}
				catch (const CameraMonitorError& e)
				{
					SUCCEED();
				}
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}

	TEST(heoc_Camera_MVS, PassiveActiveSet)
	{
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';

		{
			Camera_MVS_Active cameraActive;
			cameraActive.setIP(ipList[0]);

			try
			{
				cameraActive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraActive.startMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraActive.setExposureTime(2000);
				cameraActive.setGain(10);
				cameraActive.setInTriggerLine(0);
				//cameraActive.setIOTime(2000); //实现还有问题
				cameraActive.setTriggerMode(CameraTriggerMode::SoftwareTriggered);
			}
			catch (const CameraSettingError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				auto exposureTime = cameraActive.getExposureTime();
				EXPECT_EQ(exposureTime, 2000);

				auto gain = cameraActive.getGain();
				EXPECT_EQ(gain, 10);

				/*auto ioTime = cameraActive.getIOTime();
				EXPECT_EQ(ioTime, 2000);*/

				auto triggerMode = cameraActive.getMonitorMode();
				EXPECT_EQ(triggerMode, CameraTriggerMode::SoftwareTriggered);

				auto triggerLine = cameraActive.getTriggerLine();
				EXPECT_EQ(triggerLine, 0);
			}
			catch (const CameraRetrievalError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraActive.stopMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		{
			Camera_MVS_Passive cameraPassive;
			cameraPassive.setIP(ipList[0]);

			try
			{
				cameraPassive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraPassive.startMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraPassive.setExposureTime(2000);
				cameraPassive.setGain(10);
				cameraPassive.setInTriggerLine(0);
				//cameraActive.setIOTime(2000); //实现还有问题
				cameraPassive.setTriggerMode(CameraTriggerMode::SoftwareTriggered);
			}
			catch (const CameraSettingError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				auto exposureTime = cameraPassive.getExposureTime();
				EXPECT_EQ(exposureTime, 2000);

				auto gain = cameraPassive.getGain();
				EXPECT_EQ(gain, 10);

				/*             auto ioTime = cameraPassive.getIOTime();
							 EXPECT_EQ(ioTime, 2000);*/

				auto triggerMode = cameraPassive.getMonitorMode();
				EXPECT_EQ(triggerMode, CameraTriggerMode::SoftwareTriggered);

				auto triggerLine = cameraPassive.getTriggerLine();
				EXPECT_EQ(triggerLine, 0);
			}
			catch (const CameraRetrievalError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			try
			{
				cameraPassive.stopMonitor();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			catch (const CameraMonitorError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}

	TEST(heoc_Camera_MVS, ActiveTakePicture)
	{
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		Camera_MVS_Active cameraActive;
		cameraActive.setIP(ipList[0]);

		try
		{
			cameraActive.connectCamera();
		}
		catch (const CameraConnectionError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		try
		{
			cameraActive.startMonitor();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		catch (const CameraMonitorError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		try
		{
			auto imageGet = cameraActive.getImage();
			auto size = imageGet.size();
			EXPECT_GT(size.width, 0);
			EXPECT_GT(size.height, 0);

			cv::waitKey(0);
		}
		catch (const CameraRetrievalError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		SUCCEED();
	}

	TEST(heoc_Camera_MVS, PassiveTakePicture)
	{
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		Camera_MVS_Passive cameraPassive([](cv::Mat mat)
			{
				std::cout << mat.size().width << '\n';
				std::cout << mat.size().height << '\n';
				std::cout << "We have gotten a image" << '\n';
			});
		cameraPassive.setIP(ipList[0]);

		try
		{
			cameraPassive.connectCamera();
		}
		catch (const CameraConnectionError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		try
		{
			cameraPassive.RegisterCallBackFunc();
			cameraPassive.startMonitor();
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		}
		catch (const CameraMonitorError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		SUCCEED();
	}

	TEST(hoec_Camera_MVS, PassiveStressTesting) {
		std::cout << "We will take stress test for passive camera if you want to test, please input y in 10s" << '\n';

		std::string c;
		auto future = std::async(std::launch::async, []() {
			std::string input;
			std::cin >> input;
			return input;
			});

		if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
			c = "n"; // 默认输入不是y
		}
		else {
			c = future.get();
		}

		if (c != "y") {
			SUCCEED();
			return;
		}

		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		Camera_MVS_Passive cameraPassive([](cv::Mat mat)
			{
				static int count = 1;
				std::cout << "We have gotten a image:" << count << '\n';
				count++;
			});
		cameraPassive.setIP(ipList[0]);

		try
		{
			cameraPassive.connectCamera();
		}
		catch (const CameraConnectionError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		try
		{
			cameraPassive.RegisterCallBackFunc();

			std::cout << "Please set triggerMode in 10s ,0 is software trigger ,1 is hardware trigger" << '\n';

			std::string c;
			auto future = std::async(std::launch::async, []() {
				std::string input;
				std::cin >> input;
				return input;
				});

			if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
				c = "0";
			}
			else {
				c = future.get();
			}

			if (c == "0") {
				cameraPassive.setTriggerMode(CameraTriggerMode::SoftwareTriggered);
			}
			else {
				cameraPassive.setTriggerMode(CameraTriggerMode::HardwareTriggered);
			}

			cameraPassive.startMonitor();
			std::this_thread::sleep_for(std::chrono::seconds(300));
		}
		catch (const CameraMonitorError& e)
		{
			std::cout << e.what() << '\n';
			FAIL();
		}

		SUCCEED();
	}

	TEST(hoec_Camera_MVS, setOutTriggerConfig) {
		auto ipList = Camera_MVS::getCameraIpList();
		if (ipList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		std::cout << "Will test camera by one" << '\n';
		{
			Camera_MVS_Active cameraActive;
			cameraActive.setIP(ipList[0]);

			try
			{
				cameraActive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			auto cameraInfo = cameraActive.getCameraInfo();
			std::cout << cameraInfo.ip << '\n';
			std::cout << cameraInfo.name << '\n';
			std::cout << cameraInfo.mac << '\n';
		}

		{
			Camera_MVS_Passive cameraPassive;
			cameraPassive.setIP(ipList[0]);

			try
			{
				cameraPassive.connectCamera();
			}
			catch (const CameraConnectionError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}

			auto cameraInfo = cameraPassive.getCameraInfo();
			std::cout << cameraInfo.ip << '\n';
			std::cout << cameraInfo.name << '\n';
			std::cout << cameraInfo.mac << '\n';

			OutTriggerConfig config;
			config.lineSelector = 2;
			config.lineMode = 8;
			config.lineSource = 5;
			config.durationValue = 1000 * 1000;
			config.delayValue = 0;
			config.preDelayValue = 0;
			config.strobeEnable = true;
			cameraPassive.setOutTriggerConfig(config);

			cameraPassive.outTrigger();
			cameraPassive.outTrigger(true);
		}

		SUCCEED();
	}
}