#pragma once

#include"IndexType.hpp"

#include<QString>

struct DatasetDivide
{
	static void divideByClassid(
		const ClassIdToPath & classIdToPath,
		const QString & outputDir
	);
	
	static void divideByQuantity(
		const ClassIdToPath& classIdToPath, 
		const DataSetPathToClassIds& dataSetPathToClassIds,
		const DivideByQuantityCfg & cfg,
		const QString& outputDir
	);

	static void divideByRandom(
		const ClassIdToPath& classIdToPath,
		const DataSetPathToClassIds & dataSetPathToClassIds,
		const DivideByRandomCfg& cfg, 
		const QString& outputDir
	);

	static void divideByProportion(
		const ClassIdToPath& classIdToPath, 
		const DataSetPathToClassIds& dataSetPathToClassIds,
		const DivideByProportionCfg& cfg, 
		const QString& outputDir
	);

};
