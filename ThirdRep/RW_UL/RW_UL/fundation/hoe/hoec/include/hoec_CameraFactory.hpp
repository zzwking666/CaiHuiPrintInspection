#pragma once
#include <string>

#include "hoec_Camera.hpp"

#include <vector>
#include <memory>

#include"hoec_CameraException.hpp"

namespace rw
{
	namespace hoec
	{
		struct CameraIP
		{
			std::string ip;
			hoec::CameraProvider provider;
		};

		class CameraFactory {
		public:
			static std::vector<CameraIP> checkAllCamera();
			static std::unique_ptr<CameraActive> CreateActiveCamera(CameraIP cameraIP, CameraTriggerMode triggerMode);
			static std::unique_ptr<CameraPassive> CreatePassiveCamera(CameraIP cameraIP, CameraTriggerMode triggerMode, CameraPassive::UserToCallBack userToCallBack);
		};
	} // namespace hoec
} // namespace rw