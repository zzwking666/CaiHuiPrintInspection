#include"hoec_Camera_MVS_private.hpp"

#include"hoec_Camera_core_private.hpp"
#include"hoec_CameraException.hpp"

#include"MvCameraControl.h"
#include"CameraParams.h"

#include<algorithm>

namespace rw {
	namespace hoec {
		bool Camera_MVS::_isIniSDK = false;
		std::atomic<size_t> Camera_MVS::_cameraNum = 0;

		Camera_MVS::Camera_MVS()
			:triggerMode(CameraTriggerMode::SoftwareTriggered)
		{
			if (_cameraNum == 0)
			{
				initSDK();
			}
			_cameraNum++;
		}
		Camera_MVS::~Camera_MVS()
		{
			//先停止采集
			try
			{
				if (_isMonitor) {
					Camera_MVS::stopMonitor();
				}
			}
			catch (const std::exception&)
			{
			}
			//关闭相机
			if (m_cameraHandle) {
				MV_CC_CloseDevice(m_cameraHandle);
				MV_CC_DestroyHandle(m_cameraHandle);
			}
			_cameraNum--;
			if (_cameraNum == 0)
			{
				unInitSDK();
			}
		}
		std::vector<std::string> Camera_MVS::getCameraIpList()
		{
			MV_CC_DEVICE_INFO_LIST deviceList;
			memset(&deviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

			// 枚举设备
			if (MV_CC_EnumDevices(MV_GIGE_DEVICE, &deviceList) != MV_OK) {
				return std::vector<std::string>();
			}

			std::vector<std::string> ipList;
			for (unsigned int i = 0; i < deviceList.nDeviceNum; ++i) {
				MV_CC_DEVICE_INFO* pDeviceInfo = deviceList.pDeviceInfo[i];
				if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
					// 获取设备的 IP 地址
					unsigned int ip = pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp;
					std::string ipStr = std::to_string((ip >> 24) & 0xFF) + "." +
						std::to_string((ip >> 16) & 0xFF) + "." +
						std::to_string((ip >> 8) & 0xFF) + "." +
						std::to_string(ip & 0xFF);
					ipList.push_back(ipStr);
				}
			}

			return ipList;
		}

		std::vector<CameraInfo> Camera_MVS::getCameraInfoList()
		{
			MV_CC_DEVICE_INFO_LIST deviceList;
			memset(&deviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
			// 枚举设备
			if (MV_CC_EnumDevices(MV_GIGE_DEVICE, &deviceList) != MV_OK) {
				return std::vector<CameraInfo>();
			}
			std::vector<CameraInfo> cameraInfoList;
			for (unsigned int i = 0; i < deviceList.nDeviceNum; ++i) {
				MV_CC_DEVICE_INFO* pDeviceInfo = deviceList.pDeviceInfo[i];
				if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
					CameraInfo cameraInfo;
					// 获取设备的 IP 地址
					unsigned int ip = pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp;
					cameraInfo.ip = std::to_string((ip >> 24) & 0xFF) + "." +
						std::to_string((ip >> 16) & 0xFF) + "." +
						std::to_string((ip >> 8) & 0xFF) + "." +
						std::to_string(ip & 0xFF);
					cameraInfo.name = std::string("MVS") + std::to_string(pDeviceInfo->nMajorVer) + std::to_string(pDeviceInfo->nMinorVer);
					cameraInfo.mac = std::to_string(pDeviceInfo->nMacAddrHigh) + std::to_string(pDeviceInfo->nMacAddrLow);
					cameraInfoList.push_back(cameraInfo);
				}
			}
			return cameraInfoList;
		}

		void Camera_MVS::initSDK()
		{
			_isIniSDK = true;
			auto result = MV_CC_Initialize();
			if (result == MV_OK) {
				return;
			}
			throw CameraInitError("Failed to initialize SDK");
		}

