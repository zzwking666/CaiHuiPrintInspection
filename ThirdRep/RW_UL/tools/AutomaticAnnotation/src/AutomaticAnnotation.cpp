#include "AutomaticAnnotation.h"

#include"NumberKeyboard.h"

#include"rqw_ImagePainter.h"

#include "ui_AutomaticAnnotation.h"

#include<QMessageBox>
#include<QFileDialog>
#include<QDirIterator>

static QStringList getAllImagePaths(const QString& path)
{
	QString folderPath = path; // 获取目录路径
	if (folderPath.isEmpty()) {
		return QStringList();
	}

	QDir dir(folderPath);
	if (!dir.exists()) {
		return QStringList();
	}

	// 定义支持的图片格式
	QStringList imageFilters = { "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff", "*.webp" };
	QStringList imagePaths;

	// 使用 QDirIterator 递归遍历目录
	QDirIterator it(folderPath, imageFilters, QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		imagePaths.append(it.next()); // 获取图片的绝对路径
	}

	return imagePaths;
}

AutomaticAnnotation::AutomaticAnnotation(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::AutomaticAnnotationClass())
{
	ui->setupUi(this);

	build_ui();
	build_connect();
}

AutomaticAnnotation::~AutomaticAnnotation()
{
	for (int i = 0; i < threads.size(); i++)
	{
		disconnect(threads[i], &AutomaticAnnotationThread::imageProcessed, this, &AutomaticAnnotation::displayImage);
	}

	for (int i = 0; i < threads.size(); i++) {
		threads[i]->wait();
		delete threads[i];
	}
	delete ui;
}

void AutomaticAnnotation::build_ui()
{
	ui->progressBar->setValue(0);
	ui->tabWidget->tabBar()->hide();
	ui->tabWidget->setCurrentIndex(0);
	viewer = new PicturesViewer(this);

	ui->cBox_checkDeployType->addItem("TensorRT");
	ui->cBox_checkDeployType->addItem("OnnxRuntime");
	ui->cBox_checkDeployType->setCurrentIndex(0);

	ui->cBox_checkModelType->addItem("Yolov11_det");
	ui->cBox_checkModelType->addItem("Yolov11_seg");
	ui->cBox_checkModelType->addItem("Yolov11_obb");
	ui->cBox_checkModelType->addItem("Yolov11_segWithMask");
	ui->cBox_checkModelType->setCurrentIndex(0);

	ui->cBox_exportLabelType->addItem("Detection");
	ui->cBox_exportLabelType->addItem("Segment");
	ui->cBox_exportLabelType->addItem("OrientedBoundingBoxes");
	ui->cBox_exportLabelType->setCurrentIndex(0);
}

void AutomaticAnnotation::build_connect()
{
	QObject::connect(ui->pbtn_setImageInput, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_setImageInput_clicked);
	QObject::connect(ui->pbtn_setAllOutput, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_setAllOutput_clicked);
	QObject::connect(ui->pbtn_setModelPath, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_setModelPath_clicked);
	QObject::connect(ui->pbtn_setWorkers, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_setWorkers_clicked);
	QObject::connect(ui->pbtn_setConfThreshold, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_setConfThreshold_clicked);
	QObject::connect(ui->pbtn_nmsThreshold, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_nmsThreshold_clicked);
	QObject::connect(ui->pbtn_exit, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_exit_clicked);
	QObject::connect(ui->pbtn_LookImage, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_LookImage_clicked);
	QObject::connect(ui->pbtn_next, &QPushButton::clicked
		, this, &AutomaticAnnotation::pbtn_next_clicked);
}

void AutomaticAnnotation::iniThread()
{
	auto currentDeployType = ui->cBox_checkDeployType->currentText();
	auto currentModelType = ui->cBox_checkModelType->currentText();
	auto imageInput = ui->lineEdit_ImageInput->text();
	auto allOutPut = ui->lineEdit_AllOutput->text();
	auto labelOutput = labelOutPut;
	auto imageOutput = imageOutPut;
	auto modelPath = ui->lineEdit_modelPath->text();
	auto workers = ui->pbtn_setWorkers->text().toInt();
	auto confThreshold = ui->pbtn_setConfThreshold->text();
	auto nmsThreshold = ui->pbtn_nmsThreshold->text();
	rw::ModelEngineConfig config;
	config.modelPath = modelPath.toStdString();
	config.nms_threshold = std::stof(nmsThreshold.toStdString());
	config.conf_threshold = std::stof(confThreshold.toStdString());
	config.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;

	auto paths = getAllImagePaths(ui->lineEdit_ImageInput->text());
	size = paths.size();
	int averageNum = paths.size() / workers;
	int lastNum = paths.size() % workers;
	auto modelType = getModelType();
	auto deployType = getDeployType();

	QString labelListQString = ui->lEdit_labelList->text();
	// 使用正则表达式分割，匹配非数字字符
	QStringList strList = labelListQString.split(QRegularExpression("[^0-9]+"), Qt::SkipEmptyParts);

	QVector<int> labelList;
	for (const QString& str : strList) {
		labelList.append(str.toInt());
	}

	try
	{
		auto test = rw::ModelEngineFactory::createModelEngine(config, modelType, deployType);

		for (int i = 0; i < workers; i++) {
			AutomaticAnnotationThread* thread = nullptr;
			if (i == 0) {
				thread = new AutomaticAnnotationThread(paths.mid(i, averageNum + lastNum));
			}
			else {
				thread = new AutomaticAnnotationThread(paths.mid(i * averageNum, averageNum));
			}
			thread->labelType = ui->cBox_exportLabelType->currentText();
			thread->modelType = modelType;
			thread->config = config;
			thread->labelOutput = labelOutput;
			thread->imageOutput = imageOutput;
			thread->isAutoZip = ui->ckb_autoZip->isChecked();
			thread->rootOutPutPath = allOutPut;
			thread->deployType = deployType;
			thread->labelList = labelList;
			threads.push_back(thread);
			connect(thread, &AutomaticAnnotationThread::imageProcessed, this, &AutomaticAnnotation::displayImage, Qt::QueuedConnection);
		}

		ui->tabWidget->setCurrentIndex(1);
	}
	catch (...)
	{
		QMessageBox::warning(this, "Warning", QString("模型加载失败，请检查模型路径和类型"));
		return;
	}
}

void AutomaticAnnotation::pbtn_setImageInput_clicked()
{
	QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), "");
	if (!folderPath.isEmpty())
	{
		ui->lineEdit_ImageInput->setText(folderPath);
	}
	else
	{
		ui->lineEdit_ImageInput->clear();
	}
}

void AutomaticAnnotation::pbtn_setAllOutput_clicked()
{
	QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), "");
	if (!folderPath.isEmpty())
	{
		ui->lineEdit_AllOutput->setText(folderPath);
		imageOutPut = folderPath + R"(/images/train)";
		labelOutPut = folderPath + R"(/labels/train)";
	}
	else
	{
		ui->lineEdit_AllOutput->clear();
		imageOutPut.clear();
		labelOutPut.clear();
	}
}

void AutomaticAnnotation::pbtn_setModelPath_clicked()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Select File"), "", tr("Model Files (*.onnx *.pb *.xml *.engine)"));
	if (!filePath.isEmpty())
	{
		ui->lineEdit_modelPath->setText(filePath);
	}
	else
	{
		ui->lineEdit_modelPath->clear();
	}
}

void AutomaticAnnotation::pbtn_setWorkers_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		ui->pbtn_setWorkers->setText(value);
	}
}

void AutomaticAnnotation::pbtn_setConfThreshold_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		ui->pbtn_setConfThreshold->setText(value);
	}
}

void AutomaticAnnotation::pbtn_nmsThreshold_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		ui->pbtn_nmsThreshold->setText(value);
	}
}

void AutomaticAnnotation::pbtn_exit_clicked()
{
	this->close();
}

void AutomaticAnnotation::pbtn_LookImage_clicked()
{
	viewer->setRootPath(ui->lineEdit_ImageInput->text());
	viewer->show();
}

void AutomaticAnnotation::pbtn_next_clicked()
{
	if (ui->lineEdit_ImageInput->text().isEmpty())
	{
		QMessageBox::warning(this, "Warning", QString("请设置图像输入路径"));
		return;
	}
	if (ui->lineEdit_AllOutput->text().isEmpty())
	{
		QMessageBox::warning(this, "Warning", QString("请设置输出路径"));
		return;
	}
	if (ui->lineEdit_modelPath->text().isEmpty())
	{
		QMessageBox::warning(this, "Warning", QString("请设置模型路径"));
		return;
	}
	if (ui->pbtn_setWorkers->text().toInt() == 0)
	{
		QMessageBox::warning(this, "Warning", QString("工作线程数量不能为0"));
		return;
	}
	iniThread();
}

void AutomaticAnnotation::on_pbtn_preStep_clicked()
{
	for (int i = 0; i < threads.size(); i++)
	{
		disconnect(threads[i], &AutomaticAnnotationThread::imageProcessed, this, &AutomaticAnnotation::displayImage);
	}

	for (int i = 0; i < threads.size(); i++) {
		threads[i]->wait();
		delete threads[i];
	}
	threads.clear();
	ui->tabWidget->setCurrentIndex(0);
}

void AutomaticAnnotation::on_pbtn_startAnnotation_clicked()
{
	complete = 0;
	ui->progressBar->setValue(0);
	for (int i = 0; i < threads.size(); i++) {
		threads[i]->start();
	}
}

void AutomaticAnnotation::on_pbtn_tab2_exit_clicked()
{
	this->close();
}

void AutomaticAnnotation::displayImage(QString imagePath, QPixmap pixmap)
{
	complete++;
	ui->progressBar->setValue((complete * 100) / size);

	if (pixmap.isNull()) {
		qDebug() << "Failed to load image:" << imagePath;
		return;
	}
	ui->label_imgDisplay->setPixmap(pixmap.scaled(ui->label_imgDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

rw::ModelType AutomaticAnnotation::getModelType()
{
	auto currentModelType = ui->cBox_checkModelType->currentText();
	if (currentModelType == "Yolov11_seg")
	{
		return rw::ModelType::Yolov11_Seg_CudaAcc;
	}
	else if (currentModelType == "Yolov11_det")
	{
		return rw::ModelType::Yolov11_Det_CudaAcc;
	}
	else if (currentModelType == "Yolov11_obb")
	{
		return rw::ModelType::Yolov11_Obb;
	}
	else if (currentModelType == "Yolov11_segWithMask")
	{
		return rw::ModelType::Yolov11_Seg_Mask;
	}
	else
	{
		return rw::ModelType::Yolov11_Det_CudaAcc;
	}
}

rw::ModelEngineDeployType AutomaticAnnotation::getDeployType()
{
	auto currentDeployType = ui->cBox_checkDeployType->currentText();
	if (currentDeployType == "OnnxRuntime")
	{
		return rw::ModelEngineDeployType::OnnxRuntime;
	}
	else if (currentDeployType == "TensorRT")
	{
		return rw::ModelEngineDeployType::TensorRT;
	}
	else
	{
		return rw::ModelEngineDeployType::OnnxRuntime;
	}
}