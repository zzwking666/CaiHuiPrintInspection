#pragma once
#include <QImage>
#include <QString>
#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QDateTime>
#include <atomic>
#include <vector>
#include <map>

namespace rw {
	namespace rqw {
		enum class ImageSaveEnginePolicy
		{
			Normal,
			MaxSaveImageNum,
			SaveAllImg
		};

		enum class ImageSaveFormat
		{
			JPEG,
			PNG,
			BMP
		};

		QString imageFormatToString(rw::rqw::ImageSaveFormat format);

		struct ImageInfo
		{
		public:
			QImage image;
			QString classify;
			QString dirName{ "" }; //if empty, save to rootPath/classify else save to rootPath/dirName
		public:
			QString time;
		public:
			ImageInfo(const QImage& image)
			{
				this->image = image;
				QDateTime currentTime = QDateTime::currentDateTime();
				this->time = currentTime.toString("yyyyMMddhhmmsszzz"); // 年月日时分秒毫秒
			}
		};

		class ImageSaveEngine : public QThread {
			Q_OBJECT

		public:
			ImageSaveEngine(QObject* parent = nullptr, int threadCount = 4);

			~ImageSaveEngine();

			void setRootPath(const QString& rootPath);

			QString getRootPath();

			void pushImage(const ImageInfo& image);

			void stop();

			void startEngine();

			void setSavePolicy(ImageSaveEnginePolicy policy);

			void setMaxSaveImageNum(int maxNum);

		private:
			int saveImgQuality = 99;
		public:
			void setSaveImgQuality(int quality);
		private:
			ImageSaveFormat _saveImgFormat = ImageSaveFormat::JPEG;
		public:
			void setSaveImgFormat(ImageSaveFormat format);
		protected:
			void processImages();
		public:
			bool isAllImageSaved();
		private:
			void saveImage(const ImageInfo& image);

			QString rootPath;
			QQueue<ImageInfo> saveQueue;
			QMutex mutex;
			QWaitCondition condition;
			std::atomic<bool> stopFlag;

			const int maxQueueSize = 80; 
			const int batchSize = 20;     
			int threadCount;             
			std::vector<QThread*> workerThreads;

			ImageSaveEnginePolicy savePolicy = ImageSaveEnginePolicy::Normal; 
			int maxSaveImageNum = 50;
			std::map<QString, std::vector<QString>> savedImages; 
		};
	}
}