		void Camera_MVS::unInitSDK()
		{
			_isIniSDK = false;
			auto result = MV_CC_Finalize();
			if (result == MV_OK) {
				return;
			}
			throw CameraInitError("Failed to uninitialize SDK");
		}

		void Camera_MVS::connectCamera()
		{
			MV_CC_DEVICE_INFO_LIST deviceList;
			memset(&deviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

			if (MV_CC_EnumDevices(MV_GIGE_DEVICE, &deviceList) != MV_OK) {
				throw CameraConnectionError("Failed to enumerate devices");
			}

			MV_CC_DEVICE_INFO* targetDevice = nullptr;
			for (unsigned int i = 0; i < deviceList.nDeviceNum; ++i) {
				auto pDeviceInfo = deviceList.pDeviceInfo[i];
				if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
					// 获取设备的 IP 地址
					unsigned int ip = pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp;
					std::string ipStr = std::to_string((ip >> 24) & 0xFF) + "." +
						std::to_string((ip >> 16) & 0xFF) + "." +
						std::to_string((ip >> 8) & 0xFF) + "." +
						std::to_string(ip & 0xFF);
					if (ipStr == _ip) {
						targetDevice = pDeviceInfo;
						break;
					}
				}
			}
			if (!targetDevice) {
				throw CameraConnectionError("Failed to find target device");
			}

			//是否可以独占访问
			auto canAccessible = MV_CC_IsDeviceAccessible(targetDevice, MV_ACCESS_Exclusive);
			if (canAccessible != true) {
				throw CameraConnectionError("Failed to access device");
			}

			//创建句柄
			auto creatHandleResult = MV_CC_CreateHandle(&m_cameraHandle, targetDevice);
			if (creatHandleResult != MV_OK) {
				throw CameraConnectionError("Failed to create handle");
			}

			//独占打开设备
			auto openDeviceResult = MV_CC_OpenDevice(m_cameraHandle, MV_ACCESS_Exclusive);
			if (openDeviceResult != MV_OK) {
				throw CameraConnectionError("Failed to open device");
			}

			auto cameraInfoList = Camera_MVS::getCameraInfoList();
			auto findResult = std::find_if(cameraInfoList.begin(), cameraInfoList.end(), [this](const CameraInfo& cameraInfo) {
				return cameraInfo.ip == _ip;
				});

			if (findResult == cameraInfoList.end()) {
				throw CameraConnectionError("Failed to find target device");
			}

			_cameraInfo = *findResult;

			return;
		}

		bool Camera_MVS::getConnectState()
		{
			return MV_CC_IsDeviceConnected(m_cameraHandle);
		}

		void Camera_MVS::setFrameRate(float cameraFrameRate)
		{
			auto result = MV_CC_SetFrameRate(m_cameraHandle, cameraFrameRate);
			if (result == MV_OK) {
				return;
			}
			throw CameraSettingError("Failed to set camera frame rate");
		}

		float Camera_MVS::getFrameRate()
		{
			MVCC_FLOATVALUE frameRate;
			auto result = MV_CC_GetFrameRate(m_cameraHandle, &frameRate);
			if (result == MV_OK) {
				return frameRate.fCurValue;
			}
		}

		void Camera_MVS::startMonitor()
		{
			if (_isMonitor) {
				throw CameraMonitorError("Monitor has already started");
			}

			auto result = MV_CC_StartGrabbing(m_cameraHandle);
			if (result != MV_OK) {
				throw CameraMonitorError("Failed to start grabbing");
			}
			_isMonitor = true;
			return;
		}

		void Camera_MVS::stopMonitor()
		{
			if (!_isMonitor) {
				throw CameraMonitorError("Monitor has already stopped");
			}
			auto result = MV_CC_StopGrabbing(m_cameraHandle);
			if (result != MV_OK) {
				throw CameraMonitorError("Failed to stop grabbing");
			}
			_isMonitor = false;
			return;
		}

