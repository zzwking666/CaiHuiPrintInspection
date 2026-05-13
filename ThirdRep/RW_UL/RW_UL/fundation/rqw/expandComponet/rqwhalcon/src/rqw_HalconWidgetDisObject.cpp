#include"rqw_HalconWidgetDisObject.hpp"

#include "halconcpp/HalconCpp.h"

namespace rw
{
	namespace rqw
	{
		HalconWidgetObject::HalconWidgetObject(HalconCpp::HObject* obj)
			:_object(obj)
		{
		}

		HalconWidgetObject::HalconWidgetObject(const HalconCpp::HImage& image)
			:type(HalconObjectType::Image)
		{
			HalconCpp::HImage* newImage = new HalconCpp::HImage(image);
			_object = newImage;
		}

		HalconWidgetObject::HalconWidgetObject(const cv::Mat& mat)
			:type(HalconObjectType::Image)
		{
			HalconCpp::HImage hImage = CvMatToHImage(mat);
			auto newImage = new HalconCpp::HImage(hImage);
			_object = newImage;
		}

		HalconWidgetObject::HalconWidgetObject(const QImage& image)
			:type(HalconObjectType::Image)
		{
			HalconCpp::HImage hImage = QImageToHImage(image);
			auto newImage = new HalconCpp::HImage(hImage);
			_object = newImage;
		}

		HalconWidgetObject::HalconWidgetObject(const QPixmap& pixmap)
			:type(HalconObjectType::Image)
		{
			QImage image = pixmap.toImage();
			HalconCpp::HImage hImage = QImageToHImage(image);
			auto newImage = new HalconCpp::HImage(hImage);
			_object = newImage;
		}

		HalconWidgetObject::~HalconWidgetObject()
		{
			if (_object)
			{
				delete _object;
			}
		}

		HalconWidgetObject::HalconWidgetObject(const HalconWidgetObject& other)
			: _object(other._object ? new HalconCpp::HObject(*other._object) : nullptr),
			id(other.id),
			descrption(other.descrption),
			isShow(other.isShow),
			type(other.type),
			painterConfig(other.painterConfig)
		{
		}

		HalconWidgetObject::HalconWidgetObject(HalconWidgetObject&& other) noexcept
			: _object(other._object),
			id(other.id),
			descrption(std::move(other.descrption)),
			isShow(other.isShow),
			type(other.type),
			painterConfig(other.painterConfig)
		{
			other._object = nullptr;
		}

		HalconWidgetObject& HalconWidgetObject::operator=(const HalconWidgetObject& other)
		{
			if (this != &other)
			{
				// 释放当前对象
				delete _object;

				// 深拷贝
				_object = other._object ? new HalconCpp::HObject(*other._object) : nullptr;
				id = other.id;
				descrption = other.descrption;
				isShow = other.isShow;
				type = other.type;
				painterConfig = other.painterConfig;
			}
			return *this;
		}

		HalconWidgetObject& HalconWidgetObject::operator=(HalconWidgetObject&& other) noexcept
		{
			if (this != &other)
			{
				// 释放当前对象
				delete _object;

				// 移动资源
				_object = other._object;
				id = other.id;
				descrption = std::move(other.descrption);
				isShow = other.isShow;
				type = other.type;
				painterConfig = other.painterConfig;

				// 清空源对象
				other._object = nullptr;
			}
			return *this;
		}

		bool HalconWidgetObject::has_value() const
		{
			return _object != nullptr && _object->IsInitialized();
		}

		HalconCpp::HObject* HalconWidgetObject::value() const
		{
			if (!has_value())
			{
				throw std::runtime_error("HalconWidgetDisObject does not contain a valid HObject.");
			}
			return _object;
		}

		void HalconWidgetObject::release()
		{
			if (_object)
			{
				delete _object;
				_object = nullptr;
				type = HalconObjectType::Undefined;
			}
		}

		void HalconWidgetObject::updateObject(const HalconCpp::HObject& object)
		{
			if (_object)
			{
				delete _object;
			}
			_object = new HalconCpp::HObject(object);
		}

		void HalconWidgetObject::updateObject(HalconCpp::HObject* object)
		{
			if (_object)
			{
				delete _object;
			}
			_object = object;
		}

		HalconWidgetImg::HalconWidgetImg(HalconCpp::HObject* obj)
			:HalconWidgetObject(obj)
		{
		}

		HalconWidgetImg::HalconWidgetImg(const HalconCpp::HImage& image)
			:HalconWidgetObject(image)
		{
		}

		HalconWidgetImg::HalconWidgetImg(const cv::Mat& mat)
			:HalconWidgetObject(mat)
		{
		}

		HalconWidgetImg::HalconWidgetImg(const QImage& image)
			:HalconWidgetObject(image)
		{
		}

		HalconWidgetImg::HalconWidgetImg(const QPixmap& pixmap)
			:HalconWidgetObject(pixmap)
		{
		}

		HalconWidgetImg::HalconWidgetImg(const HalconWidgetImg& other)
			:HalconWidgetObject(other)
		{
		}

		HalconWidgetImg::HalconWidgetImg(HalconWidgetImg&& other) noexcept
			:HalconWidgetObject(other)
		{
		}

		HalconWidgetImg& HalconWidgetImg::operator=(const HalconWidgetImg& other)
		{
			if (this != &other)
			{
				HalconWidgetObject::operator=(other);
			}
			return *this;
		}

		HalconWidgetImg& HalconWidgetImg::operator=(HalconWidgetImg&& other) noexcept
		{
			if (this != &other)
			{
				HalconWidgetObject::operator=(std::move(other));
			}
			return *this;
		}

		HalconWidgetTemplateResult::HalconWidgetTemplateResult(HalconCpp::HObject* obj)
			:HalconWidgetObject(obj)
		{
			type = HalconObjectType::TemplateResult;
		}

		HalconWidgetTemplateResult::HalconWidgetTemplateResult(const HalconWidgetTemplateResult& other)
			:HalconWidgetObject(other), score(other.score), row(other.row), column(other.column), angle(other.angle)
		{
		}

		HalconWidgetTemplateResult::HalconWidgetTemplateResult(HalconWidgetTemplateResult&& other) noexcept
			:score(other.score), row(other.row), column(other.column), angle(other.angle), HalconWidgetObject(other)
		{
		}

		HalconWidgetTemplateResult& HalconWidgetTemplateResult::operator=(const HalconWidgetTemplateResult& other)
		{
			if (this != &other)
			{
				HalconWidgetObject::operator=(other);
				score = other.score;
				row = other.row;
				column = other.column;
				angle = other.angle;
			}
			return *this;
		}

		HalconWidgetTemplateResult& HalconWidgetTemplateResult::operator=(HalconWidgetTemplateResult&& other) noexcept
		{
			if (this != &other)
			{
				score = other.score;
				row = other.row;
				column = other.column;
				angle = other.angle;
				HalconWidgetObject::operator=(std::move(other));
			}
			return *this;
		}
	}
}