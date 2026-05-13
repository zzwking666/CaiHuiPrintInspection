#include "rqwu/rqwu_DlgModelManager.h"

#include "rqwu/rqwu_p_DlgModelManager.h"

#include "ui_rqwu_DlgModelManager.h"

#include "rqwu/Keyboard/rqwu_FullKeyBoard.h"
#include "rqwu/rqwu_MessageBox.h"

#include <QLabel>
#include <QGroupBox>

namespace rw
{
	namespace rqwu
	{
		void DlgModelManager::resetModelListByAllItem()
		{
			QList<ModelInfoItem> items;
			for (qint32 i = 0; i < _cfg.model_infos.size(); i++)
			{
				items.append(qMakePair(i, _cfg.model_infos.at(i)));
			}
			_model_list_model->setModelInfos(items);
		}

		// Constructor: Initializes the dialog with configuration and parent widget
		DlgModelManager::DlgModelManager(const DlgModelManagerConfig& cfg,QWidget* parent)
			: rqwu::Dialog(parent),_cfg(cfg)
			, ui(new Ui::DlgModelManagerClass())
		{
			ui->setupUi(this);
			build_ui();
			build_connect();
		}

		// Destructor: Cleans up UI resources
		DlgModelManager::~DlgModelManager()
		{
			delete ui;
		}

		void DlgModelManager::setModelInfos(const QVector<ModelInfo>& info)
		{
			_cfg.model_infos = info;
			resetModelListByAllItem();
			if (_model_list_model->size() > 0)
			{
				ui->listView_modelList->setCurrentIndex(_model_list_model->index(0, 0));
			}
		}

		// Build the main UI components
		void DlgModelManager::build_ui()
		{
			full_key_board = new FullKeyboard(this);
			full_key_board->emptyInputPolicy = Keyboard::EmptyInputPolicy::EnableAndAccept;

			build_gBox_imgDis();
			build_listView_modelList();
			build_gBox_modelInfo();
		}

		// Build the image display group box for each class id
		void DlgModelManager::build_gBox_imgDis()
		{
			auto& imgDisGBox = ui->gBox_imgDis;

			QHBoxLayout* mainLayout = new QHBoxLayout(imgDisGBox);

			for (auto it = _cfg.vec_classid_cfg.constBegin(); it != _cfg.vec_classid_cfg.constEnd(); ++it)
			{
				const auto& key = it.key();
				const auto& value = it.value();

				QGroupBox* gbox = new QGroupBox(imgDisGBox);
				gbox->setTitle(value.name);

				QVBoxLayout* gboxLayout = new QVBoxLayout(gbox);
				QLabel* lbl = new QLabel(gbox);
				lbl->setText("Display Img for " + value.name);
				_map_lbl_imgdis.insert(key, lbl);
				gboxLayout->addWidget(lbl);

				mainLayout->addWidget(gbox);
			}

			imgDisGBox->setLayout(mainLayout);
		}

		// Build the model list view and set its model
		void DlgModelManager::build_listView_modelList()
		{
			_model_list_model = new ModelManagerListModel( this);
			resetModelListByAllItem();

			ui->listView_modelList->setModel(_model_list_model);

			// Set index 0 as current if available
			if (_model_list_model->size() > 0)
			{
				ui->listView_modelList->setCurrentIndex(_model_list_model->index(0, 0));
			}
		}

		// Build the model info group box and display info for the first model
		void DlgModelManager::build_gBox_modelInfo()
		{
			if (_model_list_model->size()==0)
			{
				return;
			}

			// Set table view properties
			ui->tableView_modelInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
			ui->tableView_modelInfo->setSelectionMode(QAbstractItemView::NoSelection);
			ui->tableView_modelInfo->setFocusPolicy(Qt::NoFocus);

			flashModelInfoTable(0);
		}

