#include"rqw_CameraObjectThreadZMotion.hpp"

#include"rqw_CameraObjectZMotion.hpp"
#include"hoec_CameraException.hpp"

namespace rw
{
	namespace rqw
	{
		CameraPassiveThreadZMotion::CameraPassiveThreadZMotion(QObject* parent)
			:QThread(parent), _cameraObject(nullptr)
		{
		}

		CameraPassiveThreadZMotion::~CameraPassiveThreadZMotion()
		{
			quit();
			wait();
			if (_cameraObject->getConnectState()) {
				if (_cameraObject) {
					delete _cameraObject;
				}
			}
			else {
				try
				{
					if (_cameraObject) {
						delete _cameraObject;
					}
				}
				catch (const std::exception&)
				{
				}
			}
		}

		void CameraPassiveThreadZMotion::initCamera(const rw::rqw::CameraMetaData& cameraMetaData,
			rw::rqw::CameraObjectTrigger triggerMode, size_t motionInde)
		{
			if (_cameraObject)
			{
				delete _cameraObject;
			}
			if (!_cameraObject)
			{
				_cameraObject = new rw::rqw::CameraPassiveObjectZMotion();
				connect(_cameraObject, &rw::rqw::CameraPassiveObjectZMotion::frameCaptured, this, &CameraPassiveThreadZMotion::onFrameCaptured, Qt::DirectConnection);
				_cameraObject->motionInde = motionInde;
				_cameraObject->initCamera(cameraMetaData, triggerMode);
			}
		}

		bool CameraPassiveThreadZMotion::getConnectState()
		{
			return _cameraObject->getConnectState();
		}

		void CameraPassiveThreadZMotion::startMonitor()
		{
			if (!this->isRunning())
			{
				//this->start();
			}
			if (_cameraObject)
			{
				_cameraObject->startMonitor();
			}
		}

		void CameraPassiveThreadZMotion::stopMonitor()
		{
			if (_cameraObject)
			{
				_cameraObject->stopMonitor();
				delete _cameraObject;
				_cameraObject = nullptr;
			}
		}

		void CameraPassiveThreadZMotion::setHeartbeatTime(size_t value) const
		{
			if (_cameraObject)
			{
				_cameraObject->setHeartbeatTime(value);
			}
		}

		void CameraPassiveThreadZMotion::setFrameRate(float value) const
		{
			if (_cameraObject)
			{
				_cameraObject->setFrameRate(value);
			}
		}

		size_t CameraPassiveThreadZMotion::getHeartbeatTime() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getHeartbeatTime();
			}
			return 0;
		}

		float CameraPassiveThreadZMotion::getFrameRate() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getFrameRate();
			}
			return 0;
		}

		void CameraPassiveThreadZMotion::setExposureTime(size_t value) const
		{
			if (_cameraObject)
			{
				_cameraObject->setExposureTime(value);
			}
		}

		void CameraPassiveThreadZMotion::setGain(size_t value) const
		{
			if (_cameraObject)
			{
				_cameraObject->setGain(value);
			}
		}

		void CameraPassiveThreadZMotion::setTriggerMode(CameraObjectTrigger mode) const
		{
			if (_cameraObject)
			{
				_cameraObject->setTriggerMode(mode);
			}
		}

		void CameraPassiveThreadZMotion::setTriggerLine(size_t lineIndex) const
		{
			if (_cameraObject)
			{
				_cameraObject->setTriggerLine(lineIndex);
			}
		}

		size_t CameraPassiveThreadZMotion::getExposureTime() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getExposureTime();
			}
			return 0;
		}

		size_t CameraPassiveThreadZMotion::getGain() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getGain();
			}
			return 0;
		}

		CameraObjectTrigger CameraPassiveThreadZMotion::getMonitorMode() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getMonitorMode();
			}
			return CameraObjectTrigger::Undefined;
		}

		size_t CameraPassiveThreadZMotion::getTriggerLine() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getTriggerLine();
			}
			return 0;
		}

		void CameraPassiveThreadZMotion::run()
		{
			exec();
		}

		void CameraPassiveThreadZMotion::onFrameCaptured(cv::Mat frame, float location)
		{
			emit frameCaptured(std::move(frame), location, cameraIndex);
		}
	} // namespace rqw
} // namespace rw