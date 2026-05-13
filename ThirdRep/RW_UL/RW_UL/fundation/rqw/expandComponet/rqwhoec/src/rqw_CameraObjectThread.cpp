#include"rqw_CameraObjectThread.hpp"

#include"rqw_CameraObject.hpp"

namespace rw
{
	namespace rqw
	{
		CameraPassiveThread::CameraPassiveThread(QObject* parent)
			:QThread(parent), _cameraObject(nullptr)
		{
		}

		CameraPassiveThread::~CameraPassiveThread()
		{
			quit();
			wait();
			if (_cameraObject->getConnectState()) {
				if (_cameraObject) {
					delete _cameraObject;
				}
			}
			else {
				if (_cameraObject) {
					delete _cameraObject;
				}
			}
		}

		void CameraPassiveThread::initCamera(const rw::rqw::CameraMetaData& cameraMetaData,
			rw::rqw::CameraObjectTrigger triggerMode)
		{
			if (_cameraObject)
			{
				delete _cameraObject;
			}
			if (!_cameraObject)
			{
				_cameraObject = new rw::rqw::CameraPassiveObject();
				if (callBackForImgReadyBefore)
				{
					_cameraObject->callBackForImgReadyBefore=callBackForImgReadyBefore;
				}
				connect(_cameraObject, &rw::rqw::CameraPassiveObject::frameCaptured, this, &CameraPassiveThread::onFrameCaptured, Qt::DirectConnection);
				_cameraObject->initCamera(cameraMetaData, triggerMode);
			}
		}

		bool CameraPassiveThread::getConnectState()
		{
			if (_cameraObject)
			{
				return _cameraObject->getConnectState();
			}
			return false;
		}

		bool CameraPassiveThread::startMonitor()
		{
			if (!this->isRunning())
			{
				//this->start();
			}
			if (_cameraObject)
			{
				return _cameraObject->startMonitor();
			}
		}

		bool CameraPassiveThread::stopMonitor()
		{
			if (_cameraObject)
			{
				return _cameraObject->stopMonitor();
				delete _cameraObject;
				_cameraObject = nullptr;
			}
		}

