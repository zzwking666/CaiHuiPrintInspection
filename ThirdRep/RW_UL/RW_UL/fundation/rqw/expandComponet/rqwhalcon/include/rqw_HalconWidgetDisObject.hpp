#pragma once

#include"rqw_HalconUtilty.hpp"

namespace rw {
	namespace rqw {
		class HalconWidget;

		enum class HalconObjectType {
			Image,
			Region,
			Undefined,
			TemplateResult
		};

		struct HalconWidgetObject
		{
			friend HalconWidget;
		public:
			explicit HalconWidgetObject(HalconCpp::HObject* obj);
			explicit HalconWidgetObject(const HalconCpp::HImage& image);
			explicit HalconWidgetObject(const cv::Mat& mat);
			explicit HalconWidgetObject(const QImage& image);
			explicit HalconWidgetObject(const QPixmap& pixmap);

			~HalconWidgetObject();
		public:
			HalconWidgetObject(const HalconWidgetObject& other);
			HalconWidgetObject(HalconWidgetObject&& other) noexcept;
			HalconWidgetObject& operator=(const HalconWidgetObject& other);
			HalconWidgetObject& operator=(HalconWidgetObject&& other) noexcept;
		private:
			HalconCpp::HObject* _object;
		public:
			// Object properties :default id=0 for image, id>0 is other, < 0 is inside
			HalconWidgetDisObjectId id{ 0 };
			std::string descrption{ "Undefined" };
			bool isShow{ true };
			PainterConfig painterConfig;
			HalconObjectType type;
		public:
			bool has_value() const;
			HalconCpp::HObject* value() const;
		public:
			void release();
		public:
			void updateObject(const HalconCpp::HObject& object);
			void updateObject(HalconCpp::HObject* object);
		};

		struct HalconWidgetImg
			:public HalconWidgetObject
		{
		public:
			explicit HalconWidgetImg(HalconCpp::HObject* obj);
			explicit HalconWidgetImg(const HalconCpp::HImage& image);
			explicit HalconWidgetImg(const cv::Mat& mat);
			explicit HalconWidgetImg(const QImage& image);
			explicit HalconWidgetImg(const QPixmap& pixmap);
		public:
			HalconWidgetImg(const HalconWidgetImg& other);
			HalconWidgetImg(HalconWidgetImg&& other) noexcept;
			HalconWidgetImg& operator=(const HalconWidgetImg& other);
			HalconWidgetImg& operator=(HalconWidgetImg&& other) noexcept;
		public:
			~HalconWidgetImg() = default;
		};

		struct HalconWidgetTemplateResult
			:public HalconWidgetObject
		{
		public:
			explicit HalconWidgetTemplateResult(HalconCpp::HObject* obj);
		public:
			~HalconWidgetTemplateResult() = default;
		public:
			HalconWidgetTemplateResult(const HalconWidgetTemplateResult& other);
			HalconWidgetTemplateResult(HalconWidgetTemplateResult&& other) noexcept;
			HalconWidgetTemplateResult& operator=(const HalconWidgetTemplateResult& other);
			HalconWidgetTemplateResult& operator=(HalconWidgetTemplateResult&& other) noexcept;
		public:
			double score{ 0.0 };
			double row{ 0.0 };
			double column{ 0.0 };
			double angle{ 0.0 };
		};
	}
}