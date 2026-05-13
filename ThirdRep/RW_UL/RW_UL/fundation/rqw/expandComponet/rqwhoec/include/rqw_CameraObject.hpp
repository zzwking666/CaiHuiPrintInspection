#pragma once

#include<memory>

#include"rqw_CameraObjectCore.hpp"

#include<opencv2/opencv.hpp>

#include<QObject>

namespace rw {
	namespace hoec_v1
	{
		class CameraPassive;
	}

	namespace rqw {
		class CameraPassiveObject : public QObject
		{
			Q_OBJECT
		private:
			std::unique_ptr<hoec_v1::CameraPassive> _cameraPassive;
			CameraMetaData _cameraMetaData;
		public:
			CallBackForImgReadyBefore callBackForImgReadyBefore;
		public:
			CameraPassiveObject(const CameraPassiveObject&) = delete;
			CameraPassiveObject& operator=(const CameraPassiveObject&) = delete;
			CameraPassiveObject(CameraPassiveObject&&) = delete;
			CameraPassiveObject& operator=(CameraPassiveObject&&) = delete;
		public:
			CameraPassiveObject(QObject* parent = nullptr);
			~CameraPassiveObject() override;
		public:
			bool startMonitor() const;
			bool stopMonitor() const;
		public:
			bool setHeartbeatTime(size_t value) const;
			bool setFrameRate(float value) const;
			bool setExposureTime(size_t value) const;
			bool setGain(size_t value) const;
			bool setTriggerMode(CameraObjectTrigger mode) const;
			bool setTriggerLine(size_t lineIndex)const;
		public:
			size_t getHeartbeatTime(bool& isGet) const;
			float getFrameRate(bool& isGet) const;
			[[nodiscard]] size_t getExposureTime(bool& isGet) const;
			[[nodiscard]] size_t getGain(bool& isGet) const;
			[[nodiscard]] CameraObjectTrigger getMonitorMode(bool& isGet) const;
			[[nodiscard]] size_t getTriggerLine(bool& isGet) const;

			size_t getHeartbeatTime() const;
			float getFrameRate() const;
			[[nodiscard]] size_t getExposureTime() const;
			[[nodiscard]] size_t getGain() const;
			[[nodiscard]] CameraObjectTrigger getMonitorMode() const;
			[[nodiscard]] size_t getTriggerLine() const;
		public:
			void initCamera(const CameraMetaData& cameraMetaData, CameraObjectTrigger triggerMode);
			bool getConnectState(bool& isGet);
			bool getConnectState();
		public:
			void setOutTriggerConfig(const OutTriggerConfig& config);
			void outTrigger();
			void outTrigger(bool isOpen);
		public:
			//线阵相机用
			bool setLineHeight(size_t number);
			size_t getLineHeight(bool& isGet);
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
			void frameCaptured(MatInfo frame);
		};
	} // namespace rqw
} // namespace rw