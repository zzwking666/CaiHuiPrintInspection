#include "ImgProModule.hpp"

#include "Modules.hpp"
#include "osoFIleUtiltyFunc.hpp"
#include "Utilty.hpp"

bool ImgProModule::build()
{
	buildImgProContext();
	buildImageProcessingModule(imgProSignalWorkThreadNum);
	for (int i = 0; i < 4; i++)
	{
		imgProIsUpdate[i] = true;
	}
	return true;
}

void ImgProModule::destroy()
{
	destroyImageProcessingModule();
}

void ImgProModule::start()
{

}

void ImgProModule::stop()
{

}

void ImgProModule::buildImgProContext()
{
	buildImgProContextPreProcess();
}

void ImgProModule::buildImgProContextPreProcess()
{
	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& maiLiDingZiConfig = Modules::getInstance().configManagerModule.maiLiDingZiConfig;
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;

#pragma region  build base
	imageProcessContext_PreProcess.imageProcessPrepare = [this, &runningState,
		&maiLiDingZiConfig, &setConfig](rw::imgPro::ImageProcessContext& context)
		{
			if (context.customFields.find("ImgProcessIndex") == context.customFields.end()) {
				return;
			}
			auto ImgProcessIndex = std::any_cast<int>(context.customFields["ImgProcessIndex"]);

			if (context.customFields.find("stationIdx") == context.customFields.end()) {
				return;
			}
			auto stationIdx = std::any_cast<int>(context.customFields["stationIdx"]);

			double currentPixToWorld{ 0 };

			//update pixToWorld
			{
				if (1 == ImgProcessIndex)
				{
					currentPixToWorld = setConfig.xiangsudangliang1;
				}
				else if (2 == ImgProcessIndex)
				{
					currentPixToWorld = setConfig.xiangsudangliang2;
				}
				context.customFields["CurrentPixToWorld"] = currentPixToWorld;
			}

			//update Config
			{
				if (imgProIsUpdate[stationIdx])
				{
					context.eliminationCfg = cdm::ScoreConfigConvert::toClassIdWithEliConfigMap(currentPixToWorld * currentPixToWorld, 100);
					context.defectCfg = cdm::ScoreConfigConvert::toClassIdWithDefConfigMap();
					imgProIsUpdate[stationIdx] = false;
				}
			}

			// update limite
			{
				int limitTop{ 0 };
				int limitBottom{ 0 };
				int limitLeft{ 0 };
				int limitRight{ 0 };

				if (1 == ImgProcessIndex)
				{
					limitTop = static_cast<int>(setConfig.shangxianwei1);
					limitBottom = static_cast<int>(setConfig.xiaxianwei1);
					limitLeft = static_cast<int>(setConfig.zuoxianwei1);
					limitRight = static_cast<int>(setConfig.youxianwei1);
				}
				else if (2 == ImgProcessIndex)
				{
					limitTop = static_cast<int>(setConfig.shangxianwei2);
					limitBottom = static_cast<int>(setConfig.xiaxianwei2);
					limitLeft = static_cast<int>(setConfig.zuoxianwei2);
					limitRight = static_cast<int>(setConfig.youxianwei2);
				}

				context.customFields["LimitTop"] = limitTop;
				context.customFields["LimitBottom"] = limitBottom;
				context.customFields["LimitLeft"] = limitLeft;
				context.customFields["LimitRight"] = limitRight;
			}

			// update drawConfig
			{
				if (RunningState::Debug == runningState)
				{
					context.defectDrawCfg.textLocate = rw::imgPro::ConfigDrawRect::TextLocate::LeftTopIn;

					context.defectDrawCfg.isDrawDefects = true;
					context.defectDrawCfg.isDrawDisableDefects = true;
					context.defectDrawCfg.isDisAreaText = true;
					context.defectDrawCfg.isDisScoreText = true;

					context.runTextCfg.isDrawExtraText = false;

				}
				else if (RunningState::OpenRemoveFunc == runningState)
				{
					context.defectDrawCfg.isDrawDefects = true;
					context.defectDrawCfg.isDrawDisableDefects = true;
					context.defectDrawCfg.isDisAreaText = true;
					context.defectDrawCfg.isDisScoreText = true;

					context.runTextCfg.isDrawExtraText = false;
				}
			}
		};
#pragma endregion

#pragma region build index get
	imageProcessContext_PreProcess.indexGetContext.removeIndicesIfByInfo = [this](const rw::DetectionRectangleInfo& info
		, rw::imgPro::ImageProcessContext& context)
		{
			bool isInShieldWires = false;
			int limitTop{ -1 };
			int limitBottom{ -1 };
			int limitLeft{ -1 };
			int limitRight{ -1 };

			if (context.customFields.find("LimitTop") != context.customFields.end()) {
				limitTop = std::any_cast<int>(context.customFields["LimitTop"]);
			}
			if (context.customFields.find("LimitBottom") != context.customFields.end()) {
				limitBottom = std::any_cast<int>(context.customFields["LimitBottom"]);
			}
			if (context.customFields.find("LimitLeft") != context.customFields.end()) {
				limitLeft = std::any_cast<int>(context.customFields["LimitLeft"]);
			}
			if (context.customFields.find("LimitRight") != context.customFields.end()) {
				limitRight = std::any_cast<int>(context.customFields["LimitRight"]);
			}

			if (-1 == limitTop || -1 == limitBottom || -1 == limitLeft || -1 == limitRight)
			{
				return false;
			}
			// 判断缺陷框中心点是否在屏蔽线区域内

			if (info.center_y > limitTop && info.center_y < limitBottom)
			{
				if (info.center_x > limitLeft && info.center_x < limitRight)
				{
					isInShieldWires = true;
				}
			}

			return !isInShieldWires;
		};

	imageProcessContext_PreProcess.indexGetContext.removeIndicesPost =
		[this](rw::imgPro::ProcessResultIndexMap& indexMap,
			const rw::imgPro::ProcessResult& processResult,
			rw::imgPro::ImageProcessContext& context) {


		};
#pragma endregion

#pragma region build elimination config
	imageProcessContext_PreProcess.eliminationInfoGetContext.getEliminationItemPostOperator = [](
		const rw::imgPro::ProcessResult& processResult,
		const rw::imgPro::ProcessResultIndexMap& processResultIndexMap,
		const rw::imgPro::ClassIdWithEliminationInfoConfigMap& classIdWithEliminationInfoConfigMap,
		rw::imgPro::ImageProcessContext& context,
		rw::imgPro::EliminationInfo& eliminationInfo
		)
		{

		};
#pragma endregion

#pragma region build defect config
	imageProcessContext_PreProcess.defectResultGetContext.getDefectResultExtraPostOperate = [this](
		const rw::imgPro::ProcessResult& processResult,
		const rw::imgPro::ClassIdWithEliminationInfoConfigMap& classIdWithEliminationInfoConfigMap,
		const rw::imgPro::EliminationInfo& eliminationInfo,
		const rw::imgPro::ClassIdWithDefectResultInfoFuncConfigMap& classIdWithDefectResultInfoFuncConfigMap,
		rw::imgPro::DefectResultInfo& defectResultInfo,
		rw::imgPro::ImageProcessContext& context)
		{


		};
#pragma endregion

#pragma region build defect draw
	imageProcessContext_PreProcess.defectDrawCfg.classIdNameMap = ClassId::classIdNameMap;

	rw::imgPro::DefectDrawConfigItem drawItemConfig;

	drawItemConfig.fontSize = 50;
	drawItemConfig.textLocate = rw::imgPro::ConfigDrawRect::TextLocate::LeftTopIn;
	drawItemConfig.isDrawMask = false;

	for (size_t i = ClassId::minNum; i <= ClassId::maxNum; i++)
	{
		imageProcessContext_PreProcess.defectDrawCfg.classIdWithConfigMap[i] = drawItemConfig;
	}

	imageProcessContext_PreProcess.defectDrawFuncContext.postOperateFunc = [](
		QImage& img,
		rw::imgPro::ImageProcessContext& context) {

			int limitTop{ 0 };
			int limitBottom{ 0 };
			int limitLeft{ 0 };
			int limitRight{ 0 };

			if (context.customFields.find("LimitTop") != context.customFields.end()) {
				limitTop = std::any_cast<int>(context.customFields["LimitTop"]);
			}
			if (context.customFields.find("LimitBottom") != context.customFields.end()) {
				limitBottom = std::any_cast<int>(context.customFields["LimitBottom"]);
			}
			if (context.customFields.find("LimitLeft") != context.customFields.end()) {
				limitLeft = std::any_cast<int>(context.customFields["LimitLeft"]);
			}
			if (context.customFields.find("LimitRight") != context.customFields.end()) {
				limitRight = std::any_cast<int>(context.customFields["LimitRight"]);
			}

			rw::imgPro::ConfigDrawLine configDrawLine;
			configDrawLine.color = rw::imgPro::Color::Red;
			configDrawLine.thickness = 5;

			configDrawLine.position = limitTop;
			rw::imgPro::ImagePainter::drawHorizontalLine(img, configDrawLine);
			configDrawLine.position = limitBottom;
			rw::imgPro::ImagePainter::drawHorizontalLine(img, configDrawLine);
			configDrawLine.position = limitLeft;
			rw::imgPro::ImagePainter::drawVerticalLine(img, configDrawLine);
			configDrawLine.position = limitRight;
			rw::imgPro::ImagePainter::drawVerticalLine(img, configDrawLine);
		};
#pragma endregion

#pragma region build running time text
	imageProcessContext_PreProcess.runTextCfg.isDisProcessImgTime = false;
	imageProcessContext_PreProcess.runTextCfg.isDrawExtraText = false;
#pragma endregion
}

void ImgProModule::resetImgProIsUpdate(bool state)
{
	for (auto& flag : imgProIsUpdate) {
		flag.store(state);
	}
}

void ImgProModule::buildImageProcessingModule(size_t num)
{
	buildImageProcessingPreModule(num);
}

void ImgProModule::destroyImageProcessingModule()
{
	imageProcessingModule1.reset();
	imageProcessingModule2.reset();
}

void ImgProModule::buildImageProcessingPreModule(size_t num)
{
	imageProcessingModule1 = std::make_unique<ImageProcessingModule>(num, this);
	imageProcessingModule1->modelEnginePath = globalPath.modelPath;
	imageProcessingModule1->index = 1;
	imageProcessingModule1->BuildModule();

	imageProcessingModule2 = std::make_unique<ImageProcessingModule>(num, this);
	imageProcessingModule2->modelEnginePath = globalPath.modelPath;
	imageProcessingModule2->index = 2;
	imageProcessingModule2->BuildModule();
}

void ImgProModule::onUpdateImgProContext()
{
	buildImgProContext();
	resetImgProIsUpdate(true);
}
