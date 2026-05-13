#pragma once

#include"rqw_ImageSaveInfo.hpp"


#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>

#include <atomic>
#include <vector>
#include <map>


namespace rw {
	namespace rqw {
		class ImageSaveCom : public QObject {
			Q_OBJECT

		public:
			ImageSaveCom(QObject* parent = nullptr, int threadCount = 4);

			~ImageSaveCom();

			void pushImage(const ImageSaveComInfo& image);

			void destroyCom();

			void buildCom();

			void startCom();

			void stopCom();

			void setSavePolicy(ImageSaveComPolicy policy);

			void setMaxSaveImageNum(int maxNum);

		private:
			int saveImgQuality = 99;
		public:
			void setSaveImgQuality(int quality);
		private:
			ImageSaveComFormat _saveImgFormat = ImageSaveComFormat::JPEG;
		public:
			void setSaveImgFormat(ImageSaveComFormat format);
		protected:
			void processImages();
		public:
			bool isAllImageSaved();
		private:
			void saveImage(const ImageSaveComInfo& image);

			QQueue<ImageSaveComInfo> saveQueue;
			QMutex mutex;
			QWaitCondition condition;
			std::atomic<bool> destroyFlag {false};
			std::atomic<bool> stopFlag{false};

			const int maxQueueSize = 80;
			const int batchSize = 20;
			int threadCount;
			std::vector<QThread*> workerThreads;

			ImageSaveComPolicy savePolicy = ImageSaveComPolicy::Normal;
			int maxSaveImageNum = 50;
			std::map<QString, std::vector<QString>> savedImages;
		};
	}
}