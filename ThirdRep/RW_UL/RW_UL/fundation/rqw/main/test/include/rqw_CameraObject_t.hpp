//#pragma once
//
//#include "pch.h"
//#include "rqw_CameraObject.hpp"
//#include <gtest/gtest.h>
//#include <QCoreApplication>
//
//using namespace rw;
//
//class testComponetCameraObject : public QObject
//{
//    Q_OBJECT
//public:
//    bool isFrameCaptured = false;
//    bool isFrameCapturedWithoutArgs = false;
//    bool isFrameCapturedWithMetaData = false;
//    rqw::CameraMetaData cameraMetaData;
//public slots:
//    void slotFrameCaptured(cv::Mat mat)
//    {
//        isFrameCaptured = true;
//    }
//
//    void slotFrameCapturedWithoutArgs()
//    {
//        isFrameCapturedWithoutArgs = true;
//    }
//
//    void slotFrameCapturedWithMetaData(cv::Mat mat, rqw::CameraMetaData cameraMetaData)
//    {
//        isFrameCapturedWithMetaData = true;
//        this->cameraMetaData = cameraMetaData;
//    }
//};
//
//class CameraPassiveObjectTest
//    : public ::testing::Test
//{
//protected:
//    void SetUp() override
//    {
//        int argc = 0;
//        char** argv = nullptr;
//        app = new QCoreApplication(argc, argv);
//
//        testObj = new rqw::CameraPassiveObject();
//        rqw::CameraPassiveObject cameraPassiveObject;
//        auto cameraList = rqw::CheckCameraList();
//        if (cameraList.size() == 0)
//        {
//            std::cout << "No camera found" << '\n';
//            std::cout << "Test skipped,if you want to test ,please connect camera" << '\n';
//            isConstruct = false;
//            return;
//        }
//        cameraMetaData = cameraList[0];
//        std::cout << "Test target camera: " << '\n';
//        std::cout << "Camera IP: " << cameraMetaData.ip.toStdString() << '\n';
//        std::cout << "Camera Provider: " << cameraMetaData.provider.toStdString() << '\n';
//        testObj->initCamera(cameraMetaData, rqw::CameraObjectTrigger::Software);
//
//        QObject::connect(testObj, &rqw::CameraPassiveObject::frameCaptured, &testComponent, &testComponetCameraObject::slotFrameCaptured);
//        QObject::connect(testObj, &rqw::CameraPassiveObject::frameCapturedWithoutArgs, &testComponent, &testComponetCameraObject::slotFrameCapturedWithoutArgs);
//        QObject::connect(testObj, &rqw::CameraPassiveObject::frameCapturedWithMetaData, &testComponent, &testComponetCameraObject::slotFrameCapturedWithMetaData);
//        isConstruct = true;
//    }
//
//    void TearDown() override
//    {
//        delete testObj;
//        delete app;
//    }
//public:
//    rqw::CameraPassiveObject* testObj;
//public:
//    rqw::CameraMetaData cameraMetaData;
//    testComponetCameraObject testComponent;
//    QCoreApplication* app;
//public:
//    bool isConstruct;
//};