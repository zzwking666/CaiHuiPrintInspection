#include "rqw_ImageSaveEngine.h"

#include <iostream>
#include <QDir>

namespace rw {
	namespace rqw {
		QString imageFormatToString(rw::rqw::ImageSaveFormat format)
		{
			switch (format) {
			case rw::rqw::ImageSaveFormat::JPEG:  return "jpg";
			case rw::rqw::ImageSaveFormat::PNG:   return "png";
			case rw::rqw::ImageSaveFormat::BMP:   return "bmp";
			default: return "jpg";
			}
		}

		ImageSaveEngine::ImageSaveEngine(QObject* parent, int threadCount)
			: QThread(parent), stopFlag(false), threadCount(threadCount) {
		}

		ImageSaveEngine::~ImageSaveEngine()
		{
			stop();
		}

		void ImageSaveEngine::setRootPath(const QString& rootPath)
		{
			QMutexLocker locker(&mutex);
			this->rootPath = rootPath;
			QDir dir(rootPath);
			if (!dir.exists()) {
				dir.mkpath(".");
			}
		}

		QString ImageSaveEngine::getRootPath()
		{
			QMutexLocker locker(&mutex);
			if (rootPath.isEmpty()) {
				std::cerr << "Root path is not set." << std::endl;
				return QString();
			}
			return rootPath;
		}

		void ImageSaveEngine::pushImage(const ImageInfo& image)
		{
			QMutexLocker locker(&mutex);
			// 如果策略为 SaveAllImg，则不限制队列大小
			if (savePolicy != ImageSaveEnginePolicy::SaveAllImg && saveQueue.size() >= maxQueueSize) {
				std::cerr << "Queue is full, dropping image." << std::endl;
				return;
			}
			saveQueue.enqueue(image);
			condition.wakeOne();
		}

		void ImageSaveEngine::stop()
		{
			{
				QMutexLocker locker(&mutex);
				stopFlag = true;
				condition.wakeAll();
			}
			for (auto& thread : workerThreads) {
				thread->wait();
				delete thread;
			}
			workerThreads.clear();
		}

		void ImageSaveEngine::startEngine()
		{
			for (int i = 0; i < threadCount; ++i) {
				QThread* worker = QThread::create([this]() { this->processImages(); });
				workerThreads.push_back(worker);
				worker->start();
			}
		}

		void ImageSaveEngine::setSaveImgFormat(const ImageSaveFormat format)
		{
			_saveImgFormat = format; 
		}

		void ImageSaveEngine::processImages()
		{
			while (true) {
				QList<ImageInfo> tasks;

				{
					QMutexLocker locker(&mutex);
					if (saveQueue.isEmpty() && !stopFlag) {
						condition.wait(&mutex);
					}

					if (stopFlag && saveQueue.isEmpty()) {
						break;
					}

					// 批量取出任务
					while (!saveQueue.isEmpty() && tasks.size() < batchSize) {
						tasks.append(saveQueue.dequeue());
					}

					// 保存图片
					for (const auto& task : tasks) {
						saveImage(task);
					}
				}

				
			}
		}

		bool ImageSaveEngine::isAllImageSaved()
		{
			QMutexLocker locker(&mutex);
			return saveQueue.isEmpty();
		}

		void ImageSaveEngine::saveImage(const ImageInfo& image)
		{
			QString path{};
			if (image.dirName.isEmpty())
			{
				path = rootPath + "/" + image.classify;
			}
			else
			{
				path = rootPath + "/" + image.dirName;
			}
			QDir dir(path);
			if (!dir.exists()) {
				dir.mkpath(".");
			}

			// 获取格式字符串
			QString formatStr = imageFormatToString(_saveImgFormat);

			// 构造文件名
			QString fileName = dir.filePath(image.classify + image.time + "." + formatStr);

			// 检查策略
			if (savePolicy == ImageSaveEnginePolicy::MaxSaveImageNum) {
				auto& imageList = savedImages[image.classify];
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

		void ImageSaveEngine::setSavePolicy(ImageSaveEnginePolicy policy)
		{
			QMutexLocker locker(&mutex);
			savePolicy = policy;
		}

		void ImageSaveEngine::setMaxSaveImageNum(int maxNum)
		{
			QMutexLocker locker(&mutex);
			maxSaveImageNum = maxNum;
		}

		void ImageSaveEngine::setSaveImgQuality(int quality)
		{
			QMutexLocker locker(&mutex);
			saveImgQuality = quality;
		}
	}
}