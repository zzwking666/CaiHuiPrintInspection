#pragma once

#include <QObject>
#include <halconcpp/HalconCpp.h>

class HalconData : public QObject
{
	Q_OBJECT

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
	double maxcontrast = 10;
	double mincontrast = 20;

	QVector<HalconCpp::HObject>createRegions;
	QVector<HalconCpp::HObject>shieldRegions;



};
