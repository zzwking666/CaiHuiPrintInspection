#include"hoec_Camera_MVS_v1_private.hpp"

#include"hoec_Camera_core_v1_private.hpp"

#include"MvCameraControl.h"
#include"CameraParams.h"

namespace rw
{
	namespace hoec_v1
	{
		std::atomic<size_t> Camera_MVS::_cameraNum = 0;
		bool Camera_MVS::_isIniSDK = false;

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
			if (_isMonitor) {
				Camera_MVS::stopMonitor();
			}

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

		bool Camera_MVS::initSDK()
		{
			_isIniSDK = true;
			auto result = MV_CC_Initialize();
			if (result == MV_OK) {
				return true;
			}
			return false;
		}

		bool Camera_MVS::unInitSDK()
		{
			_isIniSDK = false;
			auto result = MV_CC_Finalize();
			if (result == MV_OK) {
				return true;
			}
			return false;
		}

		bool Camera_MVS::connectCamera()
		{
			MV_CC_DEVICE_INFO_LIST deviceList;
			memset(&deviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

			if (MV_CC_EnumDevices(MV_GIGE_DEVICE, &deviceList) != MV_OK) {
				std::cerr << "Failed to enumerate devices" << std::endl;
				return false;
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
				std::cerr << "Failed to find target device" << std::endl;
				return false;
			}

			//是否可以独占访问
			auto canAccessible = MV_CC_IsDeviceAccessible(targetDevice, MV_ACCESS_Exclusive);
			if (canAccessible != true) {
				std::cerr << "Failed to access device" << std::endl;
				return false;
			}

			//创建句柄
			auto creatHandleResult = MV_CC_CreateHandle(&m_cameraHandle, targetDevice);
			if (creatHandleResult != MV_OK) {
				std::cerr << "Failed to create handle" << std::endl;
				return false;
			}

			//独占打开设备
			auto openDeviceResult = MV_CC_OpenDevice(m_cameraHandle, MV_ACCESS_Exclusive);
			if (openDeviceResult != MV_OK) {
				std::cerr << "Failed to open device" << std::endl;
				return false;
			}

			auto cameraInfoList = Camera_MVS::getCameraInfoList();
			auto findResult = std::find_if(cameraInfoList.begin(), cameraInfoList.end(), [this](const CameraInfo& cameraInfo) {
				return cameraInfo.ip == _ip;
				});

			if (findResult == cameraInfoList.end()) {
				std::cerr << "Failed to find target device" << std::endl;
				return false;
			}

			_cameraInfo = *findResult;

			return true;
		}

		bool Camera_MVS::getConnectState(bool& isGet)
		{
			if (m_cameraHandle)
			{
				isGet = true;
				return MV_CC_IsDeviceConnected(m_cameraHandle);
			}
			else
			{
				isGet = false;
				return false;
			}
		}

		bool Camera_MVS::setFrameRate(float cameraFrameRate)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			auto result = MV_CC_SetFrameRate(m_cameraHandle, cameraFrameRate);
			if (result == MV_OK) {
				return true;
			}
			return false;
		}

		float Camera_MVS::getFrameRate(bool& isGet)
		{
			if (!m_cameraHandle)
			{
				isGet = false;
				return 0;
			}
			isGet = true;

			MVCC_FLOATVALUE frameRate;
			auto result = MV_CC_GetFrameRate(m_cameraHandle, &frameRate);
			if (result == MV_OK) {
				return frameRate.fCurValue;
			}
			return 0;
		}

		bool Camera_MVS::startMonitor()
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			if (_isMonitor) {
				std::cerr << "Monitor has already started" << std::endl;
				return false;
			}

			auto result = MV_CC_StartGrabbing(m_cameraHandle);
			if (result != MV_OK) {
				std::cerr << "Failed to start grabbing" << std::endl;
				return false;
			}
			_isMonitor = true;
			return true;
		}

