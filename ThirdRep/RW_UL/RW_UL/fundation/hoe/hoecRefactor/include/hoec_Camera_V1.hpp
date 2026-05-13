#pragma once

#include"hoec_Camera_utilty_V1.hpp"

namespace rw
{
	namespace hoec_v1
	{
		class ICamera
		{
		protected:
			CameraInfo _cameraInfo;
			std::string _ip;
		public:
			virtual ~ICamera() = default;
			ICamera() = default;
			ICamera(ICamera&&) = delete;
			ICamera& operator=(ICamera&&) = delete;
			ICamera(const ICamera&) = delete;
			ICamera& operator=(const ICamera&) = delete;

		public:
			void setIP(const std::string& ip);
			[[nodiscard]] std::string getIP(bool& isGet) const;

		public:
			virtual bool connectCamera() = 0;

			virtual bool getConnectState(bool& isGet) = 0;

			virtual bool disconnectCamera() = 0;

		public:
			virtual bool startMonitor() = 0;
			virtual bool stopMonitor() = 0;
		public:
			CameraInfo getCameraInfo();
		protected:
			void setCameraInfo(const CameraInfo& cameraInfo);
		public:
			virtual bool setHeartbeatTime(size_t heartBeatTime) = 0;
			virtual size_t getHeartbeatTime(bool& isGet) = 0;
			virtual bool setFrameRate(float cameraFrameRate) = 0;
			virtual float getFrameRate(bool& isGet) = 0;
			virtual bool setExposureTime(size_t value) = 0;
			virtual bool setGain(size_t value) = 0;
			virtual bool setInTriggerLine(size_t lineIndex) = 0;
		public:
			//面阵相机用
			virtual bool setTriggerMode(CameraTriggerMode mode) = 0;
			//线阵相机用
			virtual bool setFrameTriggered(bool state) = 0;
			virtual bool getFrameTriggered(bool& isGet) = 0;
			virtual bool setLineTriggered(bool state) = 0;
			virtual bool getLineTriggered(bool& isGet) = 0;
		public:
			//线阵相机额外配置
			virtual bool setPreDivider(size_t number) = 0;
			virtual bool setMultiplier(size_t number) = 0;
			virtual bool setPostDivider(size_t number) = 0;
			virtual bool getEncoderNumber(double& number) = 0;
			virtual bool setLineHeight(size_t number) = 0;
			virtual size_t getLineHeight(bool& isGet) = 0;
		public:
			//DS
			virtual bool setIOOut(int line_index, bool state) = 0;
			virtual bool getIOOut(int line_index, bool& state) = 0;
			virtual bool getIOIn(int line_index, bool& state) = 0;
			virtual bool encoderNumberReset()=0;
			//MVS
			virtual bool softwareTrigger() = 0;
		public:
			virtual bool setTriggerState(bool state) const = 0;
			virtual bool setTriggerSource(TriggerSource triggerSource) = 0;
		public:
			[[nodiscard]] virtual size_t getExposureTime(bool& isGet) = 0;
			[[nodiscard]] virtual size_t getGain(bool& isGet) = 0;
			[[nodiscard]] virtual CameraTriggerMode getMonitorMode(bool& isGet) = 0;
			[[nodiscard]] virtual size_t getTriggerLine(bool& isGet) = 0;
		public:
			virtual bool setOutTriggerConfig(const OutTriggerConfig& config) = 0;
			virtual bool outTrigger() = 0;
			virtual bool outTrigger(bool isOpen) = 0;
		};

		class ICameraActive
		{
		public:
			virtual ~ICameraActive() = default;
		public:
			[[nodiscard]] virtual cv::Mat getImage(bool& isGet) = 0;
			[[nodiscard]] virtual cv::Mat getImage() = 0;
		};

		class ICameraPassive
		{
		public:
			using UserToCallBack = std::function<void(MatInfo &)>;
		public:
			virtual ~ICameraPassive() = default;
			virtual void setUserToCallBackPre(UserToCallBackPre userToCallBackPre)=0;
		public:
			virtual bool RegisterCallBackFunc() = 0;
		};

