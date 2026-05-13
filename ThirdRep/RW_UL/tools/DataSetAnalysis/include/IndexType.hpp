#pragma once

#include<string>
#include<vector>
#include<unordered_map>
#include<unordered_set>

using Path = std::string;
using LabelPath = Path;
using ImgPath = Path;
using ClassId = int;
using DataSetPathItem = std::pair<ImgPath, LabelPath>;

struct DataSetPathItemHash {
	std::size_t operator()(const DataSetPathItem& item) const {
		std::string combined = item.first + "|" + item.second;
		return std::hash<std::string>{}(combined);
	}
};

struct DataSetPathItemEqual {
	bool operator()(const DataSetPathItem& lhs, const DataSetPathItem& rhs) const {
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}
};

using ClassIdToPath = std::unordered_map<ClassId, std::unordered_set<DataSetPathItem, DataSetPathItemHash, DataSetPathItemEqual>>;

using UnknownImgPathSet = std::unordered_set<ImgPath>;

using DataSetPathToClassIds = std::unordered_map<DataSetPathItem, std::unordered_set<ClassId>, DataSetPathItemHash>;

//等比划分配置
struct DivideByProportionCfg
{
	//转成百分比
	double trainProportion = 0.7; // 训练集
	double valProportion = 0.15;  // 验证集
	double testProportion = 0.15; // 测试集
};

//按数量划分
struct DivideByQuantityCfg
{
	bool isSmallPriority{ false };
	std::vector<ClassId> ignoreClassIds;
	DivideByProportionCfg proportionCfg;
};

struct DivideByRandomCfg
{
	DivideByProportionCfg proportionCfg;
};

enum class DataSetType
{
	YolODetection,
	YOlOSegmentation,
	YOLOOrientedBounding
};