		bool Camera_MVS::stopMonitor()
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			if (!_isMonitor) {
				std::cerr << "Monitor has already stopped" << std::endl;
				return false;
			}
			auto result = MV_CC_StopGrabbing(m_cameraHandle);
			if (result != MV_OK) {
				std::cerr << "Failed to stop grabbing" << std::endl;
				return false;
			}
			_isMonitor = false;
			return true;
		}

		bool Camera_MVS::setHeartbeatTime(size_t heartBeatTime)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			auto result = MV_CC_SetHeartBeatTimeout(m_cameraHandle, heartBeatTime);
			if (result == MV_OK) {
				return true;
			}
			return false;
		}

		size_t Camera_MVS::getHeartbeatTime(bool& isGet)
		{
			if (!m_cameraHandle)
			{
				isGet = false;
				return 0;
			}
			isGet = true;

			MVCC_INTVALUE heartbeatTime;
			auto result = MV_CC_GetHeartBeatTimeout(m_cameraHandle, &heartbeatTime);
			if (result == MV_OK) {
				return heartbeatTime.nCurValue;
			}
			else {
				std::cerr << "Failed to get heartbeat time" << std::endl;
				return 0;
			}
		}

		bool Camera_MVS::setExposureTime(size_t value)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			float exposureTime = static_cast<float>(value);
			auto result = MV_CC_SetExposureTime(m_cameraHandle, exposureTime);
			if (result == MV_OK)
			{
				return true;
			}

			std::cerr << "Failed to set exposuretime" << std::endl;
			return false;
		}

		bool Camera_MVS::setGain(size_t value)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			float gain = static_cast<float>(value);
			auto result = MV_CC_SetGain(m_cameraHandle, gain);
			if (result == MV_OK)
			{
				return true;
			}
			std::cerr << "Failed to set gain" << std::endl;
			return false;
		}

		bool Camera_MVS::setTriggerMode(CameraTriggerMode mode)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

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
				std::cerr << "Invalid trigger mode" << std::endl;
				return false;
			}

			if (MV_CC_SetTriggerMode(m_cameraHandle, modeValue) == MV_OK)
			{
				return true;
			}
			else
			{
				std::cerr << "Failed to set trigger mode" << std::endl;
				return false;
			}
		}

		bool Camera_MVS::setInTriggerLine(size_t lineIndex)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			unsigned int lineValue = static_cast <unsigned int> (lineIndex);
			if (MV_CC_SetTriggerSource(m_cameraHandle, lineValue) == MV_OK)
			{
				return true;
			}

			return false;
		}

		size_t Camera_MVS::getExposureTime(bool& isGet)
		{
			if (!m_cameraHandle)
			{
				isGet = false;
				return 0;
			}
			isGet = true;

			MVCC_FLOATVALUE exposureTime;
			memset(&exposureTime, 0, sizeof(MVCC_FLOATVALUE));
			auto result = MV_CC_GetExposureTime(m_cameraHandle, &exposureTime);
			if (result == MV_OK) {
				return static_cast<size_t>(exposureTime.fCurValue);
			}
			return false;
		}

		size_t Camera_MVS::getGain(bool& isGet)
		{
			if (!m_cameraHandle)
			{
				isGet = false;
				return 0;
			}
			isGet = true;

			MVCC_FLOATVALUE gain;
			memset(&gain, 0, sizeof(MVCC_FLOATVALUE));
			auto result = MV_CC_GetGain(m_cameraHandle, &gain);
			if (result == MV_OK) {
				return static_cast<size_t>(gain.fCurValue);
			}

			return false;
		}

		CameraTriggerMode Camera_MVS::getMonitorMode(bool& isGet)
		{
			isGet = true;
			return triggerMode;
		}

		size_t Camera_MVS::getTriggerLine(bool& isGet)
		{
			if (!m_cameraHandle)
			{
				isGet = false;
				return 0;
			}
			isGet = true;

			MVCC_ENUMVALUE stTriggerSource;
			if (MV_CC_GetTriggerSource(m_cameraHandle, &stTriggerSource) == MV_OK)
			{
				return static_cast<size_t>(stTriggerSource.nCurValue);
			}

			return false;
		}

		bool Camera_MVS::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			if (!config.strobeEnable)
			{
				MV_CC_SetBoolValue(m_cameraHandle, "StrobeEnable", config.strobeEnable);
				return false;
			}
			bool result = true;
			result = result && MV_OK == MV_CC_SetEnumValue(m_cameraHandle, "LineSelector", config.lineSelector);
			result = result && MV_OK == MV_CC_SetEnumValue(m_cameraHandle, "LineMode", config.lineMode);
			result = result && MV_OK == MV_CC_SetEnumValue(m_cameraHandle, "LineSource", config.lineSource);
			result = result && MV_OK == MV_CC_SetIntValue(m_cameraHandle, "StrobeLineDuration", config.durationValue);
			result = result && MV_OK == MV_CC_SetIntValue(m_cameraHandle, "StrobeLineDelay", config.delayValue);
			result = result && MV_OK == MV_CC_SetIntValue(m_cameraHandle, "StrobeLinePreDelay", config.preDelayValue);
			return result;
		}

		bool Camera_MVS::outTrigger()
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			auto result = MV_CC_SetCommandValue(m_cameraHandle, "LineTriggerSoftware");

			return result == MV_OK;
		}

		bool Camera_MVS::outTrigger(bool isOpen)
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			auto result = MV_CC_SetBoolValue(m_cameraHandle, "LineInverter", isOpen);

			return result == MV_OK;
		}

		bool Camera_MVS::disconnectCamera()
		{
			if (!m_cameraHandle) {
				return true;
			}

			if (_isMonitor) {
				MV_CC_StopGrabbing(m_cameraHandle);
				_isMonitor = false;
			}

			MV_CC_CloseDevice(m_cameraHandle);

			MV_CC_DestroyHandle(m_cameraHandle);
			m_cameraHandle = nullptr;

			return true;
		}

		bool Camera_MVS::setFrameTriggered(bool state)
		{
			return false;
		}

		bool Camera_MVS::getFrameTriggered(bool& isGet)
		{
			return false;
		}

		bool Camera_MVS::setLineTriggered(bool state)
		{
			return false;
		}

		bool Camera_MVS::getLineTriggered(bool& isGet)
		{
			return false;
		}

		bool Camera_MVS::setPreDivider(size_t number)
		{
			return false;
		}

		bool Camera_MVS::setMultiplier(size_t number)
		{
			return false;
		}

		bool Camera_MVS::setPostDivider(size_t number)
		{
			return false;
		}

		bool Camera_MVS::getEncoderNumber(double& number)
		{
			return false;
		}

		bool Camera_MVS::setLineHeight(size_t number)
		{
			return false;
		}

		size_t Camera_MVS::getLineHeight(bool& isGet)
		{
			isGet = false;
			return 0;
		}

		bool Camera_MVS::setIOOut(int line_index, bool state)
		{
			return false;
		}

		bool Camera_MVS::getIOIn(int line_index, bool& state)
		{
			return false;
		}

		bool Camera_MVS::getIOOut(int line_index, bool& state)
		{
			state = false;
			return false;
		}

		bool Camera_MVS::softwareTrigger()
		{
			int enumValue = MV_CC_SetEnumValue(m_cameraHandle, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
			if (enumValue != 0) {
				return false;
			}
			else {
			}
			int comdValue = MV_CC_SetCommandValue(m_cameraHandle, "TriggerSoftware");
			if (comdValue != 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		bool Camera_MVS::setTriggerState(bool state) const
		{
			if (!m_cameraHandle)
			{
				return false;
			}


			int result{ 0 };
			if (state)
			{
				result = MV_CC_SetTriggerMode(m_cameraHandle, 1);
			}
			else
			{
				result = MV_CC_SetTriggerMode(m_cameraHandle, 0);
			}
			return result == MV_OK;
		}

		bool Camera_MVS::setTriggerSource(TriggerSource triggerSource)
		{
			if (!m_cameraHandle)
			{
				return false;
			}
			unsigned int sourceValue;
			switch (triggerSource)
			{
			case TriggerSource::SoftwareTrigger:
				sourceValue = MV_TRIGGER_SOURCE_SOFTWARE;
				break;
			case TriggerSource::Line0:
				sourceValue = MV_TRIGGER_SOURCE_LINE0;
				break;
			case TriggerSource::Line1:
				sourceValue = MV_TRIGGER_SOURCE_LINE1;
				break;
			case TriggerSource::Line2:
				sourceValue = MV_TRIGGER_SOURCE_LINE2;
				break;
			case TriggerSource::Line3:
				sourceValue = MV_TRIGGER_SOURCE_LINE3;
				break;
			default:
				std::cerr << "Invalid trigger source" << std::endl;
				return false;
			}
			auto result = MV_CC_SetEnumValue(m_cameraHandle, "TriggerSource", sourceValue);
			return result == MV_OK;
		}

		bool Camera_MVS::encoderNumberReset()
		{
			return false;
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
				return cv::Mat();
			}
			cv::Mat image = ImageFrameConvert::MVS_ConvertFrameToMat(frameInfo);
			if (image.empty())
			{
				return cv::Mat();
			}
			auto freeResult = MV_CC_FreeImageBuffer(m_cameraHandle, &frameInfo);
			if (freeResult != MV_OK) {
				return cv::Mat();
			}
			return image;
		}

		Camera_MVS_Passive::Camera_MVS_Passive(UserToCallBack userToCallback)
			:_userToCallBack(userToCallback)
		{
		}

		Camera_MVS_Passive::~Camera_MVS_Passive()
		{
			if (_isMonitor)
			{
				this->Camera_MVS::stopMonitor();
				_isMonitor = false;
			}
		}

		bool Camera_MVS_Passive::RegisterCallBackFunc()
		{
			if (!m_cameraHandle)
			{
				return false;
			}

			auto result = MV_CC_RegisterImageCallBackEx(m_cameraHandle, Camera_MVS_Passive::ImageCallBackFunc, this);
			if (result != MV_OK) {
				return false;
			}
			return true;
		}

		void Camera_MVS_Passive::ImageCallBackFunc(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
		{
			Camera_MVS_Passive* pThis = static_cast<Camera_MVS_Passive*>(pUser);
			if (pFrameInfo)
			{
				MatInfo info;
				if (pThis->_userToCallBackPre)
				{
					pThis->_userToCallBackPre(info);
				}
				info.mat = ImageFrameConvert::MVS_ConvertFrameToMat(*pFrameInfo, pData);

				if (pThis) {
					pThis->_userToCallBack(std::move(info));
				}
			}
		}

		void Camera_MVS_Passive::setUserToCallBackPre(UserToCallBackPre userToCallBackPre)
		{
			_userToCallBackPre = userToCallBackPre;
		}
	}
}