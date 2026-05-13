#include "DataSetAnalysis.h"

#include <QFileDialog>
#include <QMessageBox>
#include <qtconcurrentrun.h>
#include <QPointer>
#include <QTimer>

#include "DatasetDivide.hpp"
#include "DatasetImport.hpp"
#include "ui_DataSetAnalysis.h"

DataSetAnalysis::DataSetAnalysis(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DataSetAnalysisClass())
{
    ui->setupUi(this);
    build_UI();
    build_Connect();
}

DataSetAnalysis::~DataSetAnalysis()
{
    delete ui;
}

void DataSetAnalysis::build_UI()
{
	ui->tabWidget->tabBar()->hide();
}

void DataSetAnalysis::build_Connect()
{
    QObject::connect(ui->pbtn_Exit, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_Exit_clicked);
    QObject::connect(ui->pbtn_ImportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_ImportDatas_clicked);
	QObject::connect(ui->pbtn_StartImportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_StartImportDatas_clicked);
	QObject::connect(ui->pbtn_divideByClassid, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_divideByClassid_clicked);
	QObject::connect(ui->pbtn_divideByRandom, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_divideByRandom_clicked);
	QObject::connect(ui->pbtn_divideByProportion, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_divideByProportion_clicked);
	QObject::connect(ui->pbtn_divideByClassidExportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_divideByClassidExportDatas_clicked);
	QObject::connect(ui->pbtn_startdivideByClassidExportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_startdivideByClassidExportDatas_clicked);
	QObject::connect(ui->pbtn_divideByRandomExportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_divideByRandomExportDatas_clicked);
	QObject::connect(ui->pbtn_startdivideByRandomExportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_startdivideByRandomExportDatas_clicked);
	QObject::connect(ui->pbtn_divideByProportionExportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_divideByProportionExportDatas_clicked);
	QObject::connect(ui->pbtn_startdivideByProportionExportDatas, &QPushButton::clicked, this, &DataSetAnalysis::pbtn_startdivideByProportionExportDatas_clicked);



}

void DataSetAnalysis::pbtn_Exit_clicked()
{
    this->close();
}

void DataSetAnalysis::pbtn_ImportDatas_clicked()
{
	// 选择数据集文件夹
	const QString selectedDir = QFileDialog::getExistingDirectory(
		this,
		tr("选择数据集文件夹"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (selectedDir.isEmpty())
	{
		return;
	}

	// 你的需求：将路径复制到 path 里（此处用局部变量）
	const QString path = QDir::toNativeSeparators(selectedDir);

	// 可选：如果 UI 里有用于显示路径的输入框，优先写进去
	// 备注：这里尝试兼容不同命名的控件（存在则写入，不存在不影响编译）
	if (ui->ledit_DataSetPath)
	{
		ui->ledit_DataSetPath->setText(path);
	}


}

void DataSetAnalysis::pbtn_StartImportDatas_clicked()
{

	// 校验路径
	const QString datasetPath = ui->ledit_DataSetPath ? ui->ledit_DataSetPath->text().trimmed() : QString();
	if (datasetPath.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请先选择数据集路径。"));
		return;
	}

	const QFileInfo fi(datasetPath);
	if (!fi.exists() || !fi.isDir())
	{
		QMessageBox::warning(this, tr("警告"), tr("数据集路径不存在或不是文件夹：\n%1").arg(datasetPath));
		return;
	}

	// 切换到导入数据标签页（第二个Tab，index=1）
	if (ui->tabWidget)
	{
		ui->tabWidget->setCurrentIndex(1);
	}

	// 清理旧结果（避免叠加）
	classIdToPath.clear();
	dataSetPathToClassIds.clear();
	unknownImgPathSet.clear();

	// 进度条动画
	QPointer<QTimer> progressTimer(new QTimer(this));
	if (ui->progressBar_ImportDatas)
	{
		ui->progressBar_ImportDatas->setRange(0, 100);
		ui->progressBar_ImportDatas->setValue(0);
		QObject::connect(progressTimer, &QTimer::timeout, this, [this]() {
			if (!ui->progressBar_ImportDatas)
			{
				return;
			}
			int v = ui->progressBar_ImportDatas->value();
			v = (v + 5) % 101;
			ui->progressBar_ImportDatas->setValue(v);
			});
		progressTimer->start(120);
	}

	// 独立 watcher，配合本次进度动画
	auto* watcher = new QFutureWatcher<bool>(this);
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, progressTimer]() {
		if (progressTimer)
		{
			progressTimer->stop();
			progressTimer->deleteLater();
		}

		const bool ok = watcher->result();

		if (ui->progressBar_ImportDatas)
		{
			ui->progressBar_ImportDatas->setRange(0, 100);
			ui->progressBar_ImportDatas->setValue(ok ? 100 : 0);
		}

		if (!ok)
		{
			QMessageBox::warning(this, tr("警告"), tr("导入失败：数据集结构不正确（需要 images/ 和 labels/）。"));
			watcher->deleteLater();
			return;
		}
		else
		{
			QMessageBox::information(this, tr("信息"), tr("处理完成"));
		}

		if (ui->tabWidget)
		{
			ui->tabWidget->setCurrentIndex(2);
		}

		watcher->deleteLater();
		});

	const QString pathCopy = datasetPath;
	DataSetType dataSetType;
	if (ui->cmb_SelectType->currentIndex() == 0)
	{
		dataSetType = DataSetType::YolODetection;

	}
	else if (ui->cmb_SelectType->currentIndex() == 1)
	{
		dataSetType = DataSetType::YOlOSegmentation;

	}
	else if (ui->cmb_SelectType->currentIndex() == 2)
	{
		dataSetType = DataSetType::YOLOOrientedBounding;

	}
	else
	{
		dataSetType = DataSetType::YolODetection;

	}

	auto future = QtConcurrent::run([this, pathCopy, dataSetType]() -> bool {
		return DatasetImport::getImportIndex(
			pathCopy,
			classIdToPath,
			dataSetPathToClassIds,
			unknownImgPathSet,
			dataSetType
		);
		});
	watcher->setFuture(future);

}

