#include"DatasetImport.hpp"

#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QDirIterator>

bool DatasetImport::getImportIndex(const QString& dataSetPath, ClassIdToPath& classIdToPath,
	DataSetPathToClassIds& dataSetPathToClassIds, UnknownImgPathSet& unknownImgPathSet, const DataSetType& dataSetType)
{
	switch (dataSetType)
	{
	case DataSetType::YolODetection:
		return getImportIndexOfYoloDetection(dataSetPath, classIdToPath, dataSetPathToClassIds, unknownImgPathSet);
	case DataSetType::YOlOSegmentation:
		return getImportIndexOfYOlOSegmentation(dataSetPath, classIdToPath, dataSetPathToClassIds, unknownImgPathSet);
	case DataSetType::YOLOOrientedBounding:
		return getImportIndexOfYOLOOrientedBounding(dataSetPath, classIdToPath, dataSetPathToClassIds, unknownImgPathSet);
	default:
		return false;
	}
}

bool DatasetImport::getImportIndexOfYoloDetection(const QString& dataSetPath, ClassIdToPath& classIdToPath,
	DataSetPathToClassIds& dataSetPathToClassIds, UnknownImgPathSet& unknownImgPathSet)
{
	QDir dataSetDir(dataSetPath);
	if (!dataSetDir.exists()) {
		return false;
	}

	QDir imagesDir(dataSetDir.absoluteFilePath("images"));
	QDir labelsDir(dataSetDir.absoluteFilePath("labels"));

	if (!imagesDir.exists() || !labelsDir.exists()) {
		return false;
	}

	// 支持的图片格式
	QStringList imageFilters = { "*.jpg", "*.jpeg", "*.png", "*.bmp" };

	// 递归遍历images目录下的所有图片文件
	QDirIterator it(imagesDir.absolutePath(), imageFilters, QDir::Files, QDirIterator::Subdirectories);

	while (it.hasNext()) {
		QString imgPath = it.next();
		QFileInfo imageFile(imgPath);

		// 获取相对于images目录的相对路径
		QString relativePath = imagesDir.relativeFilePath(imgPath);
		QString baseName = imageFile.completeBaseName();
		QString labelRelativePath = QFileInfo(relativePath).path() + "/" + baseName + ".txt";
		QString labelPath = labelsDir.absoluteFilePath(labelRelativePath);

		QFileInfo labelFile(labelPath);

		if (!labelFile.exists()) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		QFile file(labelPath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		QTextStream in(&file);
		std::unordered_set<ClassId> classIdsInFile;
		bool hasValidAnnotation = false;

		while (!in.atEnd()) {
			QString line = in.readLine().trimmed();
			if (line.isEmpty()) {
				continue;
			}

			QStringList parts = line.split(' ', Qt::SkipEmptyParts);
			if (parts.isEmpty()) {
				continue;
			}

			bool ok = false;
			ClassId classId = parts[0].toInt(&ok);
			if (!ok) {
				continue;
			}

			classIdsInFile.insert(classId);
			hasValidAnnotation = true;

			DataSetPathItem pathItem = { imgPath.toStdString(), labelPath.toStdString() };
			classIdToPath[classId].insert(pathItem);
		}

		file.close();

		if (!hasValidAnnotation) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		DataSetPathItem pathItem = { imgPath.toStdString(), labelPath.toStdString() };
		dataSetPathToClassIds[pathItem] = classIdsInFile;
	}

	return true;
}

bool DatasetImport::getImportIndexOfYOlOSegmentation(const QString& dataSetPath, ClassIdToPath& classIdToPath,
	DataSetPathToClassIds& dataSetPathToClassIds, UnknownImgPathSet& unknownImgPathSet)
{
	QDir dataSetDir(dataSetPath);
	if (!dataSetDir.exists()) {
		return false;
	}

	QDir imagesDir(dataSetDir.absoluteFilePath("images"));
	QDir labelsDir(dataSetDir.absoluteFilePath("labels"));

	if (!imagesDir.exists() || !labelsDir.exists()) {
		return false;
	}

	QStringList imageFilters = { "*.jpg", "*.jpeg", "*.png", "*.bmp" };

	QDirIterator it(imagesDir.absolutePath(), imageFilters, QDir::Files, QDirIterator::Subdirectories);

	while (it.hasNext()) {
		QString imgPath = it.next();
		QFileInfo imageFile(imgPath);

		QString relativePath = imagesDir.relativeFilePath(imgPath);
		QString baseName = imageFile.completeBaseName();
		QString labelRelativePath = QFileInfo(relativePath).path() + "/" + baseName + ".txt";
		QString labelPath = labelsDir.absoluteFilePath(labelRelativePath);

		QFileInfo labelFile(labelPath);

		if (!labelFile.exists()) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		QFile file(labelPath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		QTextStream in(&file);
		std::unordered_set<ClassId> classIdsInFile;
		bool hasValidAnnotation = false;

		while (!in.atEnd()) {
			QString line = in.readLine().trimmed();
			if (line.isEmpty()) {
				continue;
			}

			QStringList parts = line.split(' ', Qt::SkipEmptyParts);
			if (parts.isEmpty()) {
				continue;
			}

			bool ok = false;
			ClassId classId = parts[0].toInt(&ok);
			if (!ok) {
				continue;
			}

			if (parts.size() < 7) {
				continue;
			}

			classIdsInFile.insert(classId);
			hasValidAnnotation = true;

			DataSetPathItem pathItem = { imgPath.toStdString(), labelPath.toStdString() };
			classIdToPath[classId].insert(pathItem);
		}

		file.close();

		if (!hasValidAnnotation) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		DataSetPathItem pathItem = { imgPath.toStdString(), labelPath.toStdString() };
		dataSetPathToClassIds[pathItem] = classIdsInFile;
	}

	return true;
}

bool DatasetImport::getImportIndexOfYOLOOrientedBounding(const QString& dataSetPath, ClassIdToPath& classIdToPath,
	DataSetPathToClassIds& dataSetPathToClassIds, UnknownImgPathSet& unknownImgPathSet)
{
	QDir dataSetDir(dataSetPath);
	if (!dataSetDir.exists()) {
		return false;
	}

	QDir imagesDir(dataSetDir.absoluteFilePath("images"));
	QDir labelsDir(dataSetDir.absoluteFilePath("labels"));

	if (!imagesDir.exists() || !labelsDir.exists()) {
		return false;
	}

	QStringList imageFilters = { "*.jpg", "*.jpeg", "*.png", "*.bmp" };

	QDirIterator it(imagesDir.absolutePath(), imageFilters, QDir::Files, QDirIterator::Subdirectories);

	while (it.hasNext()) {
		QString imgPath = it.next();
		QFileInfo imageFile(imgPath);

		QString relativePath = imagesDir.relativeFilePath(imgPath);
		QString baseName = imageFile.completeBaseName();
		QString labelRelativePath = QFileInfo(relativePath).path() + "/" + baseName + ".txt";
		QString labelPath = labelsDir.absoluteFilePath(labelRelativePath);

		QFileInfo labelFile(labelPath);

		if (!labelFile.exists()) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		QFile file(labelPath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		QTextStream in(&file);
		std::unordered_set<ClassId> classIdsInFile;
		bool hasValidAnnotation = false;

		while (!in.atEnd()) {
			QString line = in.readLine().trimmed();
			if (line.isEmpty()) {
				continue;
			}

			QStringList parts = line.split(' ', Qt::SkipEmptyParts);
			if (parts.isEmpty()) {
				continue;
			}

			bool ok = false;
			ClassId classId = parts[0].toInt(&ok);
			if (!ok) {
				continue;
			}

			if (parts.size() < 9) {
				continue;
			}

			classIdsInFile.insert(classId);
			hasValidAnnotation = true;

			DataSetPathItem pathItem = { imgPath.toStdString(), labelPath.toStdString() };
			classIdToPath[classId].insert(pathItem);
		}

		file.close();

		if (!hasValidAnnotation) {
			unknownImgPathSet.insert(imgPath.toStdString());
			continue;
		}

		DataSetPathItem pathItem = { imgPath.toStdString(), labelPath.toStdString() };
		dataSetPathToClassIds[pathItem] = classIdsInFile;
	}

	return true;
}