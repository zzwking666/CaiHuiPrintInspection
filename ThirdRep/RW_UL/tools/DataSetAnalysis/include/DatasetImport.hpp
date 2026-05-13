#pragma once

#include <QString>

#include"IndexType.hpp"

struct DatasetImport
{
public:
	static bool getImportIndex(
		const QString& dataSetPath,
		ClassIdToPath & classIdToPath, 
		DataSetPathToClassIds & dataSetPathToClassIds,
		UnknownImgPathSet & unknownImgPathSet,
		const DataSetType & dataSetType
	);
public:
	//YoloDetection to index
	static bool getImportIndexOfYoloDetection(
		const QString& dataSetPath,
		ClassIdToPath& classIdToPath,
		DataSetPathToClassIds& dataSetPathToClassIds,
		UnknownImgPathSet& unknownImgPathSet
	);

	//YOlOSegmentation to index
	static bool getImportIndexOfYOlOSegmentation(
		const QString& dataSetPath,
		ClassIdToPath& classIdToPath,
		DataSetPathToClassIds& dataSetPathToClassIds,
		UnknownImgPathSet& unknownImgPathSet
	);

	//YOLOOrientedBounding to index
	static bool getImportIndexOfYOLOOrientedBounding(
		const QString& dataSetPath,
		ClassIdToPath& classIdToPath,
		DataSetPathToClassIds& dataSetPathToClassIds,
		UnknownImgPathSet& unknownImgPathSet
	);
};
