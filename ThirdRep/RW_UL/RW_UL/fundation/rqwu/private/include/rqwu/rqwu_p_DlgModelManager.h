#pragma once

#include"rqwu/rqwu_core.h"

#include <QAbstractListModel>

namespace rw
{
	namespace rqwu
	{
		class ModelManagerListModel 
			: public QAbstractListModel
		{
			Q_OBJECT
		public:
			ModelManagerListModel( QObject* parent = nullptr);
				
		public:
			int rowCount(const QModelIndex& parent = QModelIndex()) const override;
			
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
			
		public:
			const ModelInfoItem& getModelInfoAt(int row) const;
			void setModelInfoAt(int row, const ModelInfoItem& info);

			void setModelInfos(const QList<ModelInfoItem>& infos);

		public:
			qint32 size() const;
			
		private:
			QList<ModelInfoItem> _model_infos;
		};
	}

}
