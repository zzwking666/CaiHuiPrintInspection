#include"hoec_CameraFactory_t.hpp"

#include"opencv2/opencv.hpp"

namespace  hoec_CameraFactory
{
	TEST(hoec_CameraFactoryClass, CheckIplist)
	{
		CameraFactory::checkAllCamera();
	}

	TEST(hoec_CameraFactoryClass, PassiveActiveConstruct)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;

		std::cout << "Will test camera by one" << '\n';
		{
			try
			{
				auto cameraActive = CameraFactory::CreateActiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered);
				auto cameraInfo = cameraActive->getCameraInfo();
				std::cout << cameraInfo.ip << '\n';
				std::cout << cameraInfo.name << '\n';
				std::cout << cameraInfo.mac << '\n';
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		{
			try
			{
				auto cameraPassive = CameraFactory::CreatePassiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered, [](cv::Mat mat)
					{
						std::cout << mat.size().width << '\n';
						std::cout << mat.size().height << '\n';
						std::cout << "We have gotten a image" << '\n';
					});
				auto cameraInfo = cameraPassive->getCameraInfo();
				std::cout << cameraInfo.ip << '\n';
				std::cout << cameraInfo.name << '\n';
				std::cout << cameraInfo.mac << '\n';
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}

	TEST(hoec_CameraFactoryClass, PassiveActiveMonitor)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;

		std::cout << "Will test camera by one" << '\n';
		{
			try
			{
				auto cameraActive = CameraFactory::CreateActiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered);

				try
				{
					cameraActive->startMonitor();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					cameraActive->stopMonitor();
				}
				catch (const CameraMonitorError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		{
			try
			{
				auto cameraPassive = CameraFactory::CreatePassiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered, [](cv::Mat mat)
					{
						std::cout << mat.size().width << '\n';
						std::cout << mat.size().height << '\n';
						std::cout << "We have gotten a image" << '\n';
					});

				try
				{
					cameraPassive->startMonitor();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					cameraPassive->stopMonitor();
				}
				catch (const CameraMonitorError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}

	TEST(hoec_CameraFactoryClass, PassiveActiveMonitorError)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;

		std::cout << "Will test camera by one" << '\n';
		{
			try
			{
				auto cameraActive = CameraFactory::CreateActiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered);

				try
				{
					cameraActive->startMonitor();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));

					try
					{
						cameraActive->startMonitor();
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
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		{
			try
			{
				auto cameraPassive = CameraFactory::CreatePassiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered, [](cv::Mat mat)
					{
						std::cout << mat.size().width << '\n';
						std::cout << mat.size().height << '\n';
						std::cout << "We have gotten a image" << '\n';
					});

				try
				{
					cameraPassive->startMonitor();
					try
					{
						cameraPassive->startMonitor();
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
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}

	TEST(hoec_CameraFactoryClass, PassiveActiveSetGet)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}
		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;
		std::cout << "Will test camera by one" << '\n';
		{
			try
			{
				auto cameraActive = CameraFactory::CreateActiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered);
				try
				{
					cameraActive->setExposureTime(1000);
					cameraActive->setGain(10);
					//cameraActive->setIOTime(1000);
					cameraActive->setInTriggerLine(0);
					cameraActive->setTriggerMode(CameraTriggerMode::SoftwareTriggered);
				}
				catch (const CameraSettingError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}

				try
				{
					auto exposureTime = cameraActive->getExposureTime();
					EXPECT_EQ(exposureTime, 1000);

					auto gain = cameraActive->getGain();
					EXPECT_EQ(gain, 10);

					/*auto ioTime = cameraActive.getIOTime();
					EXPECT_EQ(ioTime, 2000);*/

					auto triggerMode = cameraActive->getMonitorMode();
					EXPECT_EQ(triggerMode, CameraTriggerMode::SoftwareTriggered);

					auto triggerLine = cameraActive->getTriggerLine();
					EXPECT_EQ(triggerLine, 0);
				}
				catch (const CameraRetrievalError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}
		{
			try
			{
				auto cameraPassive = CameraFactory::CreatePassiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered, [](cv::Mat mat)
					{
						std::cout << mat.size().width << '\n';
						std::cout << mat.size().height << '\n';
						std::cout << "We have gotten a image" << '\n';
					});
				try
				{
					cameraPassive->setExposureTime(1000);
					cameraPassive->setGain(10);
					//cameraPassive->setIOTime(1000);
					cameraPassive->setInTriggerLine(0);
					cameraPassive->setTriggerMode(CameraTriggerMode::SoftwareTriggered);
				}
				catch (const CameraSettingError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}

				try
				{
					auto exposureTime = cameraPassive->getExposureTime();
					EXPECT_EQ(exposureTime, 1000);

					auto gain = cameraPassive->getGain();
					EXPECT_EQ(gain, 10);

					/*auto ioTime = cameraActive.getIOTime();
					EXPECT_EQ(ioTime, 2000);*/

					auto triggerMode = cameraPassive->getMonitorMode();
					EXPECT_EQ(triggerMode, CameraTriggerMode::SoftwareTriggered);

					auto triggerLine = cameraPassive->getTriggerLine();
					EXPECT_EQ(triggerLine, 0);
				}
				catch (const CameraRetrievalError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}
		SUCCEED();
	}

	TEST(hoec_CameraFactoryClass, ActiveTakePictureBool)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}
		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;
		std::cout << "Will test camera by one" << '\n';
		{
			try
			{
				auto cameraActive = CameraFactory::CreateActiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered);
				cameraActive->startMonitor();
				try
				{
					bool isGet = false;
					auto image = cameraActive->getImage(isGet);
					if (!isGet)
					{
						std::cout << "Failed to get image" << '\n';
						FAIL();
					}
				}
				catch (const CameraRetrievalError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}
		SUCCEED();
	}

	TEST(hoec_CameraFactoryClass, ActiveTakePictureThrow)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0)
		{
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}
		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;
		std::cout << "Will test camera by one" << '\n';
		{
			try
			{
				auto cameraActive = CameraFactory::CreateActiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered);
				cameraActive->startMonitor();
				try
				{
					auto image = cameraActive->getImage();
				}
				catch (const CameraRetrievalError& e)
				{
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e)
			{
				std::cout << e.what() << '\n';
				FAIL();
			}
		}
		SUCCEED();
	}

	TEST(hoec_CameraFactoryClass, PassiveTakePicture)
	{
		auto cameraList = CameraFactory::checkAllCamera();
		if (cameraList.size() == 0) {
			std::cout << "Please connect the camera before test" << '\n';
			SUCCEED();
			return;
		}

		CameraIP cameraIP;
		cameraIP.ip = cameraList[0].ip;
		cameraIP.provider = cameraList[0].provider;

		std::cout << "Will test camera by one" << '\n';
		{
			try {
				auto cameraPassive = CameraFactory::CreatePassiveCamera(cameraIP, CameraTriggerMode::SoftwareTriggered, [](cv::Mat mat) {
					std::cout << mat.size().width << '\n';
					std::cout << mat.size().height << '\n';
					std::cout << "We have gotten a image" << '\n';
					});

				try {
					cameraPassive->startMonitor();
					std::this_thread::sleep_for(std::chrono::milliseconds(5000));
					cameraPassive->stopMonitor();
				}
				catch (const CameraMonitorError& e) {
					std::cout << e.what() << '\n';
					FAIL();
				}
			}
			catch (const CameraCreateError& e) {
				std::cout << e.what() << '\n';
				FAIL();
			}
		}

		SUCCEED();
	}
}