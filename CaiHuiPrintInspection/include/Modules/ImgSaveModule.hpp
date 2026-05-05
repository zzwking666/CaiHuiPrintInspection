#pragma once

#include<QObject>
#include"IModule.hpp"
#include "rqw_ImageSaveEngine.h"


class ImgSaveModule
	:public QObject, public IModule<void> {
	Q_OBJECT
private:
	static constexpr int cimgSaveThreadNum = 4;
	static constexpr int cimgSaveQuality = 80;
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<rw::rqw::ImageSaveEngine> imageSaveEngine{ nullptr };
};
