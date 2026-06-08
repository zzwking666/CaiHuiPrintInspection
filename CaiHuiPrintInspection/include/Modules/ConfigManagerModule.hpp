#pragma once

#include <QFile>
#include <QDebug>
#include"IModule.hpp"
#include<QObject>
#include "oso_StorageContext.hpp"
#include "SetConfig.hpp"
#include "CaiHuiPrintInspection.hpp"
#include "halconData.hpp"
#include "oso_func.hpp"

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
	void saveSetConfig();
public:
	std::unique_ptr<rw::oso::StorageContext> storeContext{ nullptr };
public:
	cdm::CaiHuiPrintInspectionConfig caihuiPrintInspectionConfig{};
	cdm::SetConfig setConfig{};
	QVector<HalconData>halconDatas;
public:
	template<class ConfigType>
	bool loadConfigSafe(const QString& path, ConfigType& outConfig, const QString& configName);
};

template <class ConfigType>
bool ConfigManagerModule::loadConfigSafe(const QString& path, ConfigType& outConfig, const QString& configName)
{
	if (!QFile::exists(path))
	{
		qWarning() << configName << "配置文件不存在:" << path;
		return false;
	}

	auto loadData = storeContext->loadSafe(path.toStdString());
	if (!loadData)
	{
		qWarning() << configName << "配置文件加载失败，删除:" << path;
		QFile::remove(path);
		return false;
	}

	try
	{
		outConfig = ConfigType(*loadData);
		return true;
	}
	catch (std::runtime_error&)
	{
		qWarning() << configName << "配置文件字段不匹配，尝试合并默认值...";

		rw::oso::ObjectStoreAssembly oldAssembly = *loadData;
		rw::oso::ObjectStoreAssembly newAssembly = ConfigType();
		rw::oso::AssemblyMergeTool::Merge(newAssembly, oldAssembly);

		if (storeContext->saveSafe(newAssembly, path.toStdString()))
		{
			qDebug() << configName << "配置文件更新成功:" << path;

			// 合并后重新尝试构造
			try
			{
				outConfig = ConfigType(newAssembly);
				return true;
			}
			catch (...)
			{
				qWarning() << configName << "合并后仍无法解析，使用默认值";
			}
		}
		else
		{
			qWarning() << configName << "合并后保存失败:" << path;
		}
		return false;
	}
}
