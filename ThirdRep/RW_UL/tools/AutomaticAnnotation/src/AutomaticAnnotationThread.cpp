#include"AutomaticAnnotationThread.h"

#include"ime_ModelEngineFactory.h"
#include"rqw_ImagePainter.h"
#include<QMessageBox>
#include<QFile>
#include<QFileInfo>
#include<QDir>
#include <QDirIterator>
#include <QSet>
#include "zip.h"

#include "imgPro_ImagePainter.hpp"
#include "imgPro_ImageProcessUtilty.hpp"
#include "rqw_CameraObjectCore.hpp"

static std::vector<rw::DetectionRectangleInfo> filterByLabelList(
	const std::vector<rw::DetectionRectangleInfo>& input,
	const QVector<int>& labelList)
{
	std::vector<rw::DetectionRectangleInfo> result;
	QSet<int> labelSet(labelList.begin(), labelList.end()); // Corrected initialization of QSet

	for (const auto& info : input) {
		if (labelSet.contains(static_cast<int>(info.classId))) {
			result.push_back(info);
		}
	}
	return result;
}

AutomaticAnnotationThread::AutomaticAnnotationThread(const QVector<QString>& imagePaths, QObject* parent)
	: QThread(parent), m_imagePaths(imagePaths) {
}

QString AutomaticAnnotationThread::getObjectDetectionDataSetItem(const std::vector<rw::DetectionRectangleInfo>& annotationDataSet, int width, int height)
{
	QString result;
	for (const auto& item : annotationDataSet)
	{
		std::string id = std::to_string(item.classId);

		double norCenterX = static_cast<double>(item.center_x) / static_cast<double>(width);
		double norCenterY = static_cast<double>(item.center_y) / static_cast<double>(height);
		double norWidth = static_cast<double>(item.width) / static_cast<double>(width);
		double norHeight = static_cast<double>(item.height) / static_cast<double>(height);

		auto textStr = id + " " +
			std::to_string(norCenterX) + " " + std::to_string(norCenterY) + " "
			+ std::to_string(norWidth) + " " + std::to_string(norHeight) + "\n"; // 添加换行符

		result.append(QString::fromStdString(textStr));
	}
	return result;
}