void DataSetAnalysis::pbtn_divideByClassid_clicked()
{
	// 切换到导入数据标签页（第二个Tab，index=1）
	if (ui->tabWidget)
	{
		ui->tabWidget->setCurrentIndex(3);
	}

}

void DataSetAnalysis::pbtn_divideByRandom_clicked()
{
	// 切换到导入数据标签页（第二个Tab，index=1）
	if (ui->tabWidget)
	{
		ui->tabWidget->setCurrentIndex(4);
	}
}

void DataSetAnalysis::pbtn_divideByProportion_clicked()
{
	// 切换到导入数据标签页（第二个Tab，index=1）
	if (ui->tabWidget)
	{
		ui->tabWidget->setCurrentIndex(5);
	}
}

void DataSetAnalysis::pbtn_divideByClassidExportDatas_clicked()
{
	// 选择数据集文件夹
	const QString selectedDir = QFileDialog::getExistingDirectory(
		this,
		tr("选择数据集文件夹"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (selectedDir.isEmpty())
	{
		return;
	}

	// 你的需求：将路径复制到 path 里（此处用局部变量）
	const QString path = QDir::toNativeSeparators(selectedDir);

	// 可选：如果 UI 里有用于显示路径的输入框，优先写进去
	// 备注：这里尝试兼容不同命名的控件（存在则写入，不存在不影响编译）
	if (ui->ledit_divideByClassidDataSetPath)
	{
		ui->ledit_divideByClassidDataSetPath->setText(path);
	}


}

void DataSetAnalysis::pbtn_startdivideByClassidExportDatas_clicked()
{
	// 校验路径
	const QString datasetPath = ui->ledit_divideByClassidDataSetPath ? ui->ledit_divideByClassidDataSetPath->text().trimmed() : QString();
	if (datasetPath.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请先选择数据集路径。"));
		return;
	}

	const QFileInfo fi(datasetPath);
	if (!fi.exists() || !fi.isDir())
	{
		QMessageBox::warning(this, tr("警告"), tr("数据集路径不存在或不是文件夹：\n%1").arg(datasetPath));
		return;
	}

	// 进度条动画
	QPointer<QTimer> progressTimer(new QTimer(this));
	if (ui->progressBar_divideByClassid)
	{
		ui->progressBar_divideByClassid->setRange(0, 100);
		ui->progressBar_divideByClassid->setValue(0);
		QObject::connect(progressTimer, &QTimer::timeout, this, [this]() {
			if (!ui->progressBar_divideByClassid)
			{
				return;
			}
			int v = ui->progressBar_divideByClassid->value();
			v = (v + 5) % 101;
			ui->progressBar_divideByClassid->setValue(v);
			});
		progressTimer->start(120);
	}

	// 独立 watcher，避免复用冲突
	auto* watcher = new QFutureWatcher<bool>(this);
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, progressTimer]() {
		if (progressTimer)
		{
			progressTimer->stop();
			progressTimer->deleteLater();
		}
		if (ui->progressBar_divideByClassid)
		{
			ui->progressBar_divideByClassid->setRange(0, 100);
			ui->progressBar_divideByClassid->setValue(100);
		}
		QMessageBox::information(this, tr("完成"), tr("按classid的方式划分数据集已完成！"));
		watcher->deleteLater();
		});

	// 异步执行分割
	auto future = QtConcurrent::run([this, datasetPath]() -> bool {
		DatasetDivide::divideByClassid(classIdToPath, datasetPath);
		return true;
		});
	watcher->setFuture(future);
}

