#include "rqw_CameraObject.hpp"

#include"hoec_CameraFactory_v1.hpp"
#include"hoec_Camera_v1.hpp"

namespace rw
{
	namespace rqw
	{
		CameraPassiveObject::CameraPassiveObject(QObject* parent)
		{
		}

		CameraPassiveObject::~CameraPassiveObject()
			= default;

		bool CameraPassiveObject::startMonitor() const
		{
			return _cameraPassive->startMonitor();
		}

		bool CameraPassiveObject::stopMonitor() const
		{
			return _cameraPassive->stopMonitor();
		}

		bool CameraPassiveObject::setHeartbeatTime(size_t value) const
		{
			return _cameraPassive->setHeartbeatTime(value);
		}

		bool CameraPassiveObject::setFrameRate(float value) const
		{
			return _cameraPassive->setFrameRate(value);
		}

		size_t CameraPassiveObject::getHeartbeatTime(bool& isGet) const
		{
			return _cameraPassive->getHeartbeatTime(isGet);
		}

		float CameraPassiveObject::getFrameRate(bool& isGet) const
		{
			return _cameraPassive->getFrameRate(isGet);
		}

		bool CameraPassiveObject::setExposureTime(size_t value) const
		{
			return _cameraPassive->setExposureTime(value);
		}

		bool CameraPassiveObject::setGain(size_t value) const
		{
			return _cameraPassive->setGain(value);
		}

		bool CameraPassiveObject::setTriggerMode(CameraObjectTrigger mode) const
		{
			hoec_v1::CameraTriggerMode hoecTrigger;
			if (mode == CameraObjectTrigger::Hardware)
			{
				hoecTrigger = hoec_v1::CameraTriggerMode::HardwareTriggered;
			}
			else
			{
				hoecTrigger = hoec_v1::CameraTriggerMode::SoftwareTriggered;
			}
			return _cameraPassive->setTriggerMode(hoecTrigger);
		}

