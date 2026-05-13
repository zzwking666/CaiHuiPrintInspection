#pragma once

#include<QString>
#include<QVector>
#include <opencv2/core/mat.hpp>
#include<QImage>
#include"hoec_Camera_utilty_V1.hpp"

namespace rw {
	namespace hoec
	{
		class CameraPassive;
	}

	namespace rqw {
		QImage cvMatToQImage(const cv::Mat& mat);
		using TriggerSource = hoec_v1::TriggerSource;
		using MatInfo = hoec_v1::MatInfo;

		struct OutTriggerConfig
		{
		public:
			size_t lineSelector{ 0 };
			size_t lineMode{ 0 };
			size_t lineSource{ 0 };
			long durationValue{ 0 };
			size_t delayValue{ 0 };
			size_t preDelayValue{ 0 };
			bool strobeEnable = false;
		};

		using CallBackForImgReadyBefore = rw::hoec_v1::UserToCallBackPre;

		struct CameraMetaData
		{
			QString ip{ "Undefined" };
			QString provider{ "Undefined" };
		public:
			CameraMetaData() = default;
			CameraMetaData(QString ip, QString provider)
				:ip(std::move(ip)), provider(std::move(provider))
			{
			}

			CameraMetaData(const CameraMetaData& other)
				:ip(other.ip), provider(other.provider)
			{
			}

			CameraMetaData& operator=(const CameraMetaData& other)
			{
				ip = other.ip;
				provider = other.provider;
				return *this;
			}

			CameraMetaData(CameraMetaData&& other) noexcept
				:ip(std::move(other.ip)), provider(std::move(other.provider))
			{
			}

			CameraMetaData& operator=(CameraMetaData&& other) noexcept
			{
				ip = std::move(other.ip);
				provider = std::move(other.provider);
				return *this;
			}

			~CameraMetaData() = default;
		};

		enum class CameraObjectTrigger
		{
			Hardware,
			Software,
			Undefined
		};

		enum class CameraProvider
		{
			MVS,
			DS
		};

		QVector<CameraMetaData> CheckCameraList(CameraProvider provider = CameraProvider::MVS);
	} // namespace rqw
} // namespace rw