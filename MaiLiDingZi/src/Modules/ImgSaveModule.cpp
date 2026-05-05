#include "ImgSaveModule.hpp"

#include <QDir>

#include "Utilty.hpp"

void ImgSaveModule::build()
{
	{
		QString imageSavePath = globalPath.imageSaveRootPath;
		QVector<QString> sortedFolders;

		// 打开指定路径
		QDir dir(imageSavePath);
		if (!dir.exists()) {
			return;
		}

		// 设置过滤器，只获取文件夹
		dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

		// 获取文件夹列表
		QFileInfoList folderList = dir.entryInfoList();

		// 存储文件夹及其日期
		QVector<QPair<QDate, QString>> dateFolderPairs;

		for (const QFileInfo& folderInfo : folderList) {
			QString folderName = folderInfo.fileName();

			// 尝试解析文件夹名称为日期
			QDate folderDate = QDate::fromString(folderName, "yyyy_MM_dd");
			if (folderDate.isValid()) {
				dateFolderPairs.append(qMakePair(folderDate, folderName));
			}
		}

		// 按日期排序
		std::sort(dateFolderPairs.begin(), dateFolderPairs.end(),
			[](const QPair<QDate, QString>& a, const QPair<QDate, QString>& b) {
				return a.first < b.first;
			});

		// 提取排序后的文件夹名称
		for (const auto& pair : dateFolderPairs) {
			sortedFolders.append(pair.second);
		}

		// 删除超过7天的文件夹
		QDate currentDate = QDate::currentDate();
		for (const QString& folderName : sortedFolders) {
			QDate folderDate = QDate::fromString(folderName, "yyyy_MM_dd");
			if (folderDate.isValid() && folderDate < currentDate.addDays(-7)) {
				QString folderPath = imageSavePath + folderName;
				QDir(folderPath).removeRecursively();
			}
		}
	}

	imageSaveEngine = std::make_unique<rw::rqw::ImageSaveEngine>(this, cimgSaveThreadNum);

	imageSaveEngine->setSaveImgFormat(rw::rqw::ImageSaveFormat::JPEG);
	imageSaveEngine->setSaveImgQuality(cimgSaveQuality);
	QDir dir;
	QString imageSavePath = globalPath.imageSaveRootPath;

	QString currentDate = QDate::currentDate().toString("yyyy_MM_dd");
	QString imageSaveEnginePath = imageSavePath + currentDate;

	QString imagesFilePathFilePathFull = dir.absoluteFilePath(imageSaveEnginePath);
	imageSaveEngine->setRootPath(imagesFilePathFilePathFull);
	imageSaveEngine->setSavePolicy(rw::rqw::ImageSaveEnginePolicy::Normal);
}

void ImgSaveModule::destroy()
{
	if (imageSaveEngine)
	{
		imageSaveEngine.reset();
	}
}

void ImgSaveModule::start()
{
	if (imageSaveEngine)
	{
		imageSaveEngine->startEngine();
	}
}

void ImgSaveModule::stop()
{
	if (imageSaveEngine)
	{
		imageSaveEngine->stop();
	}
}