		void Camera_MVS::setHeartbeatTime(size_t heartBeatTime)
		{
			auto result = MV_CC_SetHeartBeatTimeout(m_cameraHandle, heartBeatTime);
			if (result == MV_OK) {
				return;
			}
			throw CameraSettingError("Failed to set heartbeat time");
		}

		float Camera_MVS::getHeartbeatTime()
		{
			MVCC_INTVALUE heartbeatTime;
			auto result = MV_CC_GetHeartBeatTimeout(m_cameraHandle, &heartbeatTime);
			if (result == MV_OK) {
				return heartbeatTime.nCurValue;
			}
			else {
				throw CameraRetrievalError("Failed to get heartbeat time");
			}
		}

		void Camera_MVS::setExposureTime(size_t value)
		{
			float exposureTime = static_cast<float>(value);
			auto result = MV_CC_SetExposureTime(m_cameraHandle, exposureTime);
			if (result == MV_OK)
				return;

			throw CameraSettingError("Failed to set exposuretime");
		}

		void Camera_MVS::setGain(size_t value)
		{
			float gain = static_cast<float>(value);
			auto result = MV_CC_SetGain(m_cameraHandle, gain);
			if (result == MV_OK)
				return;
			throw CameraSettingError("Failed to set gain");
		}

		size_t Camera_MVS::getExposureTime()
		{
			MVCC_FLOATVALUE exposureTime;
			memset(&exposureTime, 0, sizeof(MVCC_FLOATVALUE));
			auto result = MV_CC_GetExposureTime(m_cameraHandle, &exposureTime);
			if (result == MV_OK) {
				return static_cast<size_t>(exposureTime.fCurValue);
			}

			throw CameraRetrievalError("Failed to get exposure time");
		}

		size_t Camera_MVS::getGain()
		{
			MVCC_FLOATVALUE gain;
			memset(&gain, 0, sizeof(MVCC_FLOATVALUE));
			auto result = MV_CC_GetGain(m_cameraHandle, &gain);
			if (result == MV_OK) {
				return static_cast<size_t>(gain.fCurValue);
			}

			throw CameraRetrievalError("Failed to get gain");
		}

		CameraTriggerMode Camera_MVS::getMonitorMode() {
			return triggerMode;
		}

		void Camera_MVS::setTriggerMode(CameraTriggerMode mode)
		{
			triggerMode = mode;
			unsigned int modeValue;
			if (mode == CameraTriggerMode::SoftwareTriggered)
			{
				modeValue = 0;
			}
			else if (mode == CameraTriggerMode::HardwareTriggered)
			{
				modeValue = 1;
			}
			else
			{
				throw CameraSettingError("Invalid trigger mode");
			}

			if (MV_CC_SetTriggerMode(m_cameraHandle, modeValue) == MV_OK)
			{
				return;
			}
			else
			{
				throw CameraSettingError("Failed to set trigger mode");
			}
		}

		void Camera_MVS::setInTriggerLine(size_t lineIndex)
		{
			unsigned int lineValue = static_cast <unsigned int> (lineIndex);
			if (MV_CC_SetTriggerSource(m_cameraHandle, lineValue) == MV_OK)
			{
				return;
			}

			throw CameraSettingError("Failed to set trigger line");
		}

		size_t Camera_MVS::getTriggerLine()
		{
			// 获取触发线索引的实现
			MVCC_ENUMVALUE stTriggerSource;
			if (MV_CC_GetTriggerSource(m_cameraHandle, &stTriggerSource) == MV_OK)
			{
				return static_cast<size_t>(stTriggerSource.nCurValue);
			}

			throw CameraRetrievalError("Failed to get trigger line");
		}

