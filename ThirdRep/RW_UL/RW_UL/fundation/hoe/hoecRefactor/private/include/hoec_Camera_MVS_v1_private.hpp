#pragma once

#include"opencv2/opencv.hpp"

#include"hoec_Camera_v1.hpp"

#include<vector>
#include<string>
#include<functional>
#include<atomic>

struct _MV_FRAME_OUT_INFO_EX_;
typedef struct _MV_FRAME_OUT_INFO_EX_ MV_FRAME_OUT_INFO_EX;

namespace rw
{
	namespace hoec_v1
	{
		class Camera_MVS :
			public ICamera {
		private:
			static std::atomic<size_t> _cameraNum;
		public:
			Camera_MVS();
			~Camera_MVS() override;
		public:
			static bool _isIniSDK;
			static std::vector<std::string> getCameraIpList();
			static std::vector<CameraInfo> getCameraInfoList();
			static bool initSDK();
			static bool unInitSDK();
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
		public:
			bool setTriggerState(bool state) const override;
			bool setTriggerSource(TriggerSource triggerSource) override;
			bool encoderNumberReset() override;

		protected:
			void* m_cameraHandle{ nullptr };
		protected:
			bool _isMonitor{ false };
			CameraTriggerMode triggerMode;
		};

		class Camera_MVS_Active
			:public Camera_MVS, public ICameraActive {
		public:
			Camera_MVS_Active();
			~Camera_MVS_Active() override;
		public:
			cv::Mat getImage(bool& isGet) override;
			cv::Mat getImage() override;
		};

		class Camera_MVS_Passive
			:public Camera_MVS, public ICameraPassive {
		private:
			UserToCallBack _userToCallBack;
			UserToCallBackPre _userToCallBackPre;
		public:
			Camera_MVS_Passive(UserToCallBack userToCallback = [](MatInfo mat) {
				std::cout << "No callback function" << std::endl;
				return;
				});
			~Camera_MVS_Passive() override;
		public:
			bool RegisterCallBackFunc() override;
		public:

			static void __stdcall ImageCallBackFunc(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser);
			void setUserToCallBackPre(UserToCallBackPre userToCallBackPre) override;
		};
	}
}