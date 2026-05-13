#include"rqwu/rqwu_p_DlgModelManager.h"

namespace rw
{
	namespace rqwu
	{
		ModelManagerListModel::ModelManagerListModel(QObject* parent)
			: QAbstractListModel(parent)
		{

		}

		int ModelManagerListModel::rowCount(const QModelIndex& parent) const
		{
			Q_UNUSED(parent);
			return _model_infos.size();
		}

		QVariant ModelManagerListModel::data(const QModelIndex& index, int role) const
		{
			if (!index.isValid() || index.row() < 0 || index.row() >= _model_infos.size())
			{
				return QVariant();
			}
			if (role == Qt::DisplayRole)
			{
				const auto& modelInfo = _model_infos.at(index.row());
				if (modelInfo.second.name.isEmpty())
				{
					return QString("未命名");
				}
				else
				{
					return modelInfo.second.name;
				}
			}
			return QVariant();
		}

		const ModelInfoItem& ModelManagerListModel::getModelInfoAt(int row) const
		{
			return _model_infos.at(row);
		}

		void ModelManagerListModel::setModelInfoAt(int row, const ModelInfoItem& info)
		{
			if (row < 0 || row >= _model_infos.size())
			{
				return;
			}
			_model_infos[row] = info;
			emit dataChanged(index(row, 0), index(row, 0));
		}

		void ModelManagerListModel::setModelInfos(const QList<ModelInfoItem>& infos)
		{
			beginResetModel();
			_model_infos = infos;
			endResetModel();
		}

		qint32 ModelManagerListModel::size() const
		{
			return _model_infos.size();
		}
	}

}

