#include"hoec_CameraFactory.hpp"

#include"hoec_Camera_MVS_private.hpp"

namespace rw {
	namespace hoec {
		std::vector<CameraIP> CameraFactory::checkAllCamera()
		{
			std::vector<CameraIP> cameraIPs;
			auto cameraList = Camera_MVS::getCameraIpList();
			for (auto& ip : cameraList)
			{
				CameraIP cameraIP;
				cameraIP.ip = ip;
				cameraIP.provider = CameraProvider::MVS;
				cameraIPs.push_back(cameraIP);
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
				throw CameraCreateError("The camera provider is not supported.");
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
				throw CameraCreateError("The camera provider is not supported.");
			}

			return result;
		}
	} // namespace sim
} // namespace rw