#pragma once

#include"rqw_CameraObjectCore.hpp"

#include<opencv2/opencv.hpp>

#include<QThread>

namespace rw {
	namespace rqw {
		class CameraPassiveObjectZMotion;

		class CameraPassiveThreadZMotion : public QThread
		{
			Q_OBJECT
		public:
			size_t cameraIndex{ 0 };
			size_t motionRedix;
		public:
			explicit CameraPassiveThreadZMotion(QObject* parent = nullptr);
			CameraPassiveThreadZMotion(const CameraPassiveThreadZMotion&) = delete; // 拷贝构造函数被删除
			CameraPassiveThreadZMotion& operator=(const CameraPassiveThreadZMotion&) = delete; // 拷贝赋值运算符被删除
			// 其他成员函数和变量

			~CameraPassiveThreadZMotion() override;

			void initCamera(const rw::rqw::CameraMetaData& cameraMetaData, rw::rqw::CameraObjectTrigger triggerMode, size_t motionInde);

			bool getConnectState();
		public:
			void startMonitor();
			void stopMonitor();

		public:
			//TODO: Add more functions
			void setHeartbeatTime(size_t value) const;
			void setFrameRate(float value) const;
			size_t getHeartbeatTime() const;
			float getFrameRate() const;
			void setExposureTime(size_t value) const;
			void setGain(size_t value) const;
			void setTriggerMode(CameraObjectTrigger mode) const;
			void setTriggerLine(size_t lineIndex)const;
		public:
			//TODO: Add more functions
			[[nodiscard]] size_t getExposureTime() const;
			[[nodiscard]] size_t getGain() const;
			[[nodiscard]] CameraObjectTrigger getMonitorMode() const;
			[[nodiscard]] size_t getTriggerLine() const;

		signals:
			void frameCaptured(cv::Mat frame, float location, size_t index);
		protected:
			void run() override;

		private:
			CameraPassiveObjectZMotion* _cameraObject;
		private slots:
			void onFrameCaptured(cv::Mat frame, float location);
		};
	} // namespace rqw
} // namespace rw