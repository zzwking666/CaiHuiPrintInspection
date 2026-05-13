#pragma once
#include <memory>
#include <string>

#include"hoec_Camera_utilty_v1.hpp"
#include"hoec_Camera_v1.hpp"

namespace rw
{
	namespace hoec_v1
	{
		struct CameraIP
		{
			std::string ip;
			rw::hoec_v1::CameraProvider provider;
		};

		class CameraFactory {
		public:
			static std::vector<CameraIP> checkAllCamera(CameraProvider provider = CameraProvider::MVS);
			static std::unique_ptr<CameraActive> CreateActiveCamera(CameraIP cameraIP, CameraTriggerMode triggerMode);
			static std::unique_ptr<CameraPassive> CreatePassiveCamera(CameraIP cameraIP, CameraTriggerMode triggerMode, CameraPassive::UserToCallBack userToCallBack);
			static std::unique_ptr<CameraPassive> CreatePassiveCamera(CameraIP cameraIP, CameraTriggerMode triggerMode, CameraPassive::UserToCallBack userToCallBack, UserToCallBackPre userToCallBackPre);
			static std::unique_ptr<CameraActive> CreateActiveCameraDS(CameraIP cameraIP);
			static std::unique_ptr<CameraPassive> CreatePassiveCameraDS(CameraIP cameraIP, CameraPassive::UserToCallBack userToCallBack);
		};
	}
}