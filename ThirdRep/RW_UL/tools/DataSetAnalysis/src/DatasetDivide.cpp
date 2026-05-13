#include"DatasetDivide.hpp"

#include <QDir>
#include <QFile>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <QFileInfo>

void DatasetDivide::divideByClassid(const ClassIdToPath& classIdToPath, const QString& outputDir)
{
	for (const auto& classPair : classIdToPath)
	{
		const ClassId classId = classPair.first;
		const auto& itemSet = classPair.second;

		QString classDir = QDir(outputDir).filePath(QString::number(classId));
		QString imagesDir = QDir(classDir).filePath("images");
		QString labelsDir = QDir(classDir).filePath("labels");
		QDir().mkpath(imagesDir);
		QDir().mkpath(labelsDir);

		for (const auto& item : itemSet)
		{
			const QString imgSrc = QString::fromStdString(item.first);
			const QString labelSrc = QString::fromStdString(item.second);

			// 拷贝图片
			if (!imgSrc.isEmpty())
			{
				QFileInfo imgInfo(imgSrc);
				QString imgDst = QDir(imagesDir).filePath(imgInfo.fileName());
				QFile::copy(imgSrc, imgDst);
			}

			// 拷贝标签
			if (!labelSrc.isEmpty())
			{
				QFileInfo labelInfo(labelSrc);
				QString labelDst = QDir(labelsDir).filePath(labelInfo.fileName());
				QFile::copy(labelSrc, labelDst);
			}
		}
	}
}

void DatasetDivide::divideByQuantity(const ClassIdToPath& classIdToPath,
	const DataSetPathToClassIds& dataSetPathToClassIds, const DivideByQuantityCfg& cfg, const QString& outputDir)
{
	// 标记已处理的样本
	std::unordered_set<DataSetPathItem, DataSetPathItemHash, DataSetPathItemEqual> processed;

	// 统计每个类别的样本数，并过滤ignoreClassIds
	std::vector<std::pair<ClassId, size_t>> classCountVec;
	for (const auto& classPair : classIdToPath)
	{
		ClassId classId = classPair.first;
		if (std::find(cfg.ignoreClassIds.begin(), cfg.ignoreClassIds.end(), classId) != cfg.ignoreClassIds.end())
			continue;
		classCountVec.emplace_back(classId, classPair.second.size());
	}

	// 排序，isSmallPriority==false时，优先处理样本数少的类别
	if (!cfg.isSmallPriority)
	{
		std::sort(classCountVec.begin(), classCountVec.end(),
			[](const auto& a, const auto& b) { return a.second < b.second; });
	}
	else
	{
		std::sort(classCountVec.begin(), classCountVec.end(),
			[](const auto& a, const auto& b) { return a.second > b.second; });
	}

	// 创建目标目录
	QString imagesDir = QDir(outputDir).filePath("images");
	QString labelsDir = QDir(outputDir).filePath("labels");
	QDir().mkpath(imagesDir);
	QDir().mkpath(labelsDir);

	// 依次处理每个类别
	for (const auto& classCount : classCountVec)
	{
		ClassId classId = classCount.first;
		const auto& itemSet = classIdToPath.at(classId);

		// 收集未处理的样本
		std::vector<DataSetPathItem> items;
		for (const auto& item : itemSet)
		{
			if (processed.find(item) == processed.end())
				items.push_back(item);
		}

		// 按比例划分
		size_t total = items.size();
		size_t trainCount = static_cast<size_t>(total * cfg.proportionCfg.trainProportion);
		size_t valCount = static_cast<size_t>(total * cfg.proportionCfg.valProportion);
		size_t testCount = total - trainCount - valCount;

		auto trainBegin = items.begin();
		auto valBegin = trainBegin + trainCount;
		auto testBegin = valBegin + valCount;

		struct SubsetInfo
		{
			QString name;
			std::vector<DataSetPathItem>::iterator begin;
			std::vector<DataSetPathItem>::iterator end;
		};

		std::vector<SubsetInfo> subsets = {
			{ "train", trainBegin, valBegin },
			{ "val", valBegin, testBegin },
			{ "test", testBegin, items.end() }
		};

		for (const auto& subset : subsets)
		{
			QString subsetImagesDir = QDir(imagesDir).filePath(subset.name);
			QString subsetLabelsDir = QDir(labelsDir).filePath(subset.name);
			QDir().mkpath(subsetImagesDir);
			QDir().mkpath(subsetLabelsDir);

			for (auto it = subset.begin; it != subset.end; ++it)
			{
				// 保证只处理一次
				if (processed.find(*it) != processed.end())
					continue;
				processed.insert(*it);

				const QString imgSrc = QString::fromStdString(it->first);
				const QString labelSrc = QString::fromStdString(it->second);

				if (!imgSrc.isEmpty())
				{
					QFileInfo imgInfo(imgSrc);
					QString imgDst = QDir(subsetImagesDir).filePath(imgInfo.fileName());
					QFile::copy(imgSrc, imgDst);
				}

				if (!labelSrc.isEmpty())
				{
					QFileInfo labelInfo(labelSrc);
					QString labelDst = QDir(subsetLabelsDir).filePath(labelInfo.fileName());
					QFile::copy(labelSrc, labelDst);
				}
			}
		}
	}
}

