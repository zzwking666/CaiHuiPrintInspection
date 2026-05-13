#include"hoec_Camera_v1.hpp"

#include"opencv2/opencv.hpp"

namespace rw
{
	namespace hoec_v1
	{
		void ICamera::setIP(const std::string& ip)
		{
			_ip = ip;
		}

		std::string ICamera::getIP(bool& isGet) const
		{
			return _ip;
		}

		CameraInfo ICamera::getCameraInfo()
		{
			return _cameraInfo;
		}

		void ICamera::setCameraInfo(const CameraInfo& cameraInfo)
		{
			_cameraInfo = cameraInfo;
		}

		bool CameraActive::connectCamera()
		{
			_camera->setIP(_ip);
			return _camera->connectCamera();
		}

		bool CameraActive::getConnectState(bool& isGet)
		{
			return _camera->getConnectState(isGet);
		}

		bool CameraActive::setFrameRate(float cameraFrameRate)
		{
			return _camera->setFrameRate(cameraFrameRate);
		}

		bool CameraActive::setHeartbeatTime(size_t heartBeatTime)
		{
			return _camera->setHeartbeatTime(heartBeatTime);
		}

		size_t CameraActive::getHeartbeatTime(bool& isGet)
		{
			return _camera->getHeartbeatTime(isGet);
		}

		float CameraActive::getFrameRate(bool& isGet)
		{
			return _camera->getFrameRate(isGet);
		}

		bool CameraActive::startMonitor()
		{
			return _camera->startMonitor();
		}

		bool CameraActive::stopMonitor()
		{
			return _camera->stopMonitor();
		}

		bool CameraActive::setExposureTime(size_t value)
		{
			return _camera->setExposureTime(value);
		}

		bool CameraActive::setGain(size_t value)
		{
			return _camera->setGain(value);
		}

		bool CameraActive::setTriggerMode(CameraTriggerMode mode)
		{
			return _camera->setTriggerMode(mode);
		}

		bool CameraActive::setInTriggerLine(size_t lineIndex)
		{
			return _camera->setInTriggerLine(lineIndex);
		}

		size_t CameraActive::getExposureTime(bool& isGet)
		{
			return _camera->getExposureTime(isGet);
		}

		size_t CameraActive::getGain(bool& isGet)
		{
			return _camera->getGain(isGet);
		}

		CameraTriggerMode CameraActive::getMonitorMode(bool& isGet)
		{
			return _camera->getMonitorMode(isGet);
		}

		size_t CameraActive::getTriggerLine(bool& isGet)
		{
			return _camera->getTriggerLine(isGet);
		}

		cv::Mat CameraActive::getImage(bool& isGet)
		{
			return _cameraActive->getImage(isGet);
		}

		cv::Mat CameraActive::getImage()
		{
			return _cameraActive->getImage();
		}

		void CameraActive::setCameraProvider(CameraProvider provider)
		{
			_provider = provider;
		}

		CameraProvider CameraActive::getCameraProvider() const
		{
			return _provider;
		}

		CameraActive::CameraActive(ICamera* camera, ICameraActive* cameraActive)
		{
			_camera = camera;
			_cameraActive = cameraActive;
		}

		CameraActive::~CameraActive()
		{
			if (_camera)
			{
				delete _camera;
				_camera = nullptr;
			}
			_cameraActive = nullptr;
		}

		bool CameraActive::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			return _camera->setOutTriggerConfig(config);
		}

		bool CameraActive::outTrigger()
		{
			return _camera->outTrigger();
		}

		bool CameraActive::outTrigger(bool isOpen)
		{
			return _camera->outTrigger();
		}

		bool CameraActive::disconnectCamera()
		{
			return _camera->disconnectCamera();
		}

		bool CameraActive::setFrameTriggered(bool state)
		{
			return _camera->setFrameTriggered(state);
		}

		bool CameraActive::getFrameTriggered(bool& isGet)
		{
			return _camera->getFrameTriggered(isGet);
		}

		bool CameraActive::setLineTriggered(bool state)
		{
			return _camera->setLineTriggered(state);
		}

		bool CameraActive::getLineTriggered(bool& isGet)
		{
			return _camera->getLineTriggered(isGet);
		}

		bool CameraActive::setPreDivider(size_t number)
		{
			return _camera->setPreDivider(number);
		}

		bool CameraActive::setMultiplier(size_t number)
		{
			return _camera->setMultiplier(number);
		}

		bool CameraActive::setPostDivider(size_t number)
		{
			return _camera->setPostDivider(number);
		}

		bool CameraActive::getEncoderNumber(double& number)
		{
			return _camera->getEncoderNumber(number);
		}

		bool CameraActive::setLineHeight(size_t number)
		{
			return _camera->setLineHeight(number);
		}

		size_t CameraActive::getLineHeight(bool& isGet)
		{
			return _camera->getLineHeight(isGet);
		}

		bool CameraActive::setIOOut(int line_index, bool state)
		{
			return _camera->setIOOut(line_index, state);
		}

		bool CameraActive::getIOIn(int line_index, bool& state)
		{
			return _camera->getIOIn(line_index, state);
		}

		bool CameraActive::getIOOut(int line_index, bool& state)
		{
			return _camera->getIOOut(line_index, state);
		}

		bool CameraActive::softwareTrigger()
		{
			return _camera->softwareTrigger();
		}

		bool CameraActive::setTriggerState(bool state) const
		{
			return _camera->setTriggerState(state);
		}

		bool CameraActive::setTriggerSource(TriggerSource triggerSource)
		{
			return _camera->setTriggerSource(triggerSource);
		}

		bool CameraActive::encoderNumberReset()
		{
			return _camera->encoderNumberReset();
		}

