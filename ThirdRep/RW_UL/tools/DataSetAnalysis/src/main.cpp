#include "DataSetAnalysis.h"
#include <QtWidgets/QApplication>

#include "DatasetDivide.hpp"
#include"DatasetImport.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	DataSetAnalysis window;
	window.show();

	/*ClassIdToPath classIdToPath;
	UnknownImgPathSet unknownImgPathSet;

	DataSetPathToClassIds dataSetPathToClassIds;

	DatasetImport::getImportIndexOfYoloDetection(
		R"(C:\Users\rw\Desktop\temp\1)",
		classIdToPath,
		dataSetPathToClassIds,
		unknownImgPathSet
	);

	DivideByQuantityCfg cfg;
	cfg.proportionCfg.trainProportion = 0.7;
	cfg.proportionCfg.testProportion = 0.15;
	cfg.proportionCfg.valProportion = 0.15;

	DatasetDivide::divideByQuantity(classIdToPath, dataSetPathToClassIds, cfg,R"(C:\Users\rw\Desktop\temp2\quantity)");*/

	return app.exec();
}
