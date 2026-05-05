#include "Modules.hpp"

#include <QDir>
#include <QMessageBox>

#include "DlgProductSet.h"
#include "CaiHuiPrintInspection.h"
#include "MaiLiDingZi.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "SetConfig.hpp"
#include "Utilty.hpp"


Modules::Modules()
{

}

Modules::~Modules()
{

}

bool Modules::build()
{
	// 读取配置
	auto configManagerBuild = configManagerModule.build();

	// 构建相机
	auto cameraBuild = cameraModule.build();

	// 构建显示信息模块
	auto runtimeInfoModuleBuild = runtimeInfoModule.build();

	// 构建图像处理模块
	auto imgProModuleBuild = imgProModule.build();

	// 构建UI模块
	uiModule.build();

	// 构建重连模块
	reconnectModule.build();

	// 构建消除模块
	auto eliminateModuleBuild = eliminateModule.build();

	// 构建图像保存模块
	imgSaveModule.build();

#ifdef BUILD_WITHOUT_HARDWARE
	test_module.build();
#endif

	return true;
}

void Modules::destroy()
{
#ifdef BUILD_WITHOUT_HARDWARE
	test_module.destroy();
#endif
	runtimeInfoModule.destroy();
	imgProModule.destroy();
	cameraModule.destroy();
	configManagerModule.destroy();
	uiModule.destroy();
	reconnectModule.destroy();
	eliminateModule.destroy();
	imgSaveModule.destroy();
}

void Modules::start()
{
	uiModule.start();
	configManagerModule.start();
	runtimeInfoModule.start();
	imgSaveModule.start();
	eliminateModule.start();
	imgProModule.start();
	cameraModule.start();
	reconnectModule.start();

#ifdef BUILD_WITHOUT_HARDWARE
	test_module.start();
#endif
}

void Modules::stop()
{
#ifdef BUILD_WITHOUT_HARDWARE
	test_module.stop();
#endif
	reconnectModule.stop();
	cameraModule.stop();
	imgProModule.stop();
	eliminateModule.stop();
	imgSaveModule.stop();
	runtimeInfoModule.stop();
	configManagerModule.stop();
	uiModule.stop();
}

void Modules::connect()
{
#pragma region connect camera and imgProModule
	QObject::connect(&cameraModule, &CameraModule::frameCaptured1,
		Modules::getInstance().imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
	QObject::connect(&cameraModule, &CameraModule::frameCaptured2,
		Modules::getInstance().imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
#pragma endregion

#pragma region connect UIModule and ReconnectModule
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::updateCameraLabelState,
		uiModule._maiLiDingZi, &CaiHuiPrintInspection::updateCameraLabelState);
#pragma endregion

#pragma region connect UIModule and imgProModule
	QObject::connect(imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::imageReady,
		uiModule._maiLiDingZi, &CaiHuiPrintInspection::onCameraDisplay);
	QObject::connect(imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::imageReady,
		uiModule._maiLiDingZi, &CaiHuiPrintInspection::onCameraDisplay);

	QObject::connect(uiModule._dlgProductSet,&DlgProductSet::paramsChanged,
		&imgProModule, &ImgProModule::onUpdateImgProContext);
#pragma endregion

#pragma region connect UIModules
	QObject::connect(uiModule._dlgProductSet, &DlgProductSet::emit_changeLanguage,
		uiModule._maiLiDingZi, &CaiHuiPrintInspection::changeLanguage);
#pragma endregion

#pragma region connect camera and ReconnectModule
	// 相机重连
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::buildCamera,
		&cameraModule, &CameraModule::onBuildCamera);
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::startMonitor,
		&cameraModule, &CameraModule::onStartCamera);
	// 相机销毁
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::destroyCamera,
		&cameraModule, &CameraModule::onDestroyCamera);
#pragma endregion

#pragma region connect imgProModule and RuntimeInfoModule
	// 未使用DetachUtiltyThread
	/*QObject::connect(imgProModule.imageProcessingModule3.get(), &ImageProcessingPackageModule::updateStatisticalInfo,
		runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updateStatisticalInfo);*/
#pragma endregion

#pragma region connect UIModule and RuntimeInfoModule
	QObject::connect(runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updateStatisticalInfo,
		uiModule._maiLiDingZi, &CaiHuiPrintInspection::onUpdateStatisticalInfoUI, Qt::QueuedConnection);
#pragma endregion

#ifdef BUILD_WITHOUT_HARDWARE
	QObject::connect(test_module.testImgPushThread.get(), &TestImgPushThread::imgReady,
		imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
	QObject::connect(test_module.testImgPushThread.get(), &TestImgPushThread::imgReady2,
		imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
	#endif
}

bool Modules::check()
{
#pragma region check single instance
	if (!rw::rqw::RunEnvCheck::isSingleInstance("MaiLiDingZi.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "已经有程序在运行，请勿多次打开");
		return false;
	}
#pragma endregion

#pragma region check run env
	if (rw::rqw::RunEnvCheck::isProcessRunning("MVS.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到海康威视软件正在运行，请先关闭后再启动本程序。");
		return false;
	}

	if (rw::rqw::RunEnvCheck::isProcessRunning("BasedCam3.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到度申相机平台软件正在运行，请先关闭后再启动本程序。");
		return false;
	}
#pragma endregion

#pragma region check directory exist
	EnsureDirectoryExists(globalPath.projectHome);
	EnsureDirectoryExists(globalPath.configRootPath);
	EnsureDirectoryExists(globalPath.modelRootPath);
	EnsureDirectoryExists(globalPath.imageSaveRootPath);
#pragma endregion

#pragma region check model exist
	if (!rw::rqw::RunEnvCheck::isFileExist(globalPath.modelPath))
	{
		QMessageBox::warning(nullptr, "错误", "模型文件缺失");
		return false;
	}
#pragma endregion

#pragma region check config format and exist
	rw::oso::StorageContext storageContext(rw::oso::StorageType::Xml);

	checkFileExistAndFormat<cdm::MaiLiDingZiConfig>(globalPath.CaiHuiPrintInspectionConfigPath, storageContext);
	checkFileExistAndFormat<cdm::SetConfig>(globalPath.setConfigPath, storageContext);
#pragma endregion

	return true;
}

bool Modules::EnsureDirectoryExists(const QString& dirPath)
{
	QDir dir(dirPath);
	if (!dir.exists()) {
		return dir.mkpath(".");
	}
	return true;
}
