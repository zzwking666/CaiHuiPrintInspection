#pragma once

#include <QMainWindow>
#include<QStandardItemModel>
#include<QItemSelection>
#include<QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class PicturesViewerClass; };
QT_END_NAMESPACE

class PicturesViewer : public QMainWindow
{
	Q_OBJECT
private:
	QString _rootPath;
private:
	QStandardItemModel* _categoryModel;
	QStandardItemModel* _picturesListModel;

public:
	PicturesViewer(QWidget* parent = nullptr);
	~PicturesViewer();

private:
	void build_ui();
	void build_connect();

public:
	void setRootPath(const QString& path);
protected:
	void showEvent(QShowEvent*) override;
private:
	void updateCategoryList();
	void addSubFolders(const QDir& parentDir, QStandardItem* parentItem);
	QModelIndex findFirstDeepestIndex(QStandardItemModel* model);
	QModelIndex findDeepestChild(QStandardItem* parentItem);
	void updatePicturesList(const QString& directoryPath, const QStringList& imageFiles);
	QList<QModelIndex> getAllIndexes(QStandardItemModel* model);
	void collectIndexes(QStandardItem* item, QList<QModelIndex>& indexes);
private:
	Ui::PicturesViewerClass* ui;
signals:
	void viewerClosed();

private slots:
	void pbtn_exit_clicked();
	void onCategorySelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void onPictureSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void pbtn_nextPicture_clicked();
	void pbtn_prevPicture_clicked();
	void pbtn_preCategory_clicked();
	void pbtn_nextCategory_clicked();
	void pbtn_delete_clicked();
	void pbtn_delete_total_clicked();
};