		bool CameraPassiveObject::setTriggerLine(size_t lineIndex) const
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setInTriggerLine(lineIndex);
			}
			return false;
		}

		bool CameraPassiveObject::setLineHeight(size_t number)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setLineHeight(number);
			}
			return false;
		}

		size_t CameraPassiveObject::getLineHeight(bool& isGet)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getLineHeight(isGet);
			}
			isGet = false;
			return 0;
		}

		bool CameraPassiveObject::setFrameTriggered(bool state)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setFrameTriggered(state);
			}
			return false;
		}

		bool CameraPassiveObject::getFrameTriggered(bool& isGet)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getFrameTriggered(isGet);
			}
			isGet = false;
			return false;
		}

		bool CameraPassiveObject::setLineTriggered(bool state)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setLineTriggered(state);
			}
			return false;
		}

		bool CameraPassiveObject::getLineTriggered(bool& isGet)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getLineTriggered(isGet);
			}
			isGet = false;
			return false;
		}

		bool CameraPassiveObject::setPreDivider(size_t number)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setPreDivider(number);
			}
			return false;
		}

		bool CameraPassiveObject::setMultiplier(size_t number)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setMultiplier(number);
			}
			return false;
		}

		bool CameraPassiveObject::setPostDivider(size_t number)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setPostDivider(number);
			}
			return false;
		}

		bool CameraPassiveObject::getEncoderNumber(double& number)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getEncoderNumber(number);
			}
			return false;
		}

		bool CameraPassiveObject::setIOOut(int line_index, bool state)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setIOOut(line_index, state);
			}
			return false;
		}

		bool CameraPassiveObject::getIOIn(int line_index, bool& state)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getIOIn(line_index, state);
			}
			return false;
		}

		bool CameraPassiveObject::getIOOut(int line_index, bool& state)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getIOOut(line_index, state);
			}
			return false;
		}

		bool CameraPassiveObject::encoderNumberReset()
		{
			if (_cameraPassive)
			{
				return _cameraPassive->encoderNumberReset();
			}
			return false;
		}

		bool CameraPassiveObject::softwareTrigger()
		{
			if (_cameraPassive)
			{
				return _cameraPassive->softwareTrigger();
			}
			return false;
		}

		bool CameraPassiveObject::setTriggerState(bool state) const
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setTriggerState(state);
			}
			return false;
		}

		bool CameraPassiveObject::setTriggerSource(TriggerSource triggerSource)
		{
			if (_cameraPassive)
			{
				return _cameraPassive->setTriggerSource(triggerSource);
			}
			return false;
		}

		size_t CameraPassiveObject::getExposureTime(bool& isGet) const
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getExposureTime(isGet);
			}
			isGet = false;
			return 0;
		}

		size_t CameraPassiveObject::getGain(bool& isGet) const
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getGain(isGet);
			}
			isGet = false;
			return 0;
		}

		CameraObjectTrigger CameraPassiveObject::getMonitorMode(bool& isGet) const
		{
			hoec_v1::CameraTriggerMode hoecTrigger = _cameraPassive->getMonitorMode(isGet);
			if (hoecTrigger == hoec_v1::CameraTriggerMode::HardwareTriggered)
			{
				return CameraObjectTrigger::Hardware;
			}
			else
			{
				return CameraObjectTrigger::Software;
			}
		}

		size_t CameraPassiveObject::getTriggerLine(bool& isGet) const
		{
			if (_cameraPassive)
			{
				return _cameraPassive->getTriggerLine(isGet);
			}
			isGet = false;
			return 0;
		}

		size_t CameraPassiveObject::getHeartbeatTime() const
		{
			bool isGet{false};
			if (_cameraPassive)
			{
				return _cameraPassive->getHeartbeatTime(isGet);
			}
			return 0;
		}

		float CameraPassiveObject::getFrameRate() const
		{
			bool isGet{false};
			if (_cameraPassive)
			{
				return _cameraPassive->getFrameRate(isGet);
			}
			return 0.0f;
		}

		size_t CameraPassiveObject::getExposureTime() const
		{
			bool isGet{false};
			if (_cameraPassive)
			{
				return _cameraPassive->getExposureTime(isGet);
			}
			isGet = false;
			return 0;
		}

		size_t CameraPassiveObject::getGain() const
		{
			bool isGet{false};
			if (_cameraPassive)
			{
				return _cameraPassive->getGain(isGet);
			}
			isGet = false;
			return 0;
		}

		CameraObjectTrigger CameraPassiveObject::getMonitorMode() const
		{
			bool isGet = false;
			hoec_v1::CameraTriggerMode hoecTrigger = _cameraPassive->getMonitorMode(isGet);
			if (hoecTrigger == hoec_v1::CameraTriggerMode::HardwareTriggered)
			{
				return CameraObjectTrigger::Hardware;
			}
			else
			{
				return CameraObjectTrigger::Software;
			}
		}

		size_t CameraPassiveObject::getTriggerLine() const
		{
			bool isGet = false;
			return _cameraPassive->getTriggerLine(isGet);
		}

		void CameraPassiveObject::initCamera(const CameraMetaData& cameraMetaData, CameraObjectTrigger triggerMode)
		{
			_cameraMetaData = cameraMetaData;
			hoec_v1::CameraIP hoecCameraIp;
			hoecCameraIp.ip = cameraMetaData.ip.toStdString();
			hoecCameraIp.provider = hoec_v1::from_string(cameraMetaData.provider.toStdString());

			if (hoecCameraIp.provider == hoec_v1::CameraProvider::MVS)
			{
				hoec_v1::CameraTriggerMode hoecTrigger;
				if (triggerMode == CameraObjectTrigger::Hardware)
				{
					hoecTrigger = hoec_v1::CameraTriggerMode::HardwareTriggered;
				}
				else
				{
					hoecTrigger = hoec_v1::CameraTriggerMode::SoftwareTriggered;
				}

				_cameraPassive = hoec_v1::CameraFactory::CreatePassiveCamera(hoecCameraIp, hoecTrigger, [this](MatInfo mat)
					{
						emit frameCaptured(std::move(mat));
					});
			}
			else if (hoecCameraIp.provider == hoec_v1::CameraProvider::DS)
			{
				_cameraPassive = hoec_v1::CameraFactory::CreatePassiveCameraDS(hoecCameraIp, [this](MatInfo  mat)
					{
						emit frameCaptured(std::move(mat));
					});
			}

			if (callBackForImgReadyBefore)
			{
				_cameraPassive->setUserToCallBackPre(callBackForImgReadyBefore);
			}

			_cameraPassive->RegisterCallBackFunc();
		}
		bool CameraPassiveObject::getConnectState(bool& isGet)
		{
			if (_cameraPassive) {
				return _cameraPassive->getConnectState(isGet);
			}
			return false;
		}

		bool CameraPassiveObject::getConnectState()
		{
			if (_cameraPassive) {
				bool isGet = false;
				return _cameraPassive->getConnectState(isGet);
			}
			return false;
		}

		void CameraPassiveObject::setOutTriggerConfig(const OutTriggerConfig& config)
		{
			if (_cameraPassive) {
				rw::hoec_v1::OutTriggerConfig configHoec;
				configHoec.delayValue = config.delayValue;
				configHoec.strobeEnable = config.strobeEnable;
				configHoec.durationValue = config.durationValue;
				configHoec.lineMode = config.lineMode;
				configHoec.lineSelector = config.lineSelector;
				configHoec.lineSource = config.lineSource;
				configHoec.preDelayValue = config.preDelayValue;
				_cameraPassive->setOutTriggerConfig(configHoec);
			}
		}

		void CameraPassiveObject::outTrigger()
		{
			if (_cameraPassive) {
				_cameraPassive->outTrigger();
			}
		}

		void CameraPassiveObject::outTrigger(bool isOpen)
		{
			if (_cameraPassive) {
				_cameraPassive->outTrigger(isOpen);
			}
		}
	} // namespace rqw
} // namespace rw