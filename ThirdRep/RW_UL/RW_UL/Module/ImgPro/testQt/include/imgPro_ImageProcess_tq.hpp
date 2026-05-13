#pragma once

#include <QThread>

#include"ime_ModelEngineFactory.h"
#include "imgPro_ImageProcess.hpp"

class ImgPro_ImageProcess_tq
	: public QThread
{
	Q_OBJECT
public:
	ImgPro_ImageProcess_tq();
	~ImgPro_ImageProcess_tq();
public:
	void build_imgPro();
private:
	std::unique_ptr<rw::ModelEngine> _engine;
	std::unique_ptr<rw::imgPro::ImageProcess> _imgProcess;
private:
	void iniRunTextConfig()
	{
		auto& context = _imgProcess->context();

		context.runTextCfg.isDrawExtraText = true;
		context.runTextCfg.isDisOperatorTime = false;
		context.runTextCfg.isDisProcessImgTime = true;
		context.runTextCfg.extraTextColor = rw::rqw::RQWColor::Orange;
		context.runTextCfg.operatorTimeTextColor = rw::rqw::RQWColor::Gray;
		context.runTextCfg.processImgTimeTextColor = rw::rqw::RQWColor::Cyan;
		context.runTextCfg.runTextProportion = 0.02;
	}
	void iniDefectResultContext();
	void iniEliminationContext();
	void iniGetIndexContext();
	void iniEliminationInfoGetConfig();
	void iniDefectResultGetConfig();
	void iniDefectDrawConfig();
public:
	void startTest();
	void run() override;
signals:
	void imgReady(QPixmap pixmap);
};
