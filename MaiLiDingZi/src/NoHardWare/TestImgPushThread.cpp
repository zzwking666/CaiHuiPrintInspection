#include"TestImgPushThread.hpp"

#include <QDir>

#include "Utilty.hpp"
#include <random>

#include "Modules.hpp"

#ifdef BUILD_WITHOUT_HARDWARE
void TestImgPushThread::setPushImgTime(size_t pushImgTime)
{
	_pushImgTime = pushImgTime;
}

TestImgPushThread::TestImgPushThread(QObject* parent)
	: QThread(parent), _running(false) {
	readAllImgsPath();
	readAllImgsPath2();
}

TestImgPushThread::~TestImgPushThread()
{
	stopThread();
	wait();
}

void TestImgPushThread::readAllImgsPath()
{
	auto paths = globalPath.testImgDirPath;
	QDir dir(paths);
	if (!dir.exists()) {
		qDebug() << "Directory does not exist: " << paths;
		return;
	}
	static const QSet<QString> supportedSuffixes = {
		"jpg", "jpeg", "png", "bmp", "gif", "tiff", "webp"
	};
	QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	imgsPath.clear();
	imgCache.clear();
	int cacheCount = 0;
	for (const QFileInfo& fileInfo : fileList) {
		if (fileInfo.isFile() && supportedSuffixes.contains(fileInfo.suffix().toLower())) {
			imgsPath.append(fileInfo.absoluteFilePath());
			if (cacheCount < 100) {
				cv::Mat mat = cv::imread(fileInfo.absoluteFilePath().toStdString());
				if (!mat.empty()) {
					imgCache.append(mat);
					++cacheCount;
				}
			}
		}
	}
}

void TestImgPushThread::readAllImgsPath2()
{
	auto paths = globalPath.testImgDirPath;
	QDir dir(paths);
	if (!dir.exists()) {
		qDebug() << "Directory 2 does not exist: " << paths;
		return;
	}
	static const QSet<QString> supportedSuffixes = {
		"jpg", "jpeg", "png", "bmp", "gif", "tiff", "webp"
	};
	QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	imgsPath2.clear();
	imgCache2.clear();
	int cacheCount = 0;
	for (const QFileInfo& fileInfo : fileList) {
		if (fileInfo.isFile() && supportedSuffixes.contains(fileInfo.suffix().toLower())) {
			imgsPath2.append(fileInfo.absoluteFilePath());
			if (cacheCount < 100) {
				cv::Mat mat = cv::imread(fileInfo.absoluteFilePath().toStdString());
				if (!mat.empty()) {
					imgCache2.append(mat);
					++cacheCount;
				}
			}
		}
	}
}

void TestImgPushThread::startThread()
{
	_running = true;
	if (!isRunning()) {
		start();
	}
}

void TestImgPushThread::stopThread()
{
	_running = false;
}

void TestImgPushThread::readImg(size_t s)
{
	auto& isPushImg = Modules::getInstance().test_module.testImgPush;
	if (!isPushImg.load())
	{
		return;
	}

	if (s % _pushImgTime == 0 && !imgCache2.isEmpty())
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, imgCache2.size() - 1);

		int randomIndex = dis(gen);

		rw::rqw::MatInfo selectedImg{};

		selectedImg.mat = imgCache2[randomIndex].clone();

		emit imgReady(selectedImg, 1);
	}
}

void TestImgPushThread::readImg2(size_t s)
{
	auto& isPushImg = Modules::getInstance().test_module.testImgPush;
	if (!isPushImg.load())
	{
		return;
	}

	if (s % _pushImgTime == 0 && !imgCache2.isEmpty())
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, imgCache2.size() - 1);

		int randomIndex = dis(gen);

		rw::rqw::MatInfo selectedImg{};

		selectedImg.mat = imgCache2[randomIndex].clone();

		emit imgReady2(selectedImg, 2);
	}
}

void TestImgPushThread::run()
{
	static size_t s = 0;
	while (_running) {
		QThread::msleep(1);
		++s;
		readImg(s);
		readImg2(s);
		if (s == 1000000)
		{
			s = 0;
		}
	}
}
#endif



