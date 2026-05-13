#pragma once

#include"rqw_CameraObjectCore.hpp"

#include<opencv2/opencv.hpp>

#include<QThread>

namespace rw {
	namespace rqw {
		class CameraPassiveObject;

		class CameraPassiveThread : public QThread
		{
			Q_OBJECT
		public:
			size_t cameraIndex{ 0 };
			CallBackForImgReadyBefore callBackForImgReadyBefore;
		public:
			explicit CameraPassiveThread(QObject* parent = nullptr);
			CameraPassiveThread(const CameraPassiveThread&) = delete; // 拷贝构造函数被删除
			CameraPassiveThread& operator=(const CameraPassiveThread&) = delete; // 拷贝赋值运算符被删除
			// 其他成员函数和变量

			~CameraPassiveThread() override;

			void initCamera(const rw::rqw::CameraMetaData& cameraMetaData, rw::rqw::CameraObjectTrigger triggerMode);

			bool getConnectState();
		public:
			bool startMonitor();
			bool stopMonitor();

		public:
			bool setHeartbeatTime(size_t value) const;
			bool setFrameRate(float value) const;
			bool setExposureTime(size_t value) const;
			bool setGain(size_t value) const;
			bool setTriggerMode(CameraObjectTrigger mode) const;
			bool setTriggerLine(size_t lineIndex)const;
		public:
			[[nodiscard]] size_t getExposureTime() const;
			[[nodiscard]] size_t getGain() const;
			[[nodiscard]] CameraObjectTrigger getMonitorMode() const;
			[[nodiscard]] size_t getTriggerLine() const;
			size_t getHeartbeatTime() const;
			float getFrameRate() const;

			[[nodiscard]] size_t getExposureTime(bool& isGet) const;
			[[nodiscard]] size_t getGain(bool& isGet) const;
			[[nodiscard]] CameraObjectTrigger getMonitorMode(bool& isGet) const;
			[[nodiscard]] size_t getTriggerLine(bool& isGet) const;
			size_t getHeartbeatTime(bool& isGet) const;
			float getFrameRate(bool& isGet) const;
		public:
			//线阵相机用
			bool setLineHeight(size_t number) const;
			size_t getLineHeight(bool& isGet) const;
			bool setFrameTriggered(bool state);
			bool getFrameTriggered(bool& isGet);
			bool setLineTriggered(bool state);
			bool getLineTriggered(bool& isGet);
		public:
			//线阵相机额外配置
			bool setPreDivider(size_t number);
			bool setMultiplier(size_t number);
			bool setPostDivider(size_t number);
			bool getEncoderNumber(double& number);
		public:
			//DS专用
			bool setIOOut(int line_index, bool state);
			bool getIOIn(int line_index, bool& state);
			bool getIOOut(int line_index, bool& state);
			bool encoderNumberReset();
			//MVS
			bool softwareTrigger();
		public:
			bool setTriggerState(bool state) const;
			bool setTriggerSource(TriggerSource triggerSource);
		signals:
			void frameCaptured(MatInfo frame, size_t index);
		protected:
			void run() override;

		private:
			CameraPassiveObject* _cameraObject;
		private slots:
			void onFrameCaptured(MatInfo frame);
		public:
			bool setOutTriggerConfig(const OutTriggerConfig& config);
			bool outTrigger();
			bool outTrigger(bool isOpen);
		};
	} // namespace rqw
} // namespace rw