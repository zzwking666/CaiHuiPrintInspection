#include "rqw_CameraObjectZMotion.hpp"

#include"hoec_CameraFactory.hpp"
#include"hoec_Camera.hpp"

namespace rw
{
	namespace rqw
	{
		CameraPassiveObjectZMotion::CameraPassiveObjectZMotion(QObject* parent)
		{
		}

		CameraPassiveObjectZMotion::~CameraPassiveObjectZMotion()
			= default;

		void CameraPassiveObjectZMotion::startMonitor() const
		{
			_cameraPassive->startMonitor();
		}

		void CameraPassiveObjectZMotion::stopMonitor() const
		{
			_cameraPassive->stopMonitor();
		}

		void CameraPassiveObjectZMotion::setHeartbeatTime(size_t value) const
		{
			_cameraPassive->setHeartbeatTime(value);
		}

		void CameraPassiveObjectZMotion::setFrameRate(float value) const
		{
			_cameraPassive->setFrameRate(value);
		}

		size_t CameraPassiveObjectZMotion::getHeartbeatTime() const
		{
			return _cameraPassive->getHeartbeatTime();
		}

		float CameraPassiveObjectZMotion::getFrameRate() const
		{
			return _cameraPassive->getFrameRate();
		}

		void CameraPassiveObjectZMotion::setExposureTime(size_t value) const
		{
			_cameraPassive->setExposureTime(value);
		}

		void CameraPassiveObjectZMotion::setGain(size_t value) const
		{
			_cameraPassive->setGain(value);
		}

		void CameraPassiveObjectZMotion::setTriggerMode(CameraObjectTrigger mode) const
		{
			hoec::CameraTriggerMode hoecTrigger;
			if (mode == CameraObjectTrigger::Hardware)
			{
				hoecTrigger = hoec::CameraTriggerMode::HardwareTriggered;
			}
			else
			{
				hoecTrigger = hoec::CameraTriggerMode::SoftwareTriggered;
			}
			_cameraPassive->setTriggerMode(hoecTrigger);
		}

		void CameraPassiveObjectZMotion::setTriggerLine(size_t lineIndex) const
		{
			_cameraPassive->setInTriggerLine(lineIndex);
		}

		size_t CameraPassiveObjectZMotion::getExposureTime() const
		{
			return _cameraPassive->getExposureTime();
		}

		size_t CameraPassiveObjectZMotion::getGain() const
		{
			return _cameraPassive->getGain();
		}

		CameraObjectTrigger CameraPassiveObjectZMotion::getMonitorMode() const
		{
			hoec::CameraTriggerMode hoecTrigger = _cameraPassive->getMonitorMode();
			if (hoecTrigger == hoec::CameraTriggerMode::HardwareTriggered)
			{
				return CameraObjectTrigger::Hardware;
			}
			else
			{
				return CameraObjectTrigger::Software;
			}
		}

		size_t CameraPassiveObjectZMotion::getTriggerLine() const
		{
			return _cameraPassive->getTriggerLine();
		}

		void CameraPassiveObjectZMotion::initCamera(const CameraMetaData& cameraMetaData, CameraObjectTrigger triggerMode)
		{
			_cameraMetaData = cameraMetaData;
			hoec::CameraIP hoecCameraIp;
			hoecCameraIp.ip = cameraMetaData.ip.toStdString();
			hoecCameraIp.provider = hoec::from_string(cameraMetaData.provider.toStdString());

			hoec::CameraTriggerMode hoecTrigger;
			if (triggerMode == CameraObjectTrigger::Hardware)
			{
				hoecTrigger = hoec::CameraTriggerMode::HardwareTriggered;
			}
			else
			{
				hoecTrigger = hoec::CameraTriggerMode::SoftwareTriggered;
			}

			_cameraPassive = hoec::CameraFactory::CreatePassiveCamera(hoecCameraIp, hoecTrigger, [this](cv::Mat  mat)
				{
					auto& cameraObject = zwy::scc::GlobalMotion::getInstance();
					float location = 0;
					cameraObject.motionPtr->GetModbus(motionInde, 1, location);

					emit frameCaptured(std::move(mat), location);
				});

			_cameraPassive->RegisterCallBackFunc();
		}
		bool CameraPassiveObjectZMotion::getConnectState()
		{
			if (_cameraPassive) {
				return _cameraPassive->getConnectState();
			}
			return false;
		}
	} // namespace rqw
} // namespace rw

zwy::scc::GlobalMotion::GlobalMotion()
{
	motionPtr = std::make_unique<Motion>();
}