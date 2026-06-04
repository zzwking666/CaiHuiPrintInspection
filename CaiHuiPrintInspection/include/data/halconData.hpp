#pragma once

#include <QVector>
#include <halconcpp/HalconCpp.h>

class HalconData
{
public:
	HalconData();
	~HalconData();
public:

	HalconCpp::HObject processImage;
	// 每个创建的感兴趣区域对应一个独立模板，一个相机可能对应多个模板
	QVector<HalconCpp::HTuple> hv_ModelIDs;
	double baoguang = 1000;
	double zengyi = 1;
	bool isMeaning = false;
	double meaning = 10;
	bool isContrast = false;
   bool ckb_findShapemodel = false;
	double maxcontrast = 20;
	double mincontrast = 10;

	QVector<HalconCpp::HObject>createRegions;
	QVector<HalconCpp::HObject>shieldRegions;



};