void DataSetAnalysis::pbtn_divideByRandomExportDatas_clicked()
{
	// 选择数据集文件夹
	const QString selectedDir = QFileDialog::getExistingDirectory(
		this,
		tr("选择数据集文件夹"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (selectedDir.isEmpty())
	{
		return;
	}

	// 你的需求：将路径复制到 path 里（此处用局部变量）
	const QString path = QDir::toNativeSeparators(selectedDir);

	// 可选：如果 UI 里有用于显示路径的输入框，优先写进去
	// 备注：这里尝试兼容不同命名的控件（存在则写入，不存在不影响编译）
	if (ui->ledit_divideByRandomDataSetPath)
	{
		ui->ledit_divideByRandomDataSetPath->setText(path);
	}
}

void DataSetAnalysis::pbtn_startdivideByRandomExportDatas_clicked()
{
	
	// 校验路径
	const QString datasetPath = ui->ledit_divideByRandomDataSetPath ? ui->ledit_divideByRandomDataSetPath->text().trimmed() : QString();
	if (datasetPath.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请先选择数据集路径。"));
		return;
	}

	const QFileInfo fi(datasetPath);
	if (!fi.exists() || !fi.isDir())
	{
		QMessageBox::warning(this, tr("警告"), tr("数据集路径不存在或不是文件夹：\n%1").arg(datasetPath));
		return;
	}

	const QString trainText = ui->ledit_divideByRandomTrain ? ui->ledit_divideByRandomTrain->text().trimmed() : QString();
	const QString valText = ui->ledit_divideByRandomVal ? ui->ledit_divideByRandomVal->text().trimmed() : QString();
	const QString testText = ui->ledit_divideByRandomTest ? ui->ledit_divideByRandomTest->text().trimmed() : QString();

	bool okTrain = false, okVal = false, okTest = false;
	double all = trainText.toDouble(&okTrain) + valText.toDouble(&okVal) + testText.toDouble(&okTest);
	if (!okTrain || !okVal || !okTest) {
		QMessageBox::warning(this, tr("警告"), tr("训练、验证、测试集比例必须为数字且不能为空。"));
		return;
	}
	const double trainRatio = trainText.toDouble(&okTrain) / all;
	const double valRatio = valText.toDouble(&okVal) / all;
	const double testRatio = testText.toDouble(&okTest) / all;

	// 进度条动画（本次独立）
	QPointer<QTimer> progressTimer(new QTimer(this));
	if (ui->progressBar_divideByRandom)
	{
		ui->progressBar_divideByRandom->setRange(0, 100);
		ui->progressBar_divideByRandom->setValue(0);
		QObject::connect(progressTimer, &QTimer::timeout, this, [this]() {
			if (!ui->progressBar_divideByRandom)
			{
				return;
			}
			int v = ui->progressBar_divideByRandom->value();
			v = (v + 5) % 101;
			ui->progressBar_divideByRandom->setValue(v);
			});
		progressTimer->start(120);
	}

	// 独立 watcher，避免复用冲突
	auto* watcher = new QFutureWatcher<bool>(this);
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, progressTimer]() {
		if (progressTimer)
		{
			progressTimer->stop();
			progressTimer->deleteLater();
		}
		if (ui->progressBar_divideByRandom)
		{
			ui->progressBar_divideByRandom->setRange(0, 100);
			ui->progressBar_divideByRandom->setValue(100);
		}
		QMessageBox::information(this, tr("完成"), tr("按随机方式划分数据集已完成！"));
		watcher->deleteLater();
		});

	// 异步执行分割
	auto future = QtConcurrent::run([this, datasetPath, trainRatio, valRatio, testRatio]() -> bool {
		DivideByRandomCfg cfg;
		cfg.proportionCfg.trainProportion = trainRatio;
		cfg.proportionCfg.valProportion = valRatio;
		cfg.proportionCfg.testProportion = testRatio;
		DatasetDivide::divideByRandom(classIdToPath, dataSetPathToClassIds, cfg, datasetPath);
		return true;
		});
	watcher->setFuture(future);
}

void DataSetAnalysis::pbtn_divideByProportionExportDatas_clicked()
{
	// 选择数据集文件夹
	const QString selectedDir = QFileDialog::getExistingDirectory(
		this,
		tr("选择数据集文件夹"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (selectedDir.isEmpty())
	{
		return;
	}

	// 你的需求：将路径复制到 path 里（此处用局部变量）
	const QString path = QDir::toNativeSeparators(selectedDir);

	// 可选：如果 UI 里有用于显示路径的输入框，优先写进去
	// 备注：这里尝试兼容不同命名的控件（存在则写入，不存在不影响编译）
	if (ui->ledit_divideByProportionDataSetPath)
	{
		ui->ledit_divideByProportionDataSetPath->setText(path);
	}
}

void DataSetAnalysis::pbtn_startdivideByProportionExportDatas_clicked()
{

	// 校验路径
	const QString datasetPath = ui->ledit_divideByProportionDataSetPath ? ui->ledit_divideByProportionDataSetPath->text().trimmed() : QString();
	if (datasetPath.isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请先选择数据集路径。"));
		return;
	}

	const QFileInfo fi(datasetPath);
	if (!fi.exists() || !fi.isDir())
	{
		QMessageBox::warning(this, tr("警告"), tr("数据集路径不存在或不是文件夹：\n%1").arg(datasetPath));
		return;
	}

	const QString trainText = ui->ledit_divideByProportionTrain ? ui->ledit_divideByProportionTrain->text().trimmed() : QString();
	const QString valText = ui->ledit_divideByProportionVal ? ui->ledit_divideByProportionVal->text().trimmed() : QString();
	const QString testText = ui->ledit_divideByProportionTest ? ui->ledit_divideByProportionTest->text().trimmed() : QString();

	bool okTrain = false, okVal = false, okTest = false;
	double all = trainText.toDouble(&okTrain) + valText.toDouble(&okVal) + testText.toDouble(&okTest);
	if (!okTrain || !okVal || !okTest) {
		QMessageBox::warning(this, tr("警告"), tr("训练、验证、测试集比例必须为数字且不能为空。"));
		return;
	}
	const double trainRatio = trainText.toDouble(&okTrain) / all;
	const double valRatio = valText.toDouble(&okVal) / all;
	const double testRatio = testText.toDouble(&okTest) / all;

	struct DivideByQuantityCfg cfg;
	cfg.proportionCfg.trainProportion = trainRatio;
	cfg.proportionCfg.valProportion = valRatio;
	cfg.proportionCfg.testProportion = testRatio;

	const QString missText = ui->ledit_divideByProportionMissClassid ? ui->ledit_divideByProportionMissClassid->text().trimmed() : QString();
	if (!missText.isEmpty())
	{
		const QStringList parts = missText.split(QRegularExpression("[,，]"), Qt::SkipEmptyParts);
		for (const auto& part : parts)
		{
			bool ok = false;
			const int classId = part.trimmed().toInt(&ok);
			if (!ok)
			{
				QMessageBox::warning(this, tr("警告"), tr("忽略的类别ID必须为整数，当前值：%1").arg(part.trimmed()));
				return;
			}
			cfg.ignoreClassIds.push_back(classId);
		}
	}

	cfg.isSmallPriority = (ui->cmb_divideByProportionSelectType->currentIndex() == 0);

	// 进度条动画（本次独立）
	QPointer<QTimer> progressTimer(new QTimer(this));
	if (ui->progressBar_divideByProportion)
	{
		ui->progressBar_divideByProportion->setRange(0, 100);
		ui->progressBar_divideByProportion->setValue(0);
		QObject::connect(progressTimer, &QTimer::timeout, this, [this]() {
			if (!ui->progressBar_divideByProportion)
			{
				return;
			}
			int v = ui->progressBar_divideByProportion->value();
			v = (v + 5) % 101;
			ui->progressBar_divideByProportion->setValue(v);
			});
		progressTimer->start(120);
	}

	// 独立 watcher
	auto* watcher = new QFutureWatcher<bool>(this);
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, progressTimer]() {
		if (progressTimer)
		{
			progressTimer->stop();
			progressTimer->deleteLater();
		}
		if (ui->progressBar_divideByProportion)
		{
			ui->progressBar_divideByProportion->setRange(0, 100);
			ui->progressBar_divideByProportion->setValue(100);
		}
		QMessageBox::information(this, tr("完成"), tr("按比例方式划分数据集已完成！"));
		watcher->deleteLater();
		});

	// 异步执行分割
	auto future = QtConcurrent::run([this, datasetPath, cfg]() -> bool {
		DatasetDivide::divideByQuantity(classIdToPath, dataSetPathToClassIds, cfg, datasetPath);
		return true;
		});
	watcher->setFuture(future);
}


