#pragma once

#include<QObject>
#include "ImageProcessorModule.hpp"
#include "imgPro_ImageProcess.hpp"
#include"IModule.hpp"

class ImgProModule
	:public QObject, public IModule<bool>
{
	Q_OBJECT
private:
	static constexpr int  imgProSignalWorkThreadNum = 1;
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
private:
	void buildImgProContext();
	void buildImgProContextPreProcess();

	void resetImgProIsUpdate(bool state);
public:
	rw::imgPro::ImageProcessContext imageProcessContext_PreProcess;

	std::atomic<bool> imgProIsUpdate[4];
private:
	void buildImageProcessingModule(size_t num);
	void destroyImageProcessingModule();

	void buildImageProcessingPreModule(size_t num);
public:
	std::unique_ptr<ImageProcessingModule> imageProcessingModule1{ nullptr };
	std::unique_ptr<ImageProcessingModule> imageProcessingModule2{ nullptr };

public slots:
	void onUpdateImgProContext();
};