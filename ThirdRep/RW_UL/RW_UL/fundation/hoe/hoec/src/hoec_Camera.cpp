#include"hoec_Camera.hpp"

#include"hoec_Camera_private.hpp"

namespace rw {
	namespace hoec {
		void ICamera::setIP(const std::string& ip)
		{
			_ip = ip;
		}

		std::string ICamera::getIP() const
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

		void CameraActive::connectCamera()
		{
			_camera->setIP(_ip);
			_camera->connectCamera();
			_cameraInfo = _camera->getCameraInfo();
		}

		bool CameraActive::getConnectState()
		{
			return _camera->getConnectState();
		}

		void CameraActive::setFrameRate(float cameraFrameRate)
		{
			return _camera->setFrameRate(cameraFrameRate);
		}

		void CameraActive::setHeartbeatTime(size_t heartBeatTime)
		{
			return _camera->setHeartbeatTime(heartBeatTime);
		}

		float CameraActive::getHeartbeatTime()
		{
			return _camera->getHeartbeatTime();
		}

		float CameraActive::getFrameRate()
		{
			return _camera->getFrameRate();
		}

		void CameraActive::startMonitor()
		{
			_camera->startMonitor();
		}

		void CameraActive::stopMonitor()
		{
			_camera->stopMonitor();
		}

		void CameraActive::setExposureTime(size_t value)
		{
			_camera->setExposureTime(value);
		}

		void CameraActive::setGain(size_t value)
		{
			_camera->setGain(value);
		}

		void CameraActive::setTriggerMode(CameraTriggerMode mode)
		{
			_camera->setTriggerMode(mode);
		}

		void CameraActive::setInTriggerLine(size_t lineIndex)
		{
			_camera->setInTriggerLine(lineIndex);
		}

		size_t CameraActive::getExposureTime()
		{
			return _camera->getExposureTime();
		}

		size_t CameraActive::getGain()
		{
			return _camera->getGain();
		}

		CameraTriggerMode CameraActive::getMonitorMode()
		{
			return _camera->getMonitorMode();
		}

		size_t CameraActive::getTriggerLine()
		{
			return _camera->getTriggerLine();
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

		void CameraActive::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			_camera->setOutTriggerConfig(config);
		}

		void CameraActive::outTrigger()
		{
			_camera->outTrigger();
		}

		void CameraActive::outTrigger(bool isOpen)
		{
			_camera->outTrigger(isOpen);
		}

		void CameraPassive::connectCamera()
		{
			_camera->setIP(_ip);
			_camera->connectCamera();
			_cameraInfo = _camera->getCameraInfo();
		}

		bool CameraPassive::getConnectState()
		{
			return _camera->getConnectState();
		}

		void CameraPassive::setFrameRate(float cameraFrameRate)
		{
			_camera->setFrameRate(cameraFrameRate);
		}

		void CameraPassive::setHeartbeatTime(size_t heartBeatTime)
		{
			_camera->setHeartbeatTime(heartBeatTime);
		}

		float CameraPassive::getHeartbeatTime()
		{
			return _camera->getHeartbeatTime();
		}

		float CameraPassive::getFrameRate()
		{
			return _camera->getFrameRate();
		}

		void CameraPassive::startMonitor()
		{
			_camera->startMonitor();
		}

		void CameraPassive::stopMonitor()
		{
			_camera->stopMonitor();
		}

		void CameraPassive::setExposureTime(size_t value)
		{
			_camera->setExposureTime(value);
		}

		void CameraPassive::setGain(size_t value)
		{
			_camera->setGain(value);
		}

		void CameraPassive::setTriggerMode(CameraTriggerMode mode)
		{
			_camera->setTriggerMode(mode);
		}

		void CameraPassive::setInTriggerLine(size_t lineIndex)
		{
			_camera->setInTriggerLine(lineIndex);
		}

		size_t CameraPassive::getExposureTime()
		{
			return _camera->getExposureTime();
		}

		size_t CameraPassive::getGain()
		{
			return _camera->getGain();
		}

		CameraTriggerMode CameraPassive::getMonitorMode()
		{
			return _camera->getMonitorMode();
		}

		size_t CameraPassive::getTriggerLine()
		{
			return _camera->getTriggerLine();
		}

		void CameraPassive::RegisterCallBackFunc()
		{
			_cameraPassive->RegisterCallBackFunc();
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

		CameraPassive::~CameraPassive()
		{
			if (_camera)
			{
				delete _camera;
				_camera = nullptr;
			}
			_cameraPassive = nullptr;
		}

		void CameraPassive::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			_camera->setOutTriggerConfig(config);
		}

		void CameraPassive::outTrigger()
		{
			_camera->outTrigger();
		}

		void CameraPassive::outTrigger(bool isOpen)
		{
			_camera->outTrigger(isOpen);
		}
	}
}