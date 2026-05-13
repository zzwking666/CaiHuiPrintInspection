#pragma once

#include <QtWidgets/QMainWindow>


#include <QFutureWatcher>

#include "IndexType.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class DataSetAnalysisClass; };
QT_END_NAMESPACE

class DataSetAnalysis : public QMainWindow
{
	Q_OBJECT

public:
	DataSetAnalysis(QWidget* parent = nullptr);
	~DataSetAnalysis();

	ClassIdToPath classIdToPath;
	DataSetPathToClassIds dataSetPathToClassIds;
	UnknownImgPathSet unknownImgPathSet;

private:
	Ui::DataSetAnalysisClass* ui;
	void build_UI();
	void build_Connect();

private:
	QFutureWatcher<bool>* m_importWatcher{};
	QFutureWatcher<bool>*m_divideByRandomWatcher{};

public slots:
	void pbtn_Exit_clicked();
	void pbtn_ImportDatas_clicked();
	void pbtn_StartImportDatas_clicked();
	void pbtn_divideByClassid_clicked();
	void pbtn_divideByRandom_clicked();
	void pbtn_divideByProportion_clicked();
	void pbtn_divideByClassidExportDatas_clicked();
	void pbtn_startdivideByClassidExportDatas_clicked();
	void pbtn_divideByRandomExportDatas_clicked();
	void pbtn_startdivideByRandomExportDatas_clicked();
	void pbtn_divideByProportionExportDatas_clicked();
	void pbtn_startdivideByProportionExportDatas_clicked();
};