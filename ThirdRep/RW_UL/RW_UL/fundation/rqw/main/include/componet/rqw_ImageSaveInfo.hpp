#pragma once

#include <QImage>
#include <QString>
#include <QDateTime>

namespace rw
{
	namespace rqw
	{
		enum class ImageSaveComPolicy
		{
			Normal,
			MaxSaveImageNum,
			SaveAllImg
		};

		enum class ImageSaveComFormat
		{
			JPEG,
			PNG,
			BMP
		};

		struct ImageSaveComInfo
		{
		public:
			QImage image;
		public:
			QString name;
			QString saveDirectoryPath{};
		public:
			ImageSaveComInfo(const QImage& image)
			{
				this->image = image;
				QDateTime currentTime = QDateTime::currentDateTime();
				this->name = currentTime.toString("yyyyMMddhhmmsszzz");
			}

			ImageSaveComInfo(QImage&& image)
			{
				this->image = std::move(image);
				QDateTime currentTime = QDateTime::currentDateTime();
				this->name = currentTime.toString("yyyyMMddhhmmsszzz");
			}
		};

		QString imageFormatToString(rw::rqw::ImageSaveComFormat format);
	}

}
