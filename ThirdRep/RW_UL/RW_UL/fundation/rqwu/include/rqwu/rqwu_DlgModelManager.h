#pragma once

#include "rqwu/rqwu_core.h"
#include"rqwu/rqwu_Dialog.h"

#include <QDialog>
#include <QLabel>
#include<QStandardItemModel>
#include<QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgModelManagerClass; };
QT_END_NAMESPACE

namespace rw
{
	namespace rqwu
	{

		struct ClassIDCfgItem
		{
			ClassIDName name;
		};

		struct DlgModelManagerConfig
		{
			QMap<ClassID, ClassIDCfgItem> vec_classid_cfg;
			QVector<ModelInfo> model_infos;
		};

		class FullKeyboard;
		class ModelManagerListModel;
		class DlgModelManager : public rqwu::Dialog
		{
			Q_OBJECT
		private:
			FullKeyboard* full_key_board;
		private:
			DlgModelManagerConfig _cfg;
			QMap<ClassID, QLabel*> _map_lbl_imgdis;
		private:
			QStandardItemModel* _model_info_model;
			ModelManagerListModel* _model_list_model;
		private:
			void resetModelListByAllItem();
		public:
			explicit DlgModelManager(const DlgModelManagerConfig & cfg,QWidget* parent = nullptr);
			~DlgModelManager() override;
		public:
			void setModelInfos(const QVector<ModelInfo> & info);
		signals:
			void deleteModel(ModelInfo info);
			void loadModel(ModelInfo info);
		private:
			void build_ui();
			void build_gBox_imgDis();
			void build_listView_modelList();
			void build_gBox_modelInfo();
		private:
			void build_connect();

		private:
			Ui::DlgModelManagerClass* ui;
		private:
			void flashModelInfoTable(qint32 index);
		private slots:
			void pbtn_exit_clicked();
			void pbtn_preModel_clicked();
			void pbtn_nextModel_clicked();
			void pbtn_searchInput_clicked();
			void pbtn_search_clicked();
			void pbtn_clear_clicked();
			void pbtn_deleteModel_clicked();
			void pbtn_loadModel_clicked();
		private slots:
			void listView_modelList_doubleClicked(const QModelIndex& index);
		private slots:
			void onCurrentListModelSelectChanged(const QModelIndex& current, const QModelIndex& previous);
		};
	}

}



