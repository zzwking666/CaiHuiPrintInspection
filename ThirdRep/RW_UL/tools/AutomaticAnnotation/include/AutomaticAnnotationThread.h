#pragma once

#include <QThread>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QImage>
#include <QPixmap>

#include"ime_ModelEngineFactory.h"

class AutomaticAnnotationThread : public QThread {
	Q_OBJECT
public:
	QString labelType;
	QVector<int> labelList;
public:
	QString labelOutput{};
	QString imageOutput{};
	QString rootOutPutPath{};
	bool isAutoZip{ false };
	rw::ModelType modelType;
	rw::ModelEngineDeployType deployType;
public:
	explicit AutomaticAnnotationThread(const QVector<QString>& imagePaths, QObject* parent = nullptr);
private:
	QString getObjectDetectionDataSetItem(const std::vector<rw::DetectionRectangleInfo>& annotationDataSet, int width, int height);
	QString getObjectSegmentDataSetItem(const std::vector<rw::DetectionRectangleInfo>& annotationDataSet, int width, int height);
	QString getOrientedBoundingBoxesDataSetItem(const std::vector<rw::DetectionRectangleInfo>& annotationDataSet, int width, int height);

	void generateTrainTxt(const QString& fileName);
	void zipFolderToParentDir(const QString& folderPath);

	void saveLabels(const QString& label, const QString& fileName);
	void saveLabels_seg(const QString& label, const QString& fileName);
	void saveImage(const QString& imagePath);
signals:
	void imageProcessed(QString imagePath, QPixmap pixmap);

protected:
	void run() override;
public:
	rw::ModelEngineConfig config;

private:
	QVector<QString> m_imagePaths;
};