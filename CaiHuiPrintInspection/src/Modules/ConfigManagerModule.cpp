#include "ConfigManagerModule.hpp"

#include "Modules.hpp"
#include "Utilty.hpp"

bool ConfigManagerModule::build()
{
    halconDatas.clear();
	halconDatas.reserve(templateMatchWindowCount);
	for (int i = 0; i < templateMatchWindowCount; ++i)
	{
		halconDatas.push_back(std::make_shared<HalconData>());
	}

	storeContext = std::make_unique<rw::oso::StorageContext>(rw::oso::StorageType::Xml);

#pragma region readHandleScannerCfg
	auto loadMainWindowConfig = storeContext->loadSafe(globalPath.CaiHuiPrintInspectionConfigPath.toStdString());
	if (loadMainWindowConfig)
	{
		maiLiDingZiConfig = *loadMainWindowConfig;
	}
#pragma endregion

#pragma region readsetCfg
	loadMainWindowConfig = storeContext->loadSafe(globalPath.setConfigPath.toStdString());
	if (loadMainWindowConfig)
	{
		setConfig = *loadMainWindowConfig;
	}
#pragma endregion

	return true;
}

void ConfigManagerModule::destroy()
{
	storeContext->saveSafe(maiLiDingZiConfig, globalPath.CaiHuiPrintInspectionConfigPath.toStdString());
	storeContext->saveSafe(setConfig, globalPath.setConfigPath.toStdString());
   halconDatas.clear();
	storeContext.reset();
}

void ConfigManagerModule::start()
{
	
}

void ConfigManagerModule::stop()
{

}

HalconData* ConfigManagerModule::getHalconData(int index)
{
	if (index < 0 || index >= halconDatas.size())
	{
		return nullptr;
	}

	const auto& data = halconDatas[index];
	return data ? data.get() : nullptr;
}

const HalconData* ConfigManagerModule::getHalconData(int index) const
{
	if (index < 0 || index >= halconDatas.size())
	{
		return nullptr;
	}

	const auto& data = halconDatas[index];
	return data ? data.get() : nullptr;
}