		class CameraActive
			:public ICameraActive, public ICamera
		{
			friend class CameraFactory;
		protected:
			bool connectCamera() override;
		public:
			bool getConnectState(bool& isGet)override;
			bool setFrameRate(float cameraFrameRate) override;
		public:
			bool setHeartbeatTime(size_t heartBeatTime)override;
			size_t getHeartbeatTime(bool& isGet) override;
			float getFrameRate(bool& isGet) override;
			bool startMonitor() override;
			bool stopMonitor() override;
			bool setExposureTime(size_t value) override;
			bool setGain(size_t value) override;
			bool setTriggerMode(CameraTriggerMode mode) override;
			bool setInTriggerLine(size_t lineIndex) override;
			[[nodiscard]] size_t getExposureTime(bool& isGet) override;
			[[nodiscard]] size_t getGain(bool& isGet) override;
			[[nodiscard]] CameraTriggerMode getMonitorMode(bool& isGet) override;
			[[nodiscard]] size_t getTriggerLine(bool& isGet) override;
		public:
			[[nodiscard]] cv::Mat getImage(bool& isGet) override;
			[[nodiscard]] cv::Mat getImage() override;
		private:
			CameraProvider _provider;
		public:
			void setCameraProvider(CameraProvider provider);
			CameraProvider getCameraProvider() const;
		private:
			ICamera* _camera{ nullptr };
			ICameraActive* _cameraActive{ nullptr };
		public:
			CameraActive(ICamera* camera, ICameraActive* cameraActive);
		public:
			~CameraActive() override;
		public:
			bool setOutTriggerConfig(const OutTriggerConfig& config) override;
			bool outTrigger() override;
			bool outTrigger(bool isOpen)override;
		public:
			bool disconnectCamera() override;
			bool setFrameTriggered(bool state) override;
			bool getFrameTriggered(bool& isGet) override;
			bool setLineTriggered(bool state) override;
			bool getLineTriggered(bool& isGet) override;
			bool setPreDivider(size_t number) override;
			bool setMultiplier(size_t number) override;
			bool setPostDivider(size_t number) override;
			bool getEncoderNumber(double& number) override;
			bool setLineHeight(size_t number) override;
			size_t getLineHeight(bool& isGet) override;
			bool setIOOut(int line_index, bool state) override;
			bool getIOIn(int line_index, bool& state) override;
			bool getIOOut(int line_index, bool& state) override;
			bool softwareTrigger() override;
			bool setTriggerState(bool state) const override;
			bool setTriggerSource(TriggerSource triggerSource) override;
			bool encoderNumberReset() override;
		};

		class CameraPassive
			:public ICameraPassive, public ICamera
		{
			friend class CameraFactory;
		public:
			UserToCallBack _userToCallBack;
			UserToCallBackPre _userToCallBackPre;
		protected:
			bool connectCamera() override;
		public:
			bool getConnectState(bool& isGet)override;
			bool setFrameRate(float cameraFrameRate) override;
		public:
			bool setHeartbeatTime(size_t heartBeatTime)override;
			size_t getHeartbeatTime(bool& isGet) override;
			float getFrameRate(bool& isGet) override;
			bool startMonitor() override;
			bool stopMonitor() override;
			bool setExposureTime(size_t value) override;
			bool setGain(size_t value) override;
			bool setTriggerMode(CameraTriggerMode mode) override;
			bool setInTriggerLine(size_t lineIndex) override;
			size_t getExposureTime(bool& isGet) override;
			size_t getGain(bool& isGet) override;
			CameraTriggerMode getMonitorMode(bool& isGet) override;
		public:
			size_t getTriggerLine(bool& isGet) override;
			bool RegisterCallBackFunc() override;
		private:
			CameraProvider _provider;
		public:
			void setCameraProvider(CameraProvider provider);
			CameraProvider getCameraProvider() const;
		private:
			ICamera* _camera{ nullptr };
			ICameraPassive* _cameraPassive{ nullptr };
		public:
			CameraPassive(ICamera* camera, ICameraPassive* cameraPassive, UserToCallBack userToCallBack);
			CameraPassive(ICamera* camera, ICameraPassive* cameraPassive, UserToCallBack userToCallBack, UserToCallBackPre userToCallBackPre);
		public:
			~CameraPassive() override;
		public:
			bool setOutTriggerConfig(const OutTriggerConfig& config) override;
			bool outTrigger() override;
			bool outTrigger(bool isOpen)override;
		public:
			bool disconnectCamera() override;
			bool setFrameTriggered(bool state) override;
			bool getFrameTriggered(bool& isGet) override;
			bool setLineTriggered(bool state) override;
			bool getLineTriggered(bool& isGet) override;
			bool setPreDivider(size_t number) override;
			bool setMultiplier(size_t number) override;
			bool setPostDivider(size_t number) override;
			bool getEncoderNumber(double& number) override;
			bool setLineHeight(size_t number) override;
			size_t getLineHeight(bool& isGet) override;
			bool setIOOut(int line_index, bool state) override;
			bool getIOIn(int line_index, bool& state) override;
			bool getIOOut(int line_index, bool& state) override;
			bool softwareTrigger() override;
			bool setTriggerState(bool state) const override;
			bool setTriggerSource(TriggerSource triggerSource) override;
			bool encoderNumberReset() override;
			void setUserToCallBackPre(UserToCallBackPre userToCallBackPre) override;
		};
	}
}