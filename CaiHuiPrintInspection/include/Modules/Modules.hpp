#pragma once

#include"IModule.hpp"

#include"CameraModule.hpp"
#include"ConfigManagerModule.hpp"
#include "EliminateModule.hpp"
#include "ImgProModule.hpp"
#include "ImgSaveModule.hpp"
#include "oso_func.hpp"
#include "RuntimeInfoModule.hpp"
#include "UIModule.hpp"
#include "ReconnectModule.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "TestModule.hpp"
#include "WarningModule.hpp"

class Modules
	:public IModule<bool>
{

#pragma region Construction
public:
	static Modules& getInstance()
	{
		static Modules instance;
		return instance;
	}

	Modules(const Modules&) = delete;
	Modules& operator=(const Modules&) = delete;

private:
	Modules();
	~Modules() override;

public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
	void connect();
#pragma endregion
public:
	static bool check();
	template<class TypeCanToAssembly>
	static void checkFileExistAndFormat(const QString& path, const rw::oso::StorageContext& context);
	static bool EnsureDirectoryExists(const QString& dirPath);

public:
	CameraModule cameraModule;
	ReconnectModule reconnectModule;
	UIModule uiModule;
	ConfigManagerModule configManagerModule;
	RuntimeInfoModule runtimeInfoModule;
	ImgSaveModule imgSaveModule;
	ImgProModule imgProModule;
	EliminateModule eliminateModule;
#ifdef BUILD_WITHOUT_HARDWARE
public:
	TestModule test_module;
#endif

};

template <class TypeCanToAssembly>
void Modules::checkFileExistAndFormat(const QString& path, const rw::oso::StorageContext& context)
{
	if (rw::rqw::RunEnvCheck::isFileExist(path))
	{
		if (!rw::rqw::RunEnvCheck::isFileFormatCorrectWithSafe<TypeCanToAssembly>(path, context))
		{
			auto assembly = context.load(path.toStdString());
			bool isMerge{ false };
			auto mergeAssembly = rw::oso::AssemblyMergeTool::Merge(TypeCanToAssembly(), *assembly, isMerge);
			if (isMerge)
			{
				context.saveSafe(mergeAssembly, path.toStdString());
			}
			else
			{
				context.saveSafe(TypeCanToAssembly(), path.toStdString());
			}
		}
	}
	else
	{
		context.saveSafe(TypeCanToAssembly(), path.toStdString());
	}
}

