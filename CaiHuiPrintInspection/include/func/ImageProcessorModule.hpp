#pragma once

#include"ime_ModelEngineFactory.h"

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <vector>
#include <QThread>
#include <QPixmap>
#include <imgPro_ImageProcess.hpp>
#include "rqw_CameraObjectCore.hpp"
#include "rqw_ImageSaveEngine.h"
#include "Utilty.hpp"


class ImageProcessor : public QThread
{
	Q_OBJECT

public:
	ImageProcessor(QQueue<MatInfo>& queue,
		QMutex& mutex,
		QWaitCondition& condition,
		int workIndex,
		QObject* parent = nullptr);

protected:
	void run() override;

private:
	void run_debug(MatInfo& frame);				// 不开剔废时候的调试模式
private:
	void run_OpenRemoveFunc(MatInfo& frame);	// 开启剔废功能时的处理模式

	void run_OpenRemoveFunc_emitErrorInfo(bool isbad);
signals:
	void imageReady(size_t index, QPixmap image);
private:
	// 存图
	void save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image);
	void save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image);
private:
	std::unique_ptr<rw::imgPro::ImageProcess> _imgProcess;
public:
	// 构建模型引擎
	void buildObbModelEngine(const QString& enginePath);
private:
	QQueue<MatInfo>& _queue;
	QMutex& _mutex;
	QWaitCondition& _condition;
	int _workIndex;
public:
	int imageProcessingModuleIndex;
};


class ImageProcessingModule : public QObject {
	Q_OBJECT
public:
	QString modelEnginePath;

public:
	// 初始化图像处理模块
	void BuildModule();
public:
	ImageProcessingModule(int numConsumers, QObject* parent = nullptr);

	~ImageProcessingModule();

public slots:
	// 相机回调函数
	void onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index);

signals:
	void imageReady(size_t index, QPixmap image);

public:
	std::vector<ImageProcessor*> getProcessors() const {
		return _processors;
	}

private:
	QQueue<MatInfo> _queue;
	QMutex _mutex;
	QWaitCondition _condition;
	std::vector<ImageProcessor*> _processors;
	int _numConsumers;
public:
	size_t index;
};


