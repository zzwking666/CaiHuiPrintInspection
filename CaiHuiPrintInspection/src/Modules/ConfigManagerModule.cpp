#include "ConfigManagerModule.hpp"

#include "Modules.hpp"
#include "Utilty.hpp"

bool ConfigManagerModule::build()
{
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
	storeContext.reset();
}

void ConfigManagerModule::start()
{
	
}

void ConfigManagerModule::stop()
{

}