		bool CameraPassiveThread::setHeartbeatTime(size_t value) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setHeartbeatTime(value);
			}
		}

		bool CameraPassiveThread::setFrameRate(float value) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setFrameRate(value);
			}
			return false;
		}

		size_t CameraPassiveThread::getHeartbeatTime() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getHeartbeatTime();
			}
			return 0;
		}

		float CameraPassiveThread::getFrameRate() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getFrameRate();
			}
			return 0;
		}

		size_t CameraPassiveThread::getExposureTime(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getExposureTime(isGet);
			}
			return 0;
		}

		size_t CameraPassiveThread::getGain(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getGain(isGet);
			}
			return 0;
		}

		CameraObjectTrigger CameraPassiveThread::getMonitorMode(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getMonitorMode(isGet);
			}
			return CameraObjectTrigger::Undefined;
		}

		size_t CameraPassiveThread::getTriggerLine(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getTriggerLine(isGet);
			}
			return 0;
		}

		size_t CameraPassiveThread::getHeartbeatTime(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getHeartbeatTime(isGet);
			}
			return 0;
		}

		float CameraPassiveThread::getFrameRate(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getFrameRate(isGet);
			}
			return 0;
		}

		bool CameraPassiveThread::setExposureTime(size_t value) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setExposureTime(value);
			}
			return false;
		}

		bool CameraPassiveThread::setGain(size_t value) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setGain(value);
			}
			return false;
		}

		bool CameraPassiveThread::setTriggerMode(CameraObjectTrigger mode) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setTriggerMode(mode);
			}
			return false;
		}

		bool CameraPassiveThread::setTriggerLine(size_t lineIndex) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setTriggerLine(lineIndex);
			}
			return false;
		}

		bool CameraPassiveThread::setLineHeight(size_t number) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setLineHeight(number);
			}
			return false;
		}

		size_t CameraPassiveThread::getLineHeight(bool& isGet) const
		{
			if (_cameraObject)
			{
				return _cameraObject->getLineHeight(isGet);
			}
			return 0;
		}

		bool CameraPassiveThread::setFrameTriggered(bool state)
		{
			if (_cameraObject)
			{
				return _cameraObject->setFrameTriggered(state);
			}
			return false;
		}

		bool CameraPassiveThread::getFrameTriggered(bool& isGet)
		{
			if (_cameraObject)
			{
				return _cameraObject->getFrameTriggered(isGet);
			}
			return false;
		}

		bool CameraPassiveThread::setLineTriggered(bool state)
		{
			if (_cameraObject)
			{
				return _cameraObject->setLineTriggered(state);
			}
			return false;
		}

		bool CameraPassiveThread::getLineTriggered(bool& isGet)
		{
			if (_cameraObject)
			{
				return _cameraObject->getLineTriggered(isGet);
			}
			return false;
		}

		bool CameraPassiveThread::setPreDivider(size_t number)
		{
			if (_cameraObject)
			{
				return _cameraObject->setPreDivider(number);
			}
			return false;
		}

		bool CameraPassiveThread::setMultiplier(size_t number)
		{
			if (_cameraObject)
			{
				return _cameraObject->setMultiplier(number);
			}
			return false;
		}

		bool CameraPassiveThread::setPostDivider(size_t number)
		{
			if (_cameraObject)
			{
				return _cameraObject->setPostDivider(number);
			}
			return false;
		}

		bool CameraPassiveThread::getEncoderNumber(double& number)
		{
			if (_cameraObject)
			{
				return _cameraObject->getEncoderNumber(number);
			}
			return false;
		}

		bool CameraPassiveThread::setIOOut(int line_index, bool state)
		{
			if (_cameraObject)
			{
				return _cameraObject->setIOOut(line_index, state);
			}
			return false;
		}

		bool CameraPassiveThread::getIOIn(int line_index, bool& state)
		{
			if (_cameraObject)
			{
				return _cameraObject->getIOIn(line_index, state);
			}
			return false;
		}

		bool CameraPassiveThread::getIOOut(int line_index, bool& state)
		{
			if (_cameraObject)
			{
				return _cameraObject->getIOOut(line_index, state);
			}
			return false;
		}

		bool CameraPassiveThread::encoderNumberReset()
		{
			if (_cameraObject)
			{
				return _cameraObject->encoderNumberReset();
			}
			return false;
		}

		bool CameraPassiveThread::softwareTrigger()
		{
			if (_cameraObject)
			{
				return _cameraObject->softwareTrigger();
			}
			return false;
		}

		bool CameraPassiveThread::setTriggerState(bool state) const
		{
			if (_cameraObject)
			{
				return _cameraObject->setTriggerState(state);
			}
			return false;
		}

		bool CameraPassiveThread::setTriggerSource(TriggerSource triggerSource)
		{
			if (_cameraObject)
			{
				return _cameraObject->setTriggerSource(triggerSource);
			}
			return false;
		}

		size_t CameraPassiveThread::getExposureTime() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getExposureTime();
			}
			return 0;
		}

		size_t CameraPassiveThread::getGain() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getGain();
			}
			return 0;
		}

		CameraObjectTrigger CameraPassiveThread::getMonitorMode() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getMonitorMode();
			}
			return CameraObjectTrigger::Undefined;
		}

		size_t CameraPassiveThread::getTriggerLine() const
		{
			if (_cameraObject)
			{
				return _cameraObject->getTriggerLine();
			}
			return 0;
		}

		void CameraPassiveThread::run()
		{
			exec();
		}

		void CameraPassiveThread::onFrameCaptured(MatInfo frame)
		{
			emit frameCaptured(std::move(frame), cameraIndex);
		}

		bool CameraPassiveThread::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			if (_cameraObject)
			{
				_cameraObject->setOutTriggerConfig(config);
			}
			return false;
		}

		bool CameraPassiveThread::outTrigger()
		{
			if (_cameraObject)
			{
				_cameraObject->outTrigger();
			}
			return false;
		}

		bool CameraPassiveThread::outTrigger(bool isOpen)
		{
			if (_cameraObject)
			{
				_cameraObject->outTrigger(isOpen);
			}
			return false;
		}
	} // namespace rqw
} // namespace rw