		// Connect UI signals to their respective slots
		void DlgModelManager::build_connect()
		{
			QObject::connect(ui->pbtn_exit, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_exit_clicked);
			QObject::connect(ui->pbtn_preModel, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_preModel_clicked);
			QObject::connect(ui->pbtn_nextModel, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_nextModel_clicked);
			QObject::connect(ui->listView_modelList->selectionModel(), &QItemSelectionModel::currentChanged,
				this, &DlgModelManager::onCurrentListModelSelectChanged);
			QObject::connect(ui->pbtn_searchInput, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_searchInput_clicked);
			QObject::connect(ui->pbtn_search, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_search_clicked);
			QObject::connect(ui->pbtn_clear, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_clear_clicked);
			QObject::connect(ui->pbtn_deleteModel, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_deleteModel_clicked);
			QObject::connect(ui->pbtn_loadModel, &QPushButton::clicked,
				this, &DlgModelManager::pbtn_loadModel_clicked);
			QObject::connect(ui->listView_modelList, &QListView::doubleClicked,
				this, &DlgModelManager::listView_modelList_doubleClicked);
		}

		// Refresh the model info table for the given index
		void DlgModelManager::flashModelInfoTable(qint32 index)
		{
			auto firstModelInfo = _model_list_model->getModelInfoAt(index);
			auto param_names = firstModelInfo.second.model_param_list.keys();
			auto param_list = firstModelInfo.second.model_param_list;
			_model_info_model = new QStandardItemModel(param_names.size(), 1, this);
			ui->tableView_modelInfo->horizontalHeader()->hide();
			_model_info_model->setHeaderData(0, Qt::Horizontal, QVariant());
			ui->tableView_modelInfo->setModel(_model_info_model);


			_model_info_model->setVerticalHeaderLabels(param_names);
			ui->tableView_modelInfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);


			for (int row = 0; row < param_names.size(); ++row)
			{
				const QString& param_name = param_names.at(row);
				const QVariant& param_value = param_list[param_name];
				QStandardItem* item = new QStandardItem(param_value.toString());
				_model_info_model->setItem(row, 0, item);
			}
		}

		// Slot: Handle exit button click
		void DlgModelManager::pbtn_exit_clicked()
		{
			this->close();
		}

		// Slot: Handle previous model button click
		void DlgModelManager::pbtn_preModel_clicked()
		{
			auto maxIndex = _model_list_model->size();
			auto currentIndex =  ui->listView_modelList->currentIndex();
			if (maxIndex == 0)
			{
				return;
			}
			if (!currentIndex.isValid())
			{
				return;
			}
			ui->listView_modelList->setCurrentIndex(currentIndex.siblingAtColumn(0).siblingAtRow((currentIndex.row() - 1 + maxIndex) % maxIndex));
		}

		// Slot: Handle next model button click
		void DlgModelManager::pbtn_nextModel_clicked()
		{
			auto maxIndex = _model_list_model->size();
			auto currentIndex = ui->listView_modelList->currentIndex();
			if (maxIndex == 0)
			{
				return;
			}
			if (!currentIndex.isValid())
			{
				return;
			}
			ui->listView_modelList->setCurrentIndex(currentIndex.siblingAtColumn(0).siblingAtRow((currentIndex.row() + 1) % maxIndex));
		}

		// Slot: Handle search input button click, show keyboard dialog
		void DlgModelManager::pbtn_searchInput_clicked()
		{
			auto isAcceptResult= full_key_board->exec();
			if (isAcceptResult!=QDialog::Accepted)
			{
				return;
			}

			auto inputText = full_key_board->getValue();
			ui->pbtn_searchInput->setText(inputText);
		}

