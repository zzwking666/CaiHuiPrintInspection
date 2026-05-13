#include"hoec_CameraFactory_v1.hpp"

#include "hoec_Camera_DS_v1_private.hpp"
#include"hoec_Camera_MVS_v1_private.hpp"

namespace rw
{
	namespace hoec_v1
	{
		std::vector<CameraIP> CameraFactory::checkAllCamera(CameraProvider provider)
		{
			std::vector<CameraIP> cameraIPs;

			if (provider == CameraProvider::MVS)
			{
				auto cameraList = Camera_MVS::getCameraIpList();
				for (auto& ip : cameraList)
				{
					CameraIP cameraIP;
					cameraIP.ip = ip;
					cameraIP.provider = CameraProvider::MVS;
					cameraIPs.push_back(cameraIP);
				}
			}
			else if (provider == CameraProvider::DS)
			{
				auto cameraList = Camera_DS::getCameraIpList();
				for (auto& ip : cameraList)
				{
					CameraIP cameraIP;
					cameraIP.ip = ip;
					cameraIP.provider = CameraProvider::DS;
					cameraIPs.push_back(cameraIP);
				}
			}

			return cameraIPs;
		}

		std::unique_ptr<CameraActive> CameraFactory::CreateActiveCamera(CameraIP cameraIP,
			CameraTriggerMode triggerMode)
		{
			std::unique_ptr<CameraActive> result;
			if (cameraIP.provider == CameraProvider::MVS)
			{
				auto camera = new Camera_MVS_Active();
				result = std::make_unique<CameraActive>(camera, camera);
				result->setIP(cameraIP.ip);
				result->connectCamera();
				result->setTriggerMode(triggerMode);
			}
			else
			{
				return result;
			}

			return result;
		}

		std::unique_ptr<CameraPassive> CameraFactory::CreatePassiveCamera(CameraIP cameraIP,
			CameraTriggerMode triggerMode, CameraPassive::UserToCallBack userToCallBack)
		{
			std::unique_ptr<CameraPassive> result;
			if (cameraIP.provider == CameraProvider::MVS)
			{
				auto camera = new Camera_MVS_Passive(userToCallBack);
				result = std::make_unique<CameraPassive>(camera, camera, userToCallBack);
				result->setIP(cameraIP.ip);
				result->connectCamera();
				result->setTriggerMode(triggerMode);
			}
			else
			{
				return result;
			}

			return result;
		}

		std::unique_ptr<CameraPassive> CameraFactory::CreatePassiveCamera(CameraIP cameraIP,
			CameraTriggerMode triggerMode, CameraPassive::UserToCallBack userToCallBack,
			UserToCallBackPre userToCallBackPre)
		{
			std::unique_ptr<CameraPassive> result;
			if (cameraIP.provider == CameraProvider::MVS)
			{
				auto camera = new Camera_MVS_Passive(userToCallBack);
				result = std::make_unique<CameraPassive>(camera, camera, userToCallBack, userToCallBackPre);
				result->setIP(cameraIP.ip);
				result->connectCamera();
				result->setTriggerMode(triggerMode);
			}
			else
			{
				return result;
			}

			return result;
		}

		std::unique_ptr<CameraActive> CameraFactory::CreateActiveCameraDS(CameraIP cameraIP)
		{
			std::unique_ptr<CameraActive> result;
			if (cameraIP.provider == CameraProvider::DS)
			{
				auto camera = new Camera_DS_Active();
				result = std::make_unique<CameraActive>(camera, camera);
				result->setIP(cameraIP.ip);
				result->connectCamera();
			}
			else
			{
				return result;
			}

			return result;
		}

		std::unique_ptr<CameraPassive> CameraFactory::CreatePassiveCameraDS(CameraIP cameraIP,
			CameraPassive::UserToCallBack userToCallBack)
		{
			std::unique_ptr<CameraPassive> result;
			if (cameraIP.provider == CameraProvider::DS)
			{
				auto camera = new Camera_DS_Passive(userToCallBack);
				result = std::make_unique<CameraPassive>(camera, camera, userToCallBack);
				result->setIP(cameraIP.ip);
				result->connectCamera();
			}
			else
			{
				return result;
			}

			return result;
		}
	}
}