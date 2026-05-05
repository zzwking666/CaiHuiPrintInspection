#pragma once
#include <QString>

#include "imgPro_ImageProcessUtilty.hpp"

inline struct GlobalPath
{
public:
	QString projectHome = R"(D:\zfkjData\CaiHuiPrintInspection\)";
public:
	QString configRootPath = projectHome + R"(config\)";
	QString modelRootPath = projectHome + R"(model\)";
	QString CaiHuiPrintInspectionConfigPath = configRootPath + R"(CaiHuiPrintInspection.xml)";
	QString setConfigPath = configRootPath + R"(SetConfig.xml)";
	QString modelPath = modelRootPath + R"(CaiHuiPrintInspection.engine)";
public:
	QString imageSaveRootPath = projectHome + R"(savedImages\)";
public:
	QString testImgDirPath = R"(D:\ImgStorage\MaiLiDingZi)";
}globalPath;

struct ClassId
{
public:
	static constexpr int minNum = 0;

	static constexpr int dingZi = 0;

	static constexpr int maxNum = 0;
	static constexpr int indexCount = 1;
	static std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> classIdNameMap;
	static std::vector<rw::imgPro::ClassId> classids;
};

struct Utility
{
	static QString cameraIp1;
	static QString cameraIp2;
};

// 图片信息
struct MatInfo {
	cv::Mat image;	// 图片内容
	float location;	// 记录拍照瞬间的时间点
	size_t index;	// 拍照的相机的下标
};