QString AutomaticAnnotationThread::getObjectSegmentDataSetItem(
	const std::vector<rw::DetectionRectangleInfo>& annotationDataSet, int width, int height)
{
	QString result;

	for (const auto& item : annotationDataSet)
	{
		if (!item.segMaskValid || item.mask_roi.empty())
			continue;

		std::string id = std::to_string(item.classId);

		cv::Mat mask;
		if (item.mask_roi.channels() != 1) {
			cv::cvtColor(item.mask_roi, mask, cv::COLOR_BGR2GRAY);
		}
		else {
			mask = item.mask_roi;
		}

		// 针对CV_32FC1类型的掩码，先归一化到0~255
		if (mask.type() == CV_32FC1) {
			cv::Mat normMask;
			// 假设mask值域在0~1
			mask.convertTo(normMask, CV_8UC1, 255.0);
			mask = normMask;
		}
		else if (mask.type() != CV_8UC1) {
			mask.convertTo(mask, CV_8UC1);
		}

		// 二值化
		cv::threshold(mask, mask, 128, 255, cv::THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		size_t maxContourIdx = 0;
		size_t maxPoints = 0;
		for (size_t i = 0; i < contours.size(); ++i) {
			if (contours[i].size() > maxPoints) {
				maxPoints = contours[i].size();
				maxContourIdx = i;
			}
		}

		std::string pointsStr;
		if (!contours.empty()) {
			for (const auto& pt : contours[maxContourIdx]) {
				// 局部坐标 + ROI偏移 = 原图坐标
				double x = static_cast<double>(pt.x + item.roi.x) / static_cast<double>(width);
				double y = static_cast<double>(pt.y + item.roi.y) / static_cast<double>(height);
				pointsStr += " " + std::to_string(x) + " " + std::to_string(y);
			}
		}

		auto textStr = id + pointsStr + "\n";
		result.append(QString::fromStdString(textStr));
	}

	return result;
}
QString AutomaticAnnotationThread::getOrientedBoundingBoxesDataSetItem(const std::vector<rw::DetectionRectangleInfo>& annotationDataSet, int width, int height)
{
	QString result;
	for (const auto& item : annotationDataSet)
	{
		std::string id = std::to_string(item.classId);

		double x1 = static_cast<double>(item.leftTop.first) / static_cast<double>(width);
		double y1 = static_cast<double>(item.leftTop.second) / static_cast<double>(height);
		double x2 = static_cast<double>(item.rightTop.first) / static_cast<double>(width);
		double y2 = static_cast<double>(item.rightTop.second) / static_cast<double>(height);
		double x3 = static_cast<double>(item.rightBottom.first) / static_cast<double>(width);
		double y3 = static_cast<double>(item.rightBottom.second) / static_cast<double>(height);
		double x4 = static_cast<double>(item.leftBottom.first) / static_cast<double>(width);
		double y4 = static_cast<double>(item.leftBottom.second) / static_cast<double>(height);

		auto textStr = id + " " +
			std::to_string(x1) + " " + std::to_string(y1) + " " +
			std::to_string(x2) + " " + std::to_string(y2) + " " +
			std::to_string(x3) + " " + std::to_string(y3) + " " +
			std::to_string(x4) + " " + std::to_string(y4) + "\n";

		result.append(QString::fromStdString(textStr));
	}
	return result;
}

void AutomaticAnnotationThread::generateTrainTxt(const QString& fileName)
{
	QString imagesDir = fileName + "/images/train";
	QString outputTxt = fileName + "/train.txt";

	QDir imgDir(imagesDir);
	if (!imgDir.exists()) {
		qDebug() << "未找到图片文件夹:" << imagesDir;
		return;
	}

	QStringList imageExts = { "*.jpg", "*.jpeg", "*.png", "*.bmp" };
	QStringList imageFiles = imgDir.entryList(imageExts, QDir::Files | QDir::NoSymLinks);

	QStringList relPaths;
	for (const QString& fname : imageFiles) {
		QString relPath = "data/images/train/" + fname;
		relPaths.append(relPath.replace("\\", "/"));
	}

	std::sort(relPaths.begin(), relPaths.end());

	QFile outFile(outputTxt);
	if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "无法写入文件:" << outputTxt;
		return;
	}
	QTextStream out(&outFile);
	for (const QString& path : relPaths) {
		out << path << "\n";
	}
	outFile.close();

	qDebug() << "已生成" << outputTxt << "共" << relPaths.size() << "张图片。";
}

