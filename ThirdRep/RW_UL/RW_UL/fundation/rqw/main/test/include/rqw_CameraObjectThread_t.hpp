//#pragma once
//
//#include"rqw_CameraObjectThread.hpp"
//
//#include <QCoreApplication>
//
//using namespace rw;
//
//class testComponetCameraObjectThread : public QObject
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
//class CameraPassiveObjectThreadTest
//    : public ::testing::Test
//{
//protected:
//    void SetUp() override
//    {
//        int argc = 0;
//        char** argv = nullptr;
//        app = new QCoreApplication(argc, argv);
//
//        testObj = new rqw::CameraPassiveThread();
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
//        QObject::connect(testObj, &rqw::CameraPassiveThread::frameCaptured, &testComponent, &testComponetCameraObjectThread::slotFrameCaptured, Qt::QueuedConnection);
//        QObject::connect(testObj, &rqw::CameraPassiveThread::frameCapturedWithoutArgs, &testComponent, &testComponetCameraObjectThread::slotFrameCapturedWithoutArgs, Qt::QueuedConnection);
//        QObject::connect(testObj, &rqw::CameraPassiveThread::frameCapturedWithMetaData, &testComponent, &testComponetCameraObjectThread::slotFrameCapturedWithMetaData, Qt::QueuedConnection);
//        isConstruct = true;
//    }
//
//    void TearDown() override
//    {
//        delete testObj;
//        delete app;
//    }
//public:
//    rqw::CameraPassiveThread* testObj;
//public:
//    rqw::CameraMetaData cameraMetaData;
//    testComponetCameraObjectThread testComponent;
//    QCoreApplication* app;
//public:
//    bool isConstruct;
//};