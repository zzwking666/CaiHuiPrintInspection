//#include"rqw_CameraObject_t.hpp"
//
//namespace rqw_CameraObject
//{
//    TEST(Global, CheckCameraListFunc)
//    {
//        auto cameraList=rqw::CheckCameraList();
//        std::cout << "Camera List: " << '\n';
//        for (auto camera : cameraList)
//        {
//            std::cout << camera.ip.toStdString() << '\n';
//            std::cout << camera.provider.toStdString() << '\n';
//        }
//    }
//
//    TEST(CameraPassiveObject_class,construct)
//    {
//        rqw::CameraPassiveObject cameraPassiveObject;
//        auto cameraList = rqw::CheckCameraList();
//        if (cameraList.size() == 0)
//        {
//            std::cout << "No camera found" << '\n';
//            std::cout << "Test skipped,if you want to test ,please connect camera" << '\n';
//            return;
//        }
//        auto cameraMetaData = cameraList[0];
//        std::cout << "Test target camera: "  << '\n';
//        std::cout << "Camera IP: " << cameraMetaData.ip.toStdString() << '\n';
//        std::cout << "Camera Provider: " << cameraMetaData.provider.toStdString() << '\n';
//
//        cameraPassiveObject.initCamera(cameraMetaData, rqw::CameraObjectTrigger::Software);
//        cameraPassiveObject.startMonitor();
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        cameraPassiveObject.stopMonitor();
//    }
//
//    TEST_F(CameraPassiveObjectTest, signalFrameCaptured)
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
//
//    TEST_F(CameraPassiveObjectTest, apiSetGet)
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
//
//
//        testObj->stopMonitor();
//    }
//
//
//
//
//}