void AutomaticAnnotationThread::zipFolderToParentDir(const QString& folderPath)
{
	QFileInfo folderInfo(folderPath);
	if (!folderInfo.exists() || !folderInfo.isDir()) {
		qWarning() << "目录不存在:" << folderPath;
		return;
	}

	QString parentDir = folderInfo.dir().absolutePath();
	QString zipFileName = parentDir + "/" + folderInfo.fileName() + ".zip";

	int errorp;
	zip_t* archive = zip_open(zipFileName.toUtf8().constData(), ZIP_CREATE | ZIP_TRUNCATE, &errorp);
	if (!archive) {
		qWarning() << "无法创建zip文件:" << zipFileName;
		return;
	}

	// 缓存所有文件内容，保证内存有效
	std::vector<QByteArray> fileBuffers;

	QDirIterator it(folderPath, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString entryPath = it.next();
		QFileInfo entryInfo(entryPath);

		QString relPath = QDir(folderPath).relativeFilePath(entryPath).replace("\\", "/");

		if (entryInfo.isDir()) {
			QDir dir(entryPath);
			if (dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot).isEmpty()) {
				if (zip_dir_add(archive, relPath.toUtf8().constData(), ZIP_FL_ENC_UTF_8) < 0) {
					qWarning() << "无法添加空目录到zip:" << relPath;
				}
			}
			continue;
		}

		QFile file(entryPath);
		if (!file.open(QIODevice::ReadOnly)) {
			qWarning() << "无法打开文件:" << entryPath;
			continue;
		}
		fileBuffers.emplace_back(file.readAll());
		file.close();

		QByteArray& fileData = fileBuffers.back();
		zip_source_t* source = zip_source_buffer(archive, fileData.constData(), fileData.size(), 0);
		if (!source) {
			qWarning() << "无法创建zip源:" << entryPath;
			continue;
		}
		if (zip_file_add(archive, relPath.toUtf8().constData(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
			qWarning() << "无法添加文件到zip:" << relPath;
			zip_source_free(source);
		}
	}

	zip_close(archive);
	qDebug() << "压缩完成:" << zipFileName;
}

void AutomaticAnnotationThread::saveLabels(const QString& label, const QString& fileName)
{
	QString labelPath = labelOutput + "/" + fileName + ".txt";
	QDir dir;
	if (!dir.exists(labelOutput)) {
		if (!dir.mkpath(labelOutput)) {
			qDebug() << "Failed to create directory:" << labelOutput;
			return;
		}
	}

	QFile file(labelPath);
	// 使用Append模式追加内容
	if (!file.open(QIODevice::Append | QIODevice::Text)) {
		qDebug() << "Failed to open file for writing:" << labelPath;
		return;
	}

	QTextStream out(&file);
	out << label;
	file.close();
}

void AutomaticAnnotationThread::saveLabels_seg(const QString& label, const QString& fileName)
{
}

void AutomaticAnnotationThread::saveImage(const QString& imagePath)
{
	// 确保目标目录存在
	QDir dir;
	if (!dir.exists(imageOutput)) {
		if (!dir.mkpath(imageOutput)) {
			qDebug() << "Failed to create directory:" << imageOutput;
			return;
		}
	}

	// 获取源文件名
	QString fileName = QFileInfo(imagePath).fileName();

	// 构造目标路径
	QString targetPath = imageOutput + "/" + fileName;

	// 拷贝文件
	if (!QFile::copy(imagePath, targetPath)) {
		qDebug() << "Failed to copy file from" << imagePath << "to" << targetPath;
	}
	else {
		qDebug() << "File copied successfully to" << targetPath;
	}
}

void AutomaticAnnotationThread::run()
{
	auto engine = rw::ModelEngineFactory::createModelEngine(config, modelType, deployType);
	if (engine == nullptr)
	{
		return;
	}

	for (const QString& path : m_imagePaths) {
		auto mat = cv::imread(path.toStdString());
		if (mat.empty()) {
			qDebug() << "Failed to load image:" << path;
			continue;
		}
		auto result = engine->processImg(mat);
		result = filterByLabelList(result, labelList);

		auto fileName = QFileInfo(path).baseName();

		if (labelType == R"(Segment)")
		{
			auto label = getObjectSegmentDataSetItem(result, mat.cols, mat.rows);
			saveLabels(label, fileName);
		}
		else if (labelType == R"(OrientedBoundingBoxes)")
		{
			auto label = getOrientedBoundingBoxesDataSetItem(result, mat.cols, mat.rows);
			saveLabels(label, fileName);
		}
		else
		{
			auto label = getObjectDetectionDataSetItem(result, mat.cols, mat.rows);
			saveLabels(label, fileName);
		}
		saveImage(path);

		auto image = rw::rqw::cvMatToQImage(mat);

		if (labelType == R"(Segment)")
		{
			rw::imgPro::ConfigDrawMask config;
			config.rectCfg.fontSize = 30;

			for (const auto& item : result)
			{
				config.rectCfg.text = QString::number(item.classId);
				rw::imgPro::ImagePainter::drawMaskOnSourceImg(image, item, config);
			}
		}
		else
		{
			rw::imgPro::ConfigDrawRect configRect;
			configRect.fontSize = std::max(14, image.height() / 25);
			configRect.textColor = rw::imgPro::Color::Green;
			for (const auto& item : result)
			{
				configRect.text = QString::number(item.classId);
				rw::imgPro::ImagePainter::drawShapesOnSourceImg(image, item, configRect);
			}
		}

		QPixmap pixmap = QPixmap::fromImage(image);

		emit imageProcessed(path, pixmap);
	}
	generateTrainTxt(rootOutPutPath);
	if (isAutoZip)
	{
		zipFolderToParentDir(rootOutPutPath);
	}
}