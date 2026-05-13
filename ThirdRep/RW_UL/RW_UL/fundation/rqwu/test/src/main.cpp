#include <QtWidgets/QApplication>

#include "rqwu/rqwu_core.h"
#include"rqwu/rqwu_DlgModelManager.h"

using namespace rw::rqwu;


int main(int argc, char* argv[])
{

	QApplication a(argc, argv);

	ini();
	DlgModelManagerConfig cfg;
	/*ClassIDCfgItem item1;
	item1.name = "0";
	cfg.vec_classid_cfg.insert(0, item1);
	item1.name = "1";
	cfg.vec_classid_cfg.insert(1, item1);

	ModelInfo info1;
	info1.model_param_list["Name1"] = "Value1";
	info1.model_param_list["Name2"] = 1;
	info1.model_param_list["Name3"] = 1.1f;
	info1.model_param_list["Name4"] = true;

	cfg.model_infos.push_back(info1);
	info1.name = "1";
	info1.model_param_list["Name1"] = "Value2";
	info1.model_param_list["Name2"] = 12;
	info1.model_param_list["Name3"] = 1.12f;
	info1.model_param_list["Name4"] = true;
	info1.classId = 1;
	cfg.model_infos.push_back(info1);
	info1.name = "model_2";
	info1.model_param_list["Name1"] = "Value3";
	info1.model_param_list["Name2"] = 1;
	info1.model_param_list["Name3"] = 1.1f;
	info1.model_param_list["Name4"] = true;
	info1.classId = 2;
	cfg.model_infos.push_back(info1);
	info1.name = "model_3";
	info1.model_param_list["Name1"] = "Value4";
	info1.model_param_list["Name2"] = 1;
	info1.model_param_list["Name3"] = 1.1f;
	info1.model_param_list["Name4"] = true;
	info1.classId = 3;
	cfg.model_infos.push_back(info1);
	info1.name = "model_4";
	info1.model_param_list["Name1"] = "Value5";
	info1.model_param_list["Name2"] = 1;
	info1.model_param_list["Name3"] = 1.1f;
	info1.model_param_list["Name4"] = true;
	info1.classId = 4;
	cfg.model_infos.push_back(info1);
	info1.name = "model_5";
	info1.model_param_list["Name1"] = "Value6";
	info1.model_param_list["Name2"] = 1;
	info1.model_param_list["Name3"] = 1.1f;
	info1.model_param_list["Name4"] = true;
	info1.classId = 5;
	cfg.model_infos.push_back(info1);*/

	DlgModelManager w(cfg);
	QObject::connect(&w, &DlgModelManager::deleteModel, [](ModelInfo  info)
		{
			qDebug() << "info" << info.name;
		});
	QObject::connect(&w, &DlgModelManager::loadModel, [](ModelInfo  info)
		{
			qDebug() << "info" << info.name;
		});

	w.show();


	return a.exec();
}