		void Camera_MVS::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			if (!config.strobeEnable)
			{
				MV_CC_SetBoolValue(m_cameraHandle, "StrobeEnable", config.strobeEnable);
				return;
			}
			MV_CC_SetEnumValue(m_cameraHandle, "LineSelector", config.lineSelector);
			MV_CC_SetEnumValue(m_cameraHandle, "LineMode", config.lineMode);
			MV_CC_SetEnumValue(m_cameraHandle, "LineSource", config.lineSource);
			MV_CC_SetIntValue(m_cameraHandle, "StrobeLineDuration", config.durationValue);
			MV_CC_SetIntValue(m_cameraHandle, "StrobeLineDelay", config.delayValue);
			MV_CC_SetIntValue(m_cameraHandle, "StrobeLinePreDelay", config.preDelayValue);
		}

		void Camera_MVS::outTrigger()
		{
			MV_CC_SetCommandValue(m_cameraHandle, "LineTriggerSoftware");
		}

		void Camera_MVS::outTrigger(bool isOpen)
		{
			MV_CC_SetBoolValue(m_cameraHandle, "LineInverter", isOpen);
		}

		Camera_MVS_Active::Camera_MVS_Active()
		{
		}

		Camera_MVS_Active::~Camera_MVS_Active()
		{
		}

		cv::Mat Camera_MVS_Active::getImage(bool& isGet)
		{
			MV_FRAME_OUT frameInfo;
			auto getResult = MV_CC_GetImageBuffer(m_cameraHandle, &frameInfo, 1000);
			if (getResult != MV_OK) {
				isGet = false;
				std::cerr << "Failed to get image buffer" << std::endl;
			}
			cv::Mat image = ImageFrameConvert::MVS_ConvertFrameToMat(frameInfo);
			if (image.empty())
			{
				isGet = false;
				std::cerr << "Failed to convert frame to mat" << std::endl;
			}
			isGet = true;
			auto freeResult = MV_CC_FreeImageBuffer(m_cameraHandle, &frameInfo);
			if (freeResult != MV_OK) {
				isGet = false;
				std::cerr << "Failed to free image buffer" << std::endl;
			}
			return image;
		}

		cv::Mat Camera_MVS_Active::getImage()
		{
			MV_FRAME_OUT frameInfo;
			auto getResult = MV_CC_GetImageBuffer(m_cameraHandle, &frameInfo, 1000);
			if (getResult != MV_OK) {
				throw CameraRetrievalError("Failed to get image buffer");
			}
			cv::Mat image = ImageFrameConvert::MVS_ConvertFrameToMat(frameInfo);
			if (image.empty())
			{
				throw CameraRetrievalError("Failed to convert frame to mat");
			}
			auto freeResult = MV_CC_FreeImageBuffer(m_cameraHandle, &frameInfo);
			if (freeResult != MV_OK) {
				throw CameraRetrievalError("Failed to free image buffer");
			}
			return image;
		}

		Camera_MVS_Passive::Camera_MVS_Passive(UserToCallBack userToCallback)
			:_userToCallBack(userToCallback)
		{
		}

		Camera_MVS_Passive::~Camera_MVS_Passive()
		{
			try
			{
				if (_isMonitor)
				{
					this->Camera_MVS::stopMonitor();
					_isMonitor = false;
				}
			}
			catch (const std::exception&)
			{
			}
		}

		void Camera_MVS_Passive::RegisterCallBackFunc()
		{
			auto result = MV_CC_RegisterImageCallBackEx(m_cameraHandle, Camera_MVS_Passive::ImageCallBackFunc, this);
			if (result != MV_OK) {
				throw CameraMonitorError("Failed to register image callback");
			}
			return;
		}

		void __stdcall Camera_MVS_Passive::ImageCallBackFunc(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser) {
			Camera_MVS_Passive* pThis = static_cast<Camera_MVS_Passive*>(pUser);
			if (pFrameInfo)
			{
				auto image = ImageFrameConvert::MVS_ConvertFrameToMat(*pFrameInfo, pData);
				if (pThis) {
					pThis->_userToCallBack(std::move(image));
				}
				else {
					throw CameraMonitorError("Failed to get user pointer");
				}
			}
		}
	} // namespace hoec
} // namespace rw