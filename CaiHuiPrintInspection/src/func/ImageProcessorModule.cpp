#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ImageProcessorModule.hpp"

#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <atomic>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include "Utilty.hpp"
#include "halconcpp/HalconCpp.h"
#include "Halcon.h"
#include "HalconDisplay.hpp"
#include <QPainter>
#include <QPen>
#include <cmath>
#include <algorithm>
#include <limits>

#include "Modules.hpp"
#include "CaiHuiPrintInspection.h"

namespace {
	// 在给定最小间隔内只放行一次调用：成功返回 true，其他并发/过快的调用返回 false
	inline bool AllowOncePer(std::atomic<long long>& lastNs, std::chrono::nanoseconds minInterval)
	{
		using clock = std::chrono::steady_clock;
		const auto nowNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
			clock::now().time_since_epoch()).count();

		auto prev = lastNs.load(std::memory_order_relaxed);
		if (nowNs - prev < minInterval.count())
			return false; // 距上次放行未到间隔，拒绝

		// 只有一个线程能成功更新 lastNs，其他并发线程会失败并返回 false
		return lastNs.compare_exchange_strong(prev, nowNs, std::memory_order_relaxed);
	}
} // namespace


ImageProcessor::ImageProcessor(QQueue<MatInfo>& queue, QMutex& mutex, QWaitCondition& condition, int workIndex, QObject* parent)
	: QThread(parent), _queue(queue), _mutex(mutex), _condition(condition), _workIndex(workIndex)
{

}

void ImageProcessor::run()
{
	while (!QThread::currentThread()->isInterruptionRequested()) {
		MatInfo frame;
		{
			QMutexLocker locker(&_mutex);
			if (_queue.isEmpty()) {
				_condition.wait(&_mutex);
				if (QThread::currentThread()->isInterruptionRequested()) {
					break;
				}
			}
			if (!_queue.isEmpty()) {
				frame = _queue.dequeue();
			}
			else {
				continue; // 如果队列仍为空，跳过本次循环
			}
		}

		// 检查 frame 是否有效
		if (frame.image.empty()) {
			continue; // 跳过空帧
		}

		auto currentRunningState = Modules::getInstance().runtimeInfoModule.runningState.load();
		switch (currentRunningState)
		{
		case RunningState::Debug:
			run_debug(frame);
			break;
		case RunningState::OpenRemoveFunc:
			run_OpenRemoveFunc(frame);
			break;
		default:
			break;
		}
	}
}

void ImageProcessor::run_debug(MatInfo& frame)
{
	if (frame.image.empty())
	{
		return;
	}

	try
	{
		auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;

		int halconIndex = -1;
		if (frame.index > 0)
		{
			halconIndex = static_cast<int>(frame.index) - 1;
		}
		if (halconIndex < 0)
		{
			halconIndex = imageProcessingModuleIndex - 1;
		}

		if (halconIndex >= 0)
		{
			if (halconDatas.size() <= halconIndex)
			{
				halconDatas.resize(halconIndex + 1);
			}

			HalconCpp::HObject hoImage = rw::rqw::HalconDisplay::matToHObject(frame.image);
			HalconCpp::HObject copiedImage;
			HalconCpp::CopyImage(hoImage, &copiedImage);
			halconDatas[halconIndex].processImage = copiedImage;
		}
	}
	catch (...)
	{
	}

 QImage qimg(frame.image.data, frame.image.cols, frame.image.rows, frame.image.step, QImage::Format_BGR888);
	QImage savedImage = qimg.copy();
	rw::rqw::ImageInfo imageInfo(savedImage);
	save_image(imageInfo, savedImage);
	QPixmap displayPixmap = QPixmap::fromImage(savedImage);
	

	
	emit imageReady(imageProcessingModuleIndex, displayPixmap);
}

