#pragma once

#include <any>
#include <string>
#include<functional>
#include"opencv2/opencv.hpp"

namespace rw
{
	namespace hoec_v1
	{
		struct MatInfo
		{
			cv::Mat mat;
			std::map<std::string, std::any> customField;

			// 默认构造
			MatInfo() = default;

			// 拷贝构造
			MatInfo(const MatInfo& other)
				: mat(other.mat.clone()), customField(other.customField) {
			}

			// 拷贝赋值
			MatInfo& operator=(const MatInfo& other)
			{
				if (this != &other) {
					mat = other.mat.clone();
					customField = other.customField;
				}
				return *this;
			}

			// 移动构造
			MatInfo(MatInfo&& other) noexcept
				: mat(std::move(other.mat)), customField(std::move(other.customField)) {
			}

			// 移动赋值
			MatInfo& operator=(MatInfo&& other) noexcept
			{
				if (this != &other) {
					mat = std::move(other.mat);
					customField = std::move(other.customField);
				}
				return *this;
			}
		};

		using UserToCallBackPre = std::function<void(MatInfo &)>;
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

		struct CameraInfo
		{
			std::string ip;
			std::string name;
			std::string mac;
		};

		enum class CameraTakePictureMode
		{
			Active,
			Passive
		};

		inline const char* to_string(CameraTakePictureMode e)
		{
			switch (e)
			{
			case CameraTakePictureMode::Active: return "Active";
			case CameraTakePictureMode::Passive: return "Passive";
			default: return "unknown";
			}
		}

		enum class CameraTriggerMode
		{
			SoftwareTriggered,
			HardwareTriggered,
		};

		enum TriggerSource
		{
			SoftwareTrigger = 0,
			Line0,
			Line1,
			Line2,
			Line3
		};

		inline const char* to_string(CameraTriggerMode e)
		{
			switch (e)
			{
			case CameraTriggerMode::SoftwareTriggered: return "SoftwareTriggered";
			case CameraTriggerMode::HardwareTriggered: return "HardwareTriggered";
			default: return "unknown";
			}
		}

		enum class CameraProvider
		{
			MVS,
			DS
		};

		inline CameraProvider from_string(const std::string& s)
		{
			if (s == "MVS")
			{
				return CameraProvider::MVS;
			}
			else
			{
				return CameraProvider::DS;
			}
		}

		inline const char* to_string(CameraProvider e)
		{
			switch (e)
			{
			case CameraProvider::MVS: return "MVS";
			case CameraProvider::DS: return "DS";
			default: return "unknown";
			}
		}
	}
}