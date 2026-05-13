#pragma once

#include"opencv2/opencv.hpp"

#include"hoec_Camera_v1.hpp"

#include<vector>
#include<string>
#include<functional>
#include<atomic>

#include"DVPCamera.h"
#include"dvpir.h"
#include"dvpParam.h"

namespace rw
{
	namespace hoec_v1
	{
		class Camera_DS :
			public ICamera {
		public:
			Camera_DS();
			~Camera_DS() override;
		public:
			static std::vector<std::string> getCameraIpList();
			static std::vector<CameraInfo> getCameraInfoList();
		public:
			bool connectCamera() override;
			bool getConnectState(bool& isGet)override;
			bool setFrameRate(float cameraFrameRate) override;
			float getFrameRate(bool& isGet) override;
		public:
			bool startMonitor() override;
			bool stopMonitor() override;
		public:
			bool setHeartbeatTime(size_t heartBeatTime)override;
			size_t getHeartbeatTime(bool& isGet) override;
			bool setExposureTime(size_t value) override;
			bool setGain(size_t value) override;
			bool setTriggerMode(CameraTriggerMode mode) override;
			bool setInTriggerLine(size_t lineIndex) override;
		public:
			size_t getExposureTime(bool& isGet) override;
			size_t getGain(bool& isGet) override;
			CameraTriggerMode getMonitorMode(bool& isGet) override;
			size_t getTriggerLine(bool& isGet) override;
		public:
			bool setOutTriggerConfig(const OutTriggerConfig& config) override;
			bool outTrigger() override;
			bool outTrigger(bool isOpen) override;
		public:
			bool setFrameTriggered(bool state) override;
			bool getFrameTriggered(bool& isGet) override;
			bool setLineTriggered(bool state) override;
			bool getLineTriggered(bool& isGet) override;
			bool disconnectCamera() override;
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

		protected:
			dvpHandle m_cameraHandle{};
		protected:
			bool _isMonitor{ false };
			CameraTriggerMode triggerMode;
		};

		class Camera_DS_Active
			:public Camera_DS, public ICameraActive {
		public:
			Camera_DS_Active();
			~Camera_DS_Active() override;
		public:
			cv::Mat getImage(bool& isGet) override;
			cv::Mat getImage() override;
		};

		class Camera_DS_Passive
			:public Camera_DS, public ICameraPassive {
		private:
			UserToCallBack _userToCallBack;
			UserToCallBackPre _userToCallBackPre;
		public:
			Camera_DS_Passive(UserToCallBack userToCallback = [](MatInfo mat) {
				std::cout << "No callback function" << std::endl;
				return;
				});
			~Camera_DS_Passive() override;
		public:
			bool RegisterCallBackFunc() override;
		public:

			static dvpInt32 __stdcall ImageCallBackFunc(
				dvpHandle handle,
				dvpStreamEvent event,
				void* pContext,
				dvpFrame* pFrame,
				void* pBuffer);
			void setUserToCallBackPre(UserToCallBackPre userToCallBackPre) override;
		};
	}
}