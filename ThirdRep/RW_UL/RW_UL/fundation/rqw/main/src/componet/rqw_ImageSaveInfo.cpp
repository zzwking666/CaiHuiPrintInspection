#include"rqw_ImageSaveInfo.hpp"


namespace rw
{
	namespace rqw
	{
		QString imageFormatToString(rw::rqw::ImageSaveComFormat format)
		{
			switch (format) {
			case rw::rqw::ImageSaveComFormat::JPEG:  return "jpg";
			case rw::rqw::ImageSaveComFormat::PNG:   return "png";
			case rw::rqw::ImageSaveComFormat::BMP:   return "bmp";
			default: return "jpg";
			}
		}
	}
}