		// Slot: Handle search button click, filter model list
		void DlgModelManager::pbtn_search_clicked()
		{
			if (ui->pbtn_searchInput->text().isEmpty())
			{
				resetModelListByAllItem();
				if (_model_list_model->size() > 0)
				{
					ui->listView_modelList->setCurrentIndex(_model_list_model->index(0, 0));
				}
				return;
			}

			QString inputStr = ui->pbtn_searchInput->text();

			QList<ModelInfoItem> filtered;
			for (qint32 i=0;i< _cfg.model_infos.size();i++)
			{
				auto& modelInfo = _cfg.model_infos.at(i);
				if (modelInfo.name.contains(inputStr, Qt::CaseInsensitive))
				{
					filtered.append(qMakePair(i, modelInfo));
				}
			}
			_model_list_model->setModelInfos(filtered);

			if (_model_list_model->size() > 0)
			{
				ui->listView_modelList->setCurrentIndex(_model_list_model->index(0, 0));
			}
			else
			{
				_model_info_model->clear();
			}

		}

		// Slot: Handle clear button click, reset search and model list
		void DlgModelManager::pbtn_clear_clicked()
		{
			ui->pbtn_searchInput->setText("");
			resetModelListByAllItem();
			if (_model_list_model->size() > 0)
			{
				ui->listView_modelList->setCurrentIndex(_model_list_model->index(0, 0));
			}
		}

		void DlgModelManager::pbtn_deleteModel_clicked()
		{
			auto currentIndex = ui->listView_modelList->currentIndex();
			if (!currentIndex.isValid())
			{
				return;
			}
			auto modelInfoItem = _model_list_model->getModelInfoAt(currentIndex.row());
			const QString modelName = modelInfoItem.second.name;
			auto ret = rqwu::MessageBox::question(this, tr("删除模型"), tr("确定要删除模型 \"%1\" 吗？").arg(modelName));
			if (ret != rqwu::MessageBox::StandardButton::Yes)
			{
				return;
			}
			_cfg.model_infos.removeAt(modelInfoItem.first);
			emit deleteModel(modelInfoItem.second);
			resetModelListByAllItem();
			if (_model_list_model->size() > 0)
			{
				ui->listView_modelList->setCurrentIndex(_model_list_model->index(0, 0));
			}
			else
			{
				_model_info_model->clear();
			}
		}

		void DlgModelManager::pbtn_loadModel_clicked()
		{
			auto currentIndex = ui->listView_modelList->currentIndex();
			if (!currentIndex.isValid())
			{
				return;
			}
			auto modelInfoItem = _model_list_model->getModelInfoAt(currentIndex.row());
			emit loadModel(modelInfoItem.second);
		}

		void DlgModelManager::listView_modelList_doubleClicked(const QModelIndex& index)
		{
			auto isAcceptResult = full_key_board->exec();
			if (isAcceptResult != QDialog::Accepted)
			{
				return;
			}

			auto inputText = full_key_board->getValue();
			if (inputText.isEmpty())
			{
				return;
			}

			auto modelInfoItem = _model_list_model->getModelInfoAt(index.row());
			const QString oldName = modelInfoItem.second.name;
			const QString newName = inputText;

			for (const auto& cfgModel : _cfg.model_infos)
			{
				if (cfgModel.name.compare(oldName, Qt::CaseInsensitive) == 0)
				{
					continue;
				}

				if (cfgModel.name.compare(newName, Qt::CaseInsensitive) == 0)
				{
					rqwu::MessageBox::warning(this, tr("重名"), tr("名称与现有模型重复，请使用不同的名称。"));
					return;
				}
			}

			modelInfoItem.second.name = newName;
			_model_list_model->setModelInfoAt(index.row(), modelInfoItem);
			_cfg.model_infos[modelInfoItem.first] = modelInfoItem.second;
		}

		// Slot: Handle model list selection change, update info table
		void DlgModelManager::onCurrentListModelSelectChanged(const QModelIndex& current, const QModelIndex& previous)
		{
			if (!current.isValid()) {
				return;
			}

			int selectedIndex = current.row();

			flashModelInfoTable(selectedIndex);
		}
	}
}



