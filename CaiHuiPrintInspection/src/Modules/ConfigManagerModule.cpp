#include "ConfigManagerModule.hpp"

#include "CaiHuiPrintInspection.h"
#include "Modules.hpp"
#include "Utilty.hpp"

bool ConfigManagerModule::build()
{
	storeContext = std::make_unique<rw::oso::StorageContext>(rw::oso::StorageType::Xml);

	loadConfigSafe(globalPath.CaiHuiPrintInspectionConfigPath, caihuiPrintInspectionConfig, "主窗体参数");
	loadConfigSafe(globalPath.setConfigPath, setConfig, "设置窗体参数");

	return true;
}

void ConfigManagerModule::destroy()
{
	storeContext->saveSafe(caihuiPrintInspectionConfig, globalPath.CaiHuiPrintInspectionConfigPath.toStdString());
	storeContext->saveSafe(setConfig, globalPath.setConfigPath.toStdString());
	storeContext.reset();
}

void ConfigManagerModule::start()
{
	
}

void ConfigManagerModule::stop()
{

}

void ConfigManagerModule::saveSetConfig()
{
	storeContext->saveSafe(setConfig, globalPath.setConfigPath.toStdString());
}
