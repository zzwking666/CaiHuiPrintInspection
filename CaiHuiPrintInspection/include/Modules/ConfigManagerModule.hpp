#pragma once

#include"IModule.hpp"
#include<QObject>
#include <QVector>
#include <memory>
#include "oso_StorageContext.hpp"
#include "SetConfig.hpp"
#include "MaiLiDingZi.hpp"
#include "halconData.hpp"

class ConfigManagerModule
	: public QObject, public IModule<bool>
{
	Q_OBJECT
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<rw::oso::StorageContext> storeContext{ nullptr };
public:
 static constexpr int templateMatchWindowCount{ 4 };

	cdm::MaiLiDingZiConfig maiLiDingZiConfig{};
	cdm::SetConfig setConfig{};
 QVector<std::shared_ptr<HalconData>> halconDatas;

	HalconData* getHalconData(int index);
	const HalconData* getHalconData(int index) const;
};
