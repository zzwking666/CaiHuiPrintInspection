#include"imgPro_ImageProcess_tq.hpp"

#include <filesystem>
#include <QLabel>
#include <QPixmap>

#include "imgPro_ImageProcess.hpp"

ImgPro_ImageProcess_tq::ImgPro_ImageProcess_tq()
{
	build_imgPro();
}

ImgPro_ImageProcess_tq::~ImgPro_ImageProcess_tq()
{
}

void ImgPro_ImageProcess_tq::build_imgPro()
{
	rw::ModelEngineConfig config;
	config.modelPath = R"(C:\Users\rw\Desktop\models\12.8\dundai.engine)";
	_engine = rw::ModelEngineFactory::createModelEngine(
		config, rw::ModelType::Yolov11_Det_CudaAcc, rw::ModelEngineDeployType::TensorRT);
	_imgProcess = std::make_unique<rw::imgPro::ImageProcess>(_engine);
	iniGetIndexContext();
	iniEliminationInfoGetConfig();
	iniEliminationContext();
	iniDefectResultGetConfig();
	iniDefectResultContext();
	iniDefectDrawConfig();
	iniRunTextConfig();
}

void ImgPro_ImageProcess_tq::iniDefectResultContext()

{
	auto& context = _imgProcess->context();
	context.defectResultGetContext.getDefectResultExtraOperate = [this](const rw::imgPro::EliminationItem& item, const rw::DetectionRectangleInfo&) {
		auto find = item.customFields.find("someValueWillBeUsed");
		if (find != item.customFields.end())
		{
			//std::cout << "someValueWillBeUsed :" << std::any_cast<int>(find->second) << "score is:" << item.score << std::endl;
		}
		};
	context.defectResultGetContext.getDefectResultExtraOperateDisable = [this](const rw::imgPro::EliminationItem& item, const rw::DetectionRectangleInfo&) {
		auto find = item.customFields.find("someValueWillBeUsed");
		if (find != item.customFields.end())
		{
			//std::cout << "It 's no ,someValueWillBeUsed :" << std::any_cast<int>(find->second) - 100 << "score is:" << item.score << std::endl;
		}
		};
}

void ImgPro_ImageProcess_tq::iniEliminationContext()

{
	auto& context = _imgProcess->context();
	//context.indexGetContext.removeIndicesIf = [](rw::imgPro::ClassId classId, rw::imgPro::ProcessResultIndex index) {
	//	return classId == 1;
	//	};

	context.eliminationInfoGetContext.getEliminationItemFuncSpecialOperator = [this](rw::imgPro::EliminationItem& item,
		const rw::DetectionRectangleInfo& info,
		const rw::imgPro::EliminationInfoGetConfig& cfg) {
			auto find = cfg.customFields.find("someValueWillBeUsed");
			if (find != cfg.customFields.end())
			{
				auto value = std::any_cast<int>(find->second) + 100;
				item.customFields["someValueWillBeUsed"] = value;
			}
		};
}

void ImgPro_ImageProcess_tq::iniGetIndexContext()
{
	auto& context = _imgProcess->context();
	//context.indexGetContext.removeIndicesIf = [](rw::imgPro::ClassId classId, rw::imgPro::ProcessResultIndex index) {
	//	return classId == 1;
	//	};

	context.indexGetContext.removeIndicesIfByInfo = [this](const rw::DetectionRectangleInfo& info, const rw::imgPro::ImageProcessContext&) {
		return false;
		};
}

void ImgPro_ImageProcess_tq::iniEliminationInfoGetConfig()

{
	rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap eliminationInfoGetConfigs;
	auto& context = _imgProcess->context();
	rw::imgPro::EliminationInfoGetConfig eliminationInfoGetConfig;
	eliminationInfoGetConfig.areaFactor = 0.00157;
	eliminationInfoGetConfig.scoreFactor = 100;
	eliminationInfoGetConfig.isUsingArea = false;
	eliminationInfoGetConfig.isUsingScore = true;
	eliminationInfoGetConfig.scoreRange = { 0,10 };
	eliminationInfoGetConfig.scoreIsUsingComplementarySet = false;
	eliminationInfoGetConfig.customFields["someValueWillBeUsed"] = (int)(100);
	eliminationInfoGetConfigs[0] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[1] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[2] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[3] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[4] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[5] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[6] = eliminationInfoGetConfig;
	eliminationInfoGetConfigs[7] = eliminationInfoGetConfig;
	context.eliminationCfg = eliminationInfoGetConfigs;
}

void ImgPro_ImageProcess_tq::iniDefectResultGetConfig()

{
	rw::imgPro::DefectResultInfoFunc::ClassIdWithConfigMap defectConfigs;
	auto& context = _imgProcess->context();
	rw::imgPro::DefectResultInfoFunc::Config defectConfig;
	defectConfig.isEnable = false;
	defectConfigs[0] = defectConfig;
	defectConfig.isEnable = true;
	defectConfigs[1] = defectConfig;
	defectConfigs[2] = defectConfig;
	defectConfigs[3] = defectConfig;
	defectConfigs[4] = defectConfig;
	defectConfigs[5] = defectConfig;
	defectConfigs[6] = defectConfig;
	defectConfigs[7] = defectConfig;
	defectConfigs[8] = defectConfig;
	defectConfigs[9] = defectConfig;
	context.defectCfg = defectConfigs;
}

void ImgPro_ImageProcess_tq::iniDefectDrawConfig()

{
	auto& context = _imgProcess->context();
	rw::imgPro::DefectDrawFunc::ConfigDefectDraw drawConfig;
	drawConfig.isDrawDefects = true;
	drawConfig.isDrawDisableDefects = true;
	drawConfig.setAllIdsWithSameColor({ 0,1,2,3,4,5,6 }, rw::rqw::RQWColor::Green, true);
	drawConfig.setAllIdsWithSameColor({ 0,1,2,3,4,5,6 }, rw::rqw::RQWColor::Red, false);
	//drawConfig.classIdWithColorWhichIsBad[2] = rw::rqw::RQWColor::Green;
	drawConfig.thickness = 3;
	drawConfig.fontSize = 20;
	drawConfig.classIdNameMap[0] = "Body";
	drawConfig.classIdNameMap[1] = "Hole";
	drawConfig.isDisAreaText = false;
	drawConfig.textLocate = rw::imgPro::ConfigDrawRect::TextLocate::LeftTopOut;
	drawConfig.isDrawMask = false;
	drawConfig.hasFrame = true;
	drawConfig.thresh = 0.9;
	drawConfig.maxVal = 1;
	context.defectDrawCfg = drawConfig;
}

void ImgPro_ImageProcess_tq::startTest()
{
    this->start();
}

void ImgPro_ImageProcess_tq::run()
{
    std::vector<std::string> imagePaths;
    std::string folder = R"(D:\zfkjDevelopment\ImgStorage\dundai)";
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            imagePaths.push_back(entry.path().string());
        }
    }
    std::vector<double> times;
    int count{ 0 };
    for (const auto& imgPath : imagePaths) {
        count++;
        auto mat = cv::imread(imgPath,cv::IMREAD_COLOR);
        if (mat.empty()) continue;
        (*_imgProcess)(mat);
        auto maskImg = _imgProcess->getMaskImg(mat);

        emit imgReady(QPixmap::fromImage(maskImg));
    }
}