void DatasetDivide::divideByRandom(const ClassIdToPath& classIdToPath,
	const DataSetPathToClassIds& dataSetPathToClassIds, const DivideByRandomCfg& cfg, const QString& outputDir)
{
	std::vector<DataSetPathItem> allItems;
	allItems.reserve(dataSetPathToClassIds.size());
	for (const auto& pair : dataSetPathToClassIds)
	{
		allItems.push_back(pair.first);
	}

	unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
	std::shuffle(allItems.begin(), allItems.end(), std::default_random_engine(seed));

	size_t total = allItems.size();
	size_t trainCount = static_cast<size_t>(total * cfg.proportionCfg.trainProportion);
	size_t valCount = static_cast<size_t>(total * cfg.proportionCfg.valProportion);
	size_t testCount = total - trainCount - valCount;

	auto trainBegin = allItems.begin();
	auto valBegin = trainBegin + trainCount;
	auto testBegin = valBegin + valCount;

	struct SubsetInfo
	{
		QString name;
		std::vector<DataSetPathItem>::iterator begin;
		std::vector<DataSetPathItem>::iterator end;
	};

	std::vector<SubsetInfo> subsets = {
		{ "train", trainBegin, valBegin },
		{ "val", valBegin, testBegin },
		{ "test", testBegin, allItems.end() }
	};

	QString imagesDir = QDir(outputDir).filePath("images");
	QString labelsDir = QDir(outputDir).filePath("labels");
	QDir().mkpath(imagesDir);
	QDir().mkpath(labelsDir);

	for (const auto& subset : subsets)
	{
		QString subsetImagesDir = QDir(imagesDir).filePath(subset.name);
		QString subsetLabelsDir = QDir(labelsDir).filePath(subset.name);
		QDir().mkpath(subsetImagesDir);
		QDir().mkpath(subsetLabelsDir);

		for (auto it = subset.begin; it != subset.end; ++it)
		{
			const QString imgSrc = QString::fromStdString(it->first);
			const QString labelSrc = QString::fromStdString(it->second);

			if (!imgSrc.isEmpty())
			{
				QFileInfo imgInfo(imgSrc);
				QString imgDst = QDir(subsetImagesDir).filePath(imgInfo.fileName());
				QFile::copy(imgSrc, imgDst);
			}

			if (!labelSrc.isEmpty())
			{
				QFileInfo labelInfo(labelSrc);
				QString labelDst = QDir(subsetLabelsDir).filePath(labelInfo.fileName());
				QFile::copy(labelSrc, labelDst);
			}
		}
	}
}

void DatasetDivide::divideByProportion(const ClassIdToPath& classIdToPath,
	const DataSetPathToClassIds& dataSetPathToClassIds, const DivideByProportionCfg& cfg, const QString& outputDir)
{
	// 按顺序收集所有 DataSetPathItem
	std::vector<DataSetPathItem> allItems;
	allItems.reserve(dataSetPathToClassIds.size());
	for (const auto& pair : dataSetPathToClassIds)
	{
		allItems.push_back(pair.first);
	}

	// 按顺序划分
	size_t total = allItems.size();
	size_t trainCount = static_cast<size_t>(total * cfg.trainProportion);
	size_t valCount = static_cast<size_t>(total * cfg.valProportion);
	size_t testCount = total - trainCount - valCount;

	auto trainBegin = allItems.begin();
	auto valBegin = trainBegin + trainCount;
	auto testBegin = valBegin + valCount;

	struct SubsetInfo
	{
		QString name;
		std::vector<DataSetPathItem>::iterator begin;
		std::vector<DataSetPathItem>::iterator end;
	};

	std::vector<SubsetInfo> subsets = {
		{ "train", trainBegin, valBegin },
		{ "val", valBegin, testBegin },
		{ "test", testBegin, allItems.end() }
	};

	QString imagesDir = QDir(outputDir).filePath("images");
	QString labelsDir = QDir(outputDir).filePath("labels");
	QDir().mkpath(imagesDir);
	QDir().mkpath(labelsDir);

	for (const auto& subset : subsets)
	{
		QString subsetImagesDir = QDir(imagesDir).filePath(subset.name);
		QString subsetLabelsDir = QDir(labelsDir).filePath(subset.name);
		QDir().mkpath(subsetImagesDir);
		QDir().mkpath(subsetLabelsDir);

		for (auto it = subset.begin; it != subset.end; ++it)
		{
			const QString imgSrc = QString::fromStdString(it->first);
			const QString labelSrc = QString::fromStdString(it->second);

			if (!imgSrc.isEmpty())
			{
				QFileInfo imgInfo(imgSrc);
				QString imgDst = QDir(subsetImagesDir).filePath(imgInfo.fileName());
				QFile::copy(imgSrc, imgDst);
			}

			if (!labelSrc.isEmpty())
			{
				QFileInfo labelInfo(labelSrc);
				QString labelDst = QDir(subsetLabelsDir).filePath(labelInfo.fileName());
				QFile::copy(labelSrc, labelDst);
			}
		}
	}
}
