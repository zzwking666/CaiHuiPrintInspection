#include "rqw_ImageSaveCom.hpp"

#include <iostream>
#include <QDir>

namespace rw {
	namespace rqw {
		QString imageFormatToString(rw::rqw::ImageSaveComFormat format)
		{
			switch (format) {
			case ImageSaveComFormat::JPEG:
				return "jpg";
			case ImageSaveComFormat::PNG:
				return "png";
			case ImageSaveComFormat::BMP:
				return "bmp";
			default:
				return "jpg"; // 默认格式
			}
		}

		ImageSaveCom::ImageSaveCom(QObject* parent, int threadCount)
			: QObject(parent), destroyFlag(false), threadCount(threadCount) {
		}

		ImageSaveCom::~ImageSaveCom()
		{
			destroyCom();
		}

		void ImageSaveCom::pushImage(const ImageSaveComInfo& image)
		{
			if (image.saveDirectoryPath.isEmpty())
			{
				return;
			}

			QMutexLocker locker(&mutex);
			// 如果策略为 SaveAllImg，则不限制队列大小
			if (savePolicy != ImageSaveComPolicy::SaveAllImg && saveQueue.size() >= maxQueueSize) {
				std::cerr << "Queue is full, dropping image." << std::endl;
				return;
			}
			saveQueue.enqueue(image);
			condition.wakeOne();
		}

		void ImageSaveCom::destroyCom()
		{
			{
				QMutexLocker locker(&mutex);
				destroyFlag = true;
				condition.wakeAll();
			}
			for (auto& thread : workerThreads) {
				thread->wait();
				delete thread;
			}
			workerThreads.clear();
		}

		void ImageSaveCom::buildCom()
		{
			destroyFlag = false;
			for (int i = 0; i < threadCount; ++i) {
				QThread* worker = QThread::create([this]() { this->processImages(); });
				workerThreads.push_back(worker);
				worker->start();
			}
		}

		void ImageSaveCom::startCom()
		{
			QMutexLocker locker(&mutex);
			stopFlag = false;
			condition.wakeAll(); 
		}

		void ImageSaveCom::stopCom()
		{
			QMutexLocker locker(&mutex);
			stopFlag = true;
			condition.wakeAll(); 
		}

		void ImageSaveCom::setSaveImgFormat(const ImageSaveComFormat format)
		{
			_saveImgFormat = format;
		}

		void ImageSaveCom::processImages()
		{
			while (true) {
				QList<ImageSaveComInfo> tasks;

				{
					QMutexLocker locker(&mutex);
					if ((saveQueue.isEmpty() || stopFlag)&& !destroyFlag) {
						condition.wait(&mutex);
					}

					if (destroyFlag) {
						break;
					}

					while (!saveQueue.isEmpty() && tasks.size() < batchSize) {
						tasks.append(saveQueue.dequeue());
					}

					for (const auto& task : tasks) {
						saveImage(task);
					}
				}


			}
		}

		bool ImageSaveCom::isAllImageSaved()
		{
			QMutexLocker locker(&mutex);
			return saveQueue.isEmpty();
		}

		void ImageSaveCom::saveImage(const ImageSaveComInfo& image)
		{
			QDir dir(image.saveDirectoryPath + "/");
			if (!dir.exists()) {
				dir.mkpath(".");
			}

			// 获取格式字符串
			QString formatStr = imageFormatToString(_saveImgFormat);

			// 构造文件名
			QString fileName = dir.filePath( image.name + "." + formatStr);

			// 检查策略
			if (savePolicy == ImageSaveComPolicy::MaxSaveImageNum) {
				auto& imageList = savedImages[image.saveDirectoryPath];
				if (imageList.size() >= maxSaveImageNum) {
					QString oldestFile = imageList.front();
					imageList.erase(imageList.begin());
					QFile::remove(oldestFile);
				}
				imageList.push_back(fileName);
			}

			// 保存图片
			if (!image.image.save(fileName, formatStr.toUtf8().constData(), saveImgQuality)) {
				std::cerr << "Failed to save image: " << fileName.toStdString() << std::endl;
			}
		}

		void ImageSaveCom::setSavePolicy(ImageSaveComPolicy policy)
		{
			QMutexLocker locker(&mutex);
			savePolicy = policy;
		}

		void ImageSaveCom::setMaxSaveImageNum(int maxNum)
		{
			QMutexLocker locker(&mutex);
			maxSaveImageNum = maxNum;
		}

		void ImageSaveCom::setSaveImgQuality(int quality)
		{
			QMutexLocker locker(&mutex);
			saveImgQuality = quality;
		}
	}
}