void ImageProcessor::run_OpenRemoveFunc(MatInfo& frame)
{
	bool isMatched = false;
	const bool shouldEmitError =
		Modules::getInstance().runtimeInfoModule.runningState.load() == RunningState::OpenRemoveFunc;

	try
	{
		auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;

		int halconIndex = -1;
		if (frame.index > 0)
		{
			halconIndex = static_cast<int>(frame.index) - 1;
		}
		if (halconIndex < 0)
		{
			halconIndex = imageProcessingModuleIndex - 1;
		}

		HalconData* halconDataPtr = nullptr;
		if (halconIndex >= 0)
		{
			if (halconDatas.size() <= halconIndex)
			{
				halconDatas.resize(halconIndex + 1);
			}
			halconDataPtr = &halconDatas[halconIndex];
		}

		HalconCpp::HObject hoImage = rw::rqw::HalconDisplay::matToHObject(frame.image);
		HalconCpp::HObject copiedImage;
		HalconCpp::CopyImage(hoImage, &copiedImage);
		if (halconDataPtr)
		{
			halconDataPtr->processImage = copiedImage;
		}

		if (!halconDataPtr || !halconDataPtr->ckb_findShapemodel)
		{
			isMatched = halconDataPtr != nullptr;
		}
		else if (halconDataPtr->hv_ModelID.TupleLength() > 0)
		{
			using namespace HalconCpp;

			auto buildUnionFromRegions = [](const QVector<HObject>& regions, HObject* outUnion)
				{
					HObject concatRegions;
					GenEmptyObj(&concatRegions);

					for (const auto& region : regions)
					{
						if (!region.IsInitialized())
						{
							continue;
						}

						HObject tmp;
						ConcatObj(concatRegions, region, &tmp);
						concatRegions = tmp;
					}

					Union1(concatRegions, outUnion);
				};

			HObject imageForMatch = hoImage;

			if (halconDataPtr->isMeaning)
			{
				int meanSize = static_cast<int>(std::round(halconDataPtr->meaning));
				if (meanSize < 1)
				{
					meanSize = 1;
				}
				if (meanSize % 2 == 0)
				{
					++meanSize;
				}

				HObject meanImage;
				MeanImage(imageForMatch, &meanImage, meanSize, meanSize);
				imageForMatch = meanImage;
			}

			// 运行态匹配改为全图匹配，不再按绘制区域 ReduceDomain

			HTuple hvFindRow, hvFindCol, hvFindAngle, hvFindScore;
            const auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
			FindShapeModel(imageForMatch,
				halconDataPtr->hv_ModelID,
				-3.1415926,
				6.2831852,
				setConfig.shapemodelScore,
				1,
				0.5,
				"least_squares",
				0,
				0.9,
				&hvFindRow,
				&hvFindCol,
				&hvFindAngle,
				&hvFindScore);

			isMatched = hvFindRow.TupleLength() > 0;
			qDebug() << "FindShapeModel result: " << isMatched << " matches found.";
			if (isMatched)
			{
				HObject modelContours;
				GetShapeModelContours(&modelContours, halconDataPtr->hv_ModelID, 1);

				const int matchCount = static_cast<int>(hvFindRow.TupleLength());
				for (int matchIdx = 0; matchIdx < matchCount; ++matchIdx)
				{
					HTuple hvHomMat2D;
					VectorAngleToRigid(
						0.0,
						0.0,
						0.0,
						hvFindRow[matchIdx],
						hvFindCol[matchIdx],
						hvFindAngle[matchIdx],
						&hvHomMat2D);

					HObject transContours;
					AffineTransContourXld(modelContours, &transContours, hvHomMat2D);

					HTuple hvContourCount;
					CountObj(transContours, &hvContourCount);
					const int contourCount = hvContourCount.TupleLength() > 0 ? hvContourCount[0].I() : 0;

					double minCol = std::numeric_limits<double>::max();
					double minRow = std::numeric_limits<double>::max();
					double maxCol = std::numeric_limits<double>::lowest();
					double maxRow = std::numeric_limits<double>::lowest();
					bool hasPoint = false;

					for (int contourIdx = 1; contourIdx <= contourCount; ++contourIdx)
					{
						HObject oneContour;
						SelectObj(transContours, &oneContour, contourIdx);

						HTuple hvRows, hvCols;
						GetContourXld(oneContour, &hvRows, &hvCols);

						const int pointCount = static_cast<int>(hvRows.TupleLength());
						if (pointCount < 2)
						{
							continue;
						}

						for (int pointIdx = 0; pointIdx < pointCount; ++pointIdx)
						{
							const double col = hvCols[pointIdx].D();
							const double row = hvRows[pointIdx].D();
							minCol = std::min(minCol, col);
							minRow = std::min(minRow, row);
							maxCol = std::max(maxCol, col);
							maxRow = std::max(maxRow, row);
							hasPoint = true;
						}
					}

					if (hasPoint)
					{
						const cv::Point topLeft(cvRound(minCol), cvRound(minRow));
						const cv::Point bottomRight(cvRound(maxCol), cvRound(maxRow));
						cv::rectangle(frame.image, topLeft, bottomRight, cv::Scalar(0, 255, 0), 5, cv::LINE_AA);
					}
				}
			}
		}

		if (shouldEmitError)
		{
			run_OpenRemoveFunc_emitErrorInfo(!isMatched);
		}
	}
	catch (...)
	{
		if (shouldEmitError)
		{
			run_OpenRemoveFunc_emitErrorInfo(true);
		}
	}

 QImage qimg(frame.image.data, frame.image.cols, frame.image.rows, frame.image.step, QImage::Format_BGR888);
	QImage savedImage = qimg.copy();
	rw::rqw::ImageInfo imageInfo(savedImage);
	save_image(imageInfo, savedImage);
	QPixmap displayPixmap = QPixmap::fromImage(savedImage);
	emit imageReady(imageProcessingModuleIndex, displayPixmap);

	if (!isMatched)
	{
		if (1 == imageProcessingModuleIndex)
		{
			auto& priorityQueue = Modules::getInstance().eliminateModule.productPriorityQueue1;
			priorityQueue->push(true);
			emit imageReady(3, displayPixmap);
		}
		else if (2 == imageProcessingModuleIndex)
		{
			auto& priorityQueue = Modules::getInstance().eliminateModule.productPriorityQueue2;
			priorityQueue->push(true);
			emit imageReady(4, displayPixmap);
		}
	}
}

void ImageProcessor::run_OpenRemoveFunc_emitErrorInfo(bool isbad)
{
	if (isbad)
	{
		if (1 == imageProcessingModuleIndex)
		{
			Modules::getInstance().eliminateModule.productPriorityQueue1->push(true);
			++Modules::getInstance().runtimeInfoModule.statisticalInfo.wasteCount;
		}
		else if (2 == imageProcessingModuleIndex)
		{
			Modules::getInstance().eliminateModule.productPriorityQueue2->push(true);
			++Modules::getInstance().runtimeInfoModule.statisticalInfo.wasteCount;
		}
	}
}

void ImageProcessor::save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image)
{
	save_image_work(imageInfo, image);
}

void ImageProcessor::save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image)
{
 
}

void ImageProcessor::buildObbModelEngine(const QString& enginePath)
{
	rw::ModelEngineConfig modelEngineConfig;
	modelEngineConfig.conf_threshold = 0.1f;
	modelEngineConfig.nms_threshold = 0.1f;
	modelEngineConfig.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;
	modelEngineConfig.letterBoxColor = cv::Scalar(114, 114, 114);
	modelEngineConfig.modelPath = enginePath.toStdString();
	auto engine = rw::ModelEngineFactory::createModelEngine(modelEngineConfig, rw::ModelType::Yolov11_Obb, rw::ModelEngineDeployType::TensorRT);

	_imgProcess = std::make_unique<rw::imgPro::ImageProcess>(engine);
	_imgProcess->context() = Modules::getInstance().imgProModule.imageProcessContext_PreProcess;
	_imgProcess->context().customFields["ImgProcessIndex"] = static_cast<int>(imageProcessingModuleIndex);
	_imgProcess->context().customFields["stationIdx"] = static_cast<int>(imageProcessingModuleIndex);
}

void ImageProcessingModule::BuildModule()
{
	for (int i = 0; i < _numConsumers; ++i) {
		static size_t workIndexCount = 0;
		ImageProcessor* processor = new ImageProcessor(_queue, _mutex, _condition, workIndexCount, this);
		workIndexCount++;
		processor->imageProcessingModuleIndex = index;
		//processor->buildObbModelEngine(modelEnginePath);
		connect(processor, &ImageProcessor::imageReady, this, &ImageProcessingModule::imageReady, Qt::QueuedConnection);

		_processors.push_back(processor);
		processor->start();
	}
}

ImageProcessingModule::ImageProcessingModule(int numConsumers, QObject* parent)
	: QObject(parent), _numConsumers(numConsumers)
{

}

ImageProcessingModule::~ImageProcessingModule()
{
	// 通知所有线程退出
	for (auto processor : _processors) {
		processor->requestInterruption();
	}

	// 唤醒所有等待的线程
	{
		QMutexLocker locker(&_mutex);
		_condition.wakeAll();
	}

	// 等待所有线程退出
	for (auto processor : _processors) {
		if (processor->isRunning()) {
			processor->wait(1000); // 使用超时机制，等待1秒
		}
		delete processor;
	}
}

void ImageProcessingModule::onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index)
{
	// 防抖动处理
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	const long long debounceMs = static_cast<long long>(std::max(0.0, setConfig.xiangjiguangdianpingbishijian));
	const auto minInterval = std::chrono::milliseconds(debounceMs);

   static std::array<std::atomic<long long>, 2> lastCamNs{ 0, 0 };
	const size_t camSlot = (index > 0 && index <= lastCamNs.size()) ? index - 1 : 0;

	if (!AllowOncePer(lastCamNs[camSlot], minInterval)) {
		return;
	}

	if (matInfo.mat.channels() == 4) {
		cv::cvtColor(matInfo.mat, matInfo.mat, cv::COLOR_BGRA2BGR);
	}
	if (matInfo.mat.type() != CV_8UC3) {
		matInfo.mat.convertTo(matInfo.mat, CV_8UC3);
	}

	if (matInfo.mat.empty()) {
		return; // 跳过空帧
	}

	QMutexLocker locker(&_mutex);
	MatInfo mat;
	mat.image = matInfo.mat;
	mat.index = index;

	_queue.enqueue(mat);
	_condition.wakeOne();
}
