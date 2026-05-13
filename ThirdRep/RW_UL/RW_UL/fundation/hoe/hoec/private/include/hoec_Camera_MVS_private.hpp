#ifndef HOEC_CAMERA_MVS_PRIVATE_H_
#define HOEC_CAMERA_MVS_PRIVATE_H_

#include"opencv2/opencv.hpp"

#include"hoec_Camera.hpp"

#include<vector>
#include<string>
#include<functional>
#include<atomic>

struct _MV_FRAME_OUT_INFO_EX_;
typedef struct _MV_FRAME_OUT_INFO_EX_ MV_FRAME_OUT_INFO_EX;

namespace rw {
	namespace hoec {
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
			static void initSDK();
			static void unInitSDK();
		public:
			void connectCamera() override;
			bool getConnectState()override;
			void setFrameRate(float cameraFrameRate) override;
			float getFrameRate() override;
		public:
			void startMonitor() override;
			void stopMonitor() override;
		public:
			void setHeartbeatTime(size_t heartBeatTime)override;
			float getHeartbeatTime() override;
			void setExposureTime(size_t value) override;
			void setGain(size_t value) override;
			void setTriggerMode(CameraTriggerMode mode) override;
			void setInTriggerLine(size_t lineIndex) override;
		public:
			size_t getExposureTime() override;
			size_t getGain() override;
			CameraTriggerMode getMonitorMode() override;
			size_t getTriggerLine() override;
		public:
			void setOutTriggerConfig(const OutTriggerConfig& config) override;
			void outTrigger() override;
			void outTrigger(bool isOpen) override;
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
		public:
			Camera_MVS_Passive(UserToCallBack userToCallback = [](cv::Mat mat) {
				std::cout << "No callback function" << std::endl;
				return;
				});
			~Camera_MVS_Passive() override;
		public:
			void RegisterCallBackFunc() override;
		public:

			static void __stdcall ImageCallBackFunc(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser);
		};
	} // namespace hoec
} // namespace rw

#endif // !HOEC_CAMERA_MVS_PRIVATE_H_