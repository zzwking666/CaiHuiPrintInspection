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

};