		bool CameraPassive::connectCamera()
		{
			_camera->setIP(_ip);
			_cameraInfo = _camera->getCameraInfo();
			return _camera->connectCamera();
		}

		bool CameraPassive::getConnectState(bool& isGet)
		{
			return _camera->getConnectState(isGet);
		}

		bool CameraPassive::setFrameRate(float cameraFrameRate)
		{
			return _camera->setFrameRate(cameraFrameRate);
		}

		bool CameraPassive::setHeartbeatTime(size_t heartBeatTime)
		{
			return _camera->setHeartbeatTime(heartBeatTime);
		}

		size_t CameraPassive::getHeartbeatTime(bool& isGet)
		{
			return _camera->getHeartbeatTime(isGet);
		}

		float CameraPassive::getFrameRate(bool& isGet)
		{
			return _camera->getFrameRate(isGet);
		}

		bool CameraPassive::startMonitor()
		{
			return _camera->startMonitor();
		}

		bool CameraPassive::stopMonitor()
		{
			return _camera->stopMonitor();
		}

		bool CameraPassive::setExposureTime(size_t value)
		{
			return _camera->setExposureTime(value);
		}

		bool CameraPassive::setGain(size_t value)
		{
			return _camera->setGain(value);
		}

		bool CameraPassive::setTriggerMode(CameraTriggerMode mode)
		{
			return _camera->setTriggerMode(mode);;
		}

		bool CameraPassive::setInTriggerLine(size_t lineIndex)
		{
			return _camera->setInTriggerLine(lineIndex);
		}

		size_t CameraPassive::getExposureTime(bool& isGet)
		{
			return _camera->getExposureTime(isGet);
		}

		size_t CameraPassive::getGain(bool& isGet)
		{
			return _camera->getGain(isGet);
		}

		CameraTriggerMode CameraPassive::getMonitorMode(bool& isGet)
		{
			return _camera->getMonitorMode(isGet);
		}

		size_t CameraPassive::getTriggerLine(bool& isGet)
		{
			return _camera->getTriggerLine(isGet);
		}

		bool CameraPassive::RegisterCallBackFunc()
		{
			return _cameraPassive->RegisterCallBackFunc();
		}

		void CameraPassive::setCameraProvider(CameraProvider provider)
		{
			_provider = provider;
		}

		CameraProvider CameraPassive::getCameraProvider() const
		{
			return _provider;
		}

		CameraPassive::CameraPassive(ICamera* camera, ICameraPassive* cameraPassive, UserToCallBack userToCallBack)
		{
			_camera = camera;
			_cameraPassive = cameraPassive;
			_userToCallBack = userToCallBack;
		}

		CameraPassive::CameraPassive(ICamera* camera, ICameraPassive* cameraPassive, UserToCallBack userToCallBack,
			UserToCallBackPre userToCallBackPre)
		{
			_camera = camera;
			_cameraPassive = cameraPassive;
			_userToCallBack = userToCallBack;
			_userToCallBackPre = userToCallBackPre;
		}

		CameraPassive::~CameraPassive()
		{
			if (_camera)
			{
				delete _camera;
				_camera = nullptr;
			}
			_cameraPassive = nullptr;
		}

		bool CameraPassive::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			return _camera->setOutTriggerConfig(config);
		}

		bool CameraPassive::outTrigger()
		{
			return _camera->outTrigger();
		}

		bool CameraPassive::outTrigger(bool isOpen)
		{
			return _camera->outTrigger(isOpen);
		}

		bool CameraPassive::disconnectCamera()
		{
			return _camera->disconnectCamera();
		}

		bool CameraPassive::setFrameTriggered(bool state)
		{
			return _camera->setFrameTriggered(state);
		}

		bool CameraPassive::getFrameTriggered(bool& isGet)
		{
			return _camera->getFrameTriggered(isGet);
		}

		bool CameraPassive::setLineTriggered(bool state)
		{
			return _camera->setLineTriggered(state);
		}

		bool CameraPassive::getLineTriggered(bool& isGet)
		{
			return _camera->getLineTriggered(isGet);
		}

		bool CameraPassive::setPreDivider(size_t number)
		{
			return _camera->setPreDivider(number);
		}

		bool CameraPassive::setMultiplier(size_t number)
		{
			return _camera->setMultiplier(number);
		}

		bool CameraPassive::setPostDivider(size_t number)
		{
			return _camera->setPostDivider(number);
		}

		bool CameraPassive::getEncoderNumber(double& number)
		{
			return _camera->getEncoderNumber(number);
		}

		bool CameraPassive::setLineHeight(size_t number)
		{
			return _camera->setLineHeight(number);
		}

		size_t CameraPassive::getLineHeight(bool& isGet)
		{
			return _camera->getLineHeight(isGet);
		}

		bool CameraPassive::setIOOut(int line_index, bool state)
		{
			return _camera->setIOOut(line_index, state);
		}

		bool CameraPassive::getIOIn(int line_index, bool& state)
		{
			return _camera->getIOIn(line_index, state);
		}

		bool CameraPassive::getIOOut(int line_index, bool& state)
		{
			return _camera->getIOOut(line_index, state);
		}

		bool CameraPassive::softwareTrigger()
		{
			return _camera->softwareTrigger();
		}

		bool CameraPassive::setTriggerState(bool state) const
		{
			return _camera->setTriggerState(state);
		}

		bool CameraPassive::setTriggerSource(TriggerSource triggerSource)
		{
			return _camera->setTriggerSource(triggerSource);
		}

		bool CameraPassive::encoderNumberReset()
		{
			return _camera->encoderNumberReset();
		}

		void CameraPassive::setUserToCallBackPre(UserToCallBackPre userToCallBackPre)
		{
			_cameraPassive->setUserToCallBackPre(userToCallBackPre);
		}
	}
}