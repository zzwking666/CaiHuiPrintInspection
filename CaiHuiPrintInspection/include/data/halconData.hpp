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
	HalconCpp::HTuple hv_ModelID;
	double baoguang = 1000;
	double zengyi = 1;
	bool isMeaning = false;
	double meaning = 10;
	bool isContrast = false;
    bool ckb_findShapemodel = true;
	double maxcontrast = 20;
	double mincontrast = 10;

	QVector<HalconCpp::HObject>createRegions;
	QVector<HalconCpp::HObject>shieldRegions;



};
