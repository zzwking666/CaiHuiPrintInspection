//#include"rqw_CameraObjectThread_t.hpp"
//
//namespace rqw_CameraObjectThread
//{
//    TEST(CameraPassiveThread_class, construct)
//    {
//        auto cameraList = rqw::CheckCameraList();
//        if (cameraList.size() == 0)
//        {
//            std::cout << "No camera found" << '\n';
//            std::cout << "Test skipped,if you want to test ,please connect camera" << '\n';
//            return;
//        }
//        auto cameraMetaData = cameraList[0];
//        std::cout << "Test target camera: " << '\n';
//        std::cout << "Camera IP: " << cameraMetaData.ip.toStdString() << '\n';
//        std::cout << "Camera Provider: " << cameraMetaData.provider.toStdString() << '\n';
//
//        rqw::CameraPassiveThread cameraPassiveThread;
//
//        cameraPassiveThread.initCamera(cameraMetaData, rqw::CameraObjectTrigger::Software);
//        cameraPassiveThread.startMonitor();
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        cameraPassiveThread.stopMonitor();
//    }
//
//    TEST_F(CameraPassiveObjectThreadTest, signalFrameCaptured)
//    {
//        if (!isConstruct)
//        {
//            std::cout << "Test skipped" << '\n';
//            std::cout << "if you want to test ,please connect camera" << '\n';
//            return;
//        }
//        testObj->startMonitor();
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        QCoreApplication::processEvents();
//        testObj->stopMonitor();
//
//        EXPECT_TRUE(testComponent.isFrameCaptured);
//        EXPECT_TRUE(testComponent.isFrameCapturedWithoutArgs);
//        EXPECT_TRUE(testComponent.isFrameCapturedWithMetaData);
//        EXPECT_EQ(testComponent.cameraMetaData.ip, cameraMetaData.ip);
//        EXPECT_EQ(testComponent.cameraMetaData.provider, cameraMetaData.provider);
//    }
//
//    TEST_F(CameraPassiveObjectThreadTest, SetAndGet)
//    {
//        if (!isConstruct)
//        {
//            std::cout << "Test skipped" << '\n';
//            std::cout << "if you want to test ,please connect camera" << '\n';
//            return;
//        }
//        testObj->startMonitor();
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        QCoreApplication::processEvents();
//
//        testObj->setExposureTime(1000);
//        testObj->setGain(10);
//        /*testObj->setIOTime(1000);*/
//        testObj->setTriggerMode(rqw::CameraObjectTrigger::Hardware);
//        /*testObj->setTriggerLine(1);*/
//
//        EXPECT_EQ(testObj->getExposureTime(), 1000);
//        EXPECT_EQ(testObj->getGain(), 10);
//        /*EXPECT_EQ(testObj->getIOTime(), 1000);*/
//        EXPECT_EQ(testObj->getMonitorMode(), rqw::CameraObjectTrigger::Hardware);
//        /*EXPECT_EQ(testObj->getTriggerLine(), 1);*/
//        testObj->stopMonitor();
//
//    }
//}
//
//
//