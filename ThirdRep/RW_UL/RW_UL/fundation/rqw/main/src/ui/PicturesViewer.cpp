#include "PicturesViewer.h"

#include "ui_PicturesViewer.h"

#include<QMessageBox>

PicturesViewer::PicturesViewer(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::PicturesViewerClass())
{
	ui->setupUi(this);

	build_ui();
	build_connect();
}

PicturesViewer::~PicturesViewer()
{
	delete ui;
}

void PicturesViewer::build_ui()
{
	_categoryModel = new QStandardItemModel(this);
	ui->treeView_categoryTree->setModel(_categoryModel);

	_picturesListModel = new QStandardItemModel(this);
	ui->listView_picturesList->setModel(_picturesListModel);
}

void PicturesViewer::build_connect()
{
	QObject::connect(ui->pbtn_exit, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_exit_clicked);

	QObject::connect(ui->treeView_categoryTree->selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &PicturesViewer::onCategorySelectionChanged);

	QObject::connect(ui->listView_picturesList->selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &PicturesViewer::onPictureSelectionChanged);

	QObject::connect(ui->pbtn_nextPicture, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_nextPicture_clicked);

	QObject::connect(ui->pbtn_prePicture, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_prevPicture_clicked);

	QObject::connect(ui->pbtn_delete, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_delete_clicked);

	QObject::connect(ui->pbtn_deleteTotal, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_delete_total_clicked);

	QObject::connect(ui->pbtn_preCategory, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_preCategory_clicked);

	QObject::connect(ui->pbtn_nextCategory, &QPushButton::clicked,
		this, &PicturesViewer::pbtn_nextCategory_clicked);
}

void PicturesViewer::setRootPath(const QString& path)
{
	_rootPath = path;
}

void PicturesViewer::showEvent(QShowEvent* showEvent)
{
	QMainWindow::showEvent(showEvent);
	updateCategoryList();
}

void PicturesViewer::updateCategoryList()
{
	_categoryModel->clear();
	// 检查 _rootPath 是否有效
	if (_rootPath.isEmpty()) {
		qDebug() << "Root path is empty.";
		return;
	}

	QDir rootDir(_rootPath);
	if (!rootDir.exists()) {
		qDebug() << "Root path does not exist:" << _rootPath;
		return;
	}

	_categoryModel->setHorizontalHeaderLabels({ "文件树" });

	// 遍历 _rootPath 下的所有文件夹
	QStringList categoryFolders = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& category : categoryFolders) {
		QString categoryPath = rootDir.filePath(category);
		QStandardItem* categoryItem = new QStandardItem(category);

		// 保存绝对路径到节点
		categoryItem->setData(categoryPath, Qt::UserRole);

		// 递归添加子文件夹
		QDir categoryDir(categoryPath);
		addSubFolders(categoryDir, categoryItem);

		_categoryModel->appendRow(categoryItem);
	}

	// 设置模型到 treeView_categoryTree

	ui->treeView_categoryTree->expandAll();

	// 设置当前选中的索引为第一个最深的子节点
	QModelIndex firstDeepestIndex = findFirstDeepestIndex(_categoryModel);
	if (firstDeepestIndex.isValid()) {
		ui->treeView_categoryTree->setCurrentIndex(firstDeepestIndex);
	}
}

QModelIndex PicturesViewer::findFirstDeepestIndex(QStandardItemModel* model)
{
	if (!model || model->rowCount() == 0) {
		return QModelIndex();
	}

	QStandardItem* rootItem = model->invisibleRootItem();
	return findDeepestChild(rootItem);
}

QModelIndex PicturesViewer::findDeepestChild(QStandardItem* parentItem)
{
	if (!parentItem || parentItem->rowCount() == 0) {
		return parentItem ? parentItem->index() : QModelIndex();
	}

	// 遍历子节点，找到第一个最深的子节点
	QStandardItem* firstChild = parentItem->child(0);
	return findDeepestChild(firstChild);
}

void PicturesViewer::addSubFolders(const QDir& parentDir, QStandardItem* parentItem)
{
	// 获取当前目录下的所有子文件夹
	QStringList subFolders = parentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& subFolder : subFolders) {
		QString subFolderPath = parentDir.filePath(subFolder);
		QStandardItem* subFolderItem = new QStandardItem(subFolder);

		// 保存绝对路径到节点
		subFolderItem->setData(subFolderPath, Qt::UserRole);

		// 递归处理子文件夹
		QDir subFolderDir(subFolderPath);
		addSubFolders(subFolderDir, subFolderItem);

		parentItem->appendRow(subFolderItem);
	}
}

void PicturesViewer::pbtn_exit_clicked()
{
	this->hide();
	emit viewerClosed();
}

void PicturesViewer::onCategorySelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (selected.indexes().isEmpty()) {
		qDebug() << "No selection.";
		return;
	}

	QModelIndex currentIndex = selected.indexes().first();

	QString selectedPath = currentIndex.data(Qt::UserRole).toString();

	QDir dir(selectedPath);
	if (!dir.exists()) {
		qDebug() << "Directory does not exist:" << selectedPath;
		return;
	}

	QStringList filters;
	filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif";
	QStringList imageFiles = dir.entryList(filters, QDir::Files | QDir::NoSymLinks);

	updatePicturesList(selectedPath, imageFiles);
}

void PicturesViewer::updatePicturesList(const QString& directoryPath, const QStringList& imageFiles)
{
	_picturesListModel->clear();

	QList<QStandardItem*> items;
	for (const QString& fileName : imageFiles) {
		QString absolutePath = QDir(directoryPath).filePath(fileName);

		QStandardItem* item = new QStandardItem(fileName);

		item->setData(absolutePath, Qt::UserRole);

		items.append(item);
	}

	for (QStandardItem* item : items) {
		_picturesListModel->appendRow(item);
	}

	if (!imageFiles.isEmpty()) {
		QModelIndex firstIndex = _picturesListModel->index(0, 0);
		ui->listView_picturesList->setCurrentIndex(firstIndex);
	}
	else
	{
		ui->label_imgDisplay->clear();
	}
}

void PicturesViewer::onPictureSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	// 检查是否有选中的项
	if (selected.indexes().isEmpty()) {
		qDebug() << "No picture selected.";
		return;
	}

	// 获取当前选中的索引
	QModelIndex currentIndex = selected.indexes().first();

	// 获取存储的绝对路径
	QString selectedPicturePath = currentIndex.data(Qt::UserRole).toString();

	QPixmap pixmap(selectedPicturePath);
	if (pixmap.isNull()) {
		qDebug() << "Failed to load image:" << selectedPicturePath;
		return;
	}
	ui->statusBar->showMessage("当前图片路径: " + selectedPicturePath);
	ui->label_imgDisplay->setPixmap(pixmap.scaled(ui->label_imgDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PicturesViewer::pbtn_nextPicture_clicked()
{
	QModelIndex currentIndex = ui->listView_picturesList->currentIndex();

	QStandardItem* currentItem = _picturesListModel->itemFromIndex(currentIndex);
	if (!currentItem) {
		return;
	}

	int rowCount = _picturesListModel->rowCount();

	int nextRow = (currentIndex.row() + 1) % rowCount;

	QStandardItem* nextItem = _picturesListModel->item(nextRow, 0);
	if (nextItem) {
		ui->listView_picturesList->setCurrentIndex(nextItem->index());
	}
}

void PicturesViewer::pbtn_prevPicture_clicked()
{
	QModelIndex currentIndex = ui->listView_picturesList->currentIndex();

	QStandardItem* currentItem = _picturesListModel->itemFromIndex(currentIndex);
	if (!currentItem) {
		return;
	}

	int prevRow = (currentIndex.row() - 1 + _picturesListModel->rowCount()) % _picturesListModel->rowCount();

	QStandardItem* prevItem = _picturesListModel->item(prevRow, 0);
	if (prevItem) {
		ui->listView_picturesList->setCurrentIndex(prevItem->index());
	}
}

void PicturesViewer::pbtn_preCategory_clicked()
{
	QModelIndex currentIndex = ui->treeView_categoryTree->currentIndex();
	QList<QModelIndex> allIndexes = getAllIndexes(_categoryModel);

	if (allIndexes.isEmpty()) {
		return;
	}

	int currentIndexPosition = allIndexes.indexOf(currentIndex);
	int prevIndexPosition = (currentIndexPosition - 1 + allIndexes.size()) % allIndexes.size();

	QModelIndex prevIndex = allIndexes.at(prevIndexPosition);
	ui->treeView_categoryTree->setCurrentIndex(prevIndex);
}

void PicturesViewer::pbtn_nextCategory_clicked()
{
	QModelIndex currentIndex = ui->treeView_categoryTree->currentIndex();
	QList<QModelIndex> allIndexes = getAllIndexes(_categoryModel);

	if (allIndexes.isEmpty()) {
		return;
	}

	int currentIndexPosition = allIndexes.indexOf(currentIndex);
	int nextIndexPosition = (currentIndexPosition + 1) % allIndexes.size();

	QModelIndex nextIndex = allIndexes.at(nextIndexPosition);
	ui->treeView_categoryTree->setCurrentIndex(nextIndex);
}

QList<QModelIndex> PicturesViewer::getAllIndexes(QStandardItemModel* model)
{
	QList<QModelIndex> indexes;
	QStandardItem* rootItem = model->invisibleRootItem();
	collectIndexes(rootItem, indexes);
	return indexes;
}

void PicturesViewer::collectIndexes(QStandardItem* item, QList<QModelIndex>& indexes)
{
	if (!item) {
		return;
	}

	for (int i = 0; i < item->rowCount(); ++i) {
		QStandardItem* child = item->child(i);
		if (child) {
			indexes.append(child->index());
			collectIndexes(child, indexes);
		}
	}
}

void PicturesViewer::pbtn_delete_clicked()
{
	// 获取当前选中的索引
	QModelIndex currentIndex = ui->listView_picturesList->currentIndex();

	// 检查索引是否有效
	if (!currentIndex.isValid()) {
		qDebug() << "No picture selected for deletion.";
		return;
	}

	// 获取当前选中图片的绝对路径
	QString picturePath = currentIndex.data(Qt::UserRole).toString();

	// 删除文件
	QFile file(picturePath);
	if (file.exists()) {
		if (file.remove()) {
			qDebug() << "Deleted picture:" << picturePath;
		}
		else {
			qDebug() << "Failed to delete picture:" << picturePath;
			return;
		}
	}
	else {
		qDebug() << "File does not exist:" << picturePath;
		return;
	}

	// 从模型中移除当前项
	_picturesListModel->removeRow(currentIndex.row());

	// 获取新的索引
	int rowCount = _picturesListModel->rowCount();
	if (rowCount > 0) {
		// 如果还有图片，设置索引为下一张图片
		int nextRow = currentIndex.row() % rowCount;
		QModelIndex nextIndex = _picturesListModel->index(nextRow, 0);
		ui->listView_picturesList->setCurrentIndex(nextIndex);
	}
	else {
		// 如果没有图片了，清空图片显示并设置索引为无效
		ui->label_imgDisplay->clear();
		ui->listView_picturesList->clearSelection();
	}
}

void PicturesViewer::pbtn_delete_total_clicked()
{
	auto result = QMessageBox::question(this, "提示", "是否删除当前目录下所有图片？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {
		return;
	}
	// 获取当前选中的索引
	QModelIndex currentIndex = ui->treeView_categoryTree->currentIndex();
	// 检查索引是否有效
	if (!currentIndex.isValid()) {
		qDebug() << "No category selected for deletion.";
		return;
	}

	// 获取当前选中目录的绝对路径
	QString categoryPath = currentIndex.data(Qt::UserRole).toString();
	QDir dir(categoryPath);

	// 检查目录是否存在
	if (!dir.exists()) {
		qDebug() << "Directory does not exist:" << categoryPath;
		return;
	}

	// 删除目录中的所有文件
	QStringList fileFilters;
	fileFilters << "*"; // 匹配所有文件
	QStringList files = dir.entryList(fileFilters, QDir::Files | QDir::NoSymLinks);
	for (const QString& file : files) {
		QString filePath = dir.filePath(file);
		if (!QFile::remove(filePath)) {
			qDebug() << "Failed to delete file:" << filePath;
		}
		else {
			qDebug() << "Deleted file:" << filePath;
		}
	}

	// 删除目录中的所有子文件夹及其内容
	QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& subDir : subDirs) {
		QDir subDirPath(dir.filePath(subDir));
		if (!subDirPath.removeRecursively()) {
			qDebug() << "Failed to delete subdirectory:" << subDirPath.path();
		}
		else {
			qDebug() << "Deleted subdirectory:" << subDirPath.path();
		}
	}

	qDebug() << "Cleared all contents of directory:" << categoryPath;

	// 更新目录列表
	updateCategoryList();
}