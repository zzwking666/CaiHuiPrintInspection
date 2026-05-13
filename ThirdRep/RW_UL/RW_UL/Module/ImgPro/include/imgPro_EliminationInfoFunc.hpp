#pragma once

#include"imgPro_ImageProcessUtilty.hpp"

namespace rw
{
	namespace imgPro
	{
		struct ImageProcessContext;

		struct EliminationItem
		{
			double score = 0;//置信度
			double area = 0;//面积
			ProcessResultIndex index = -1;
			ClassId classId = 0;
			bool isBad = false;//满足剔除条件
			std::unordered_map<std::string, std::any> customFields;//自定义字段
		};

		struct EliminationInfo
		{
			std::unordered_map<ClassId, std::vector<EliminationItem>> defectItems;
		};

		struct EliminationInfoGetConfig
		{
		public:
			using MinArea = double;
			using MaxArea = double;
			using AreaRange = std::pair<MinArea, MaxArea>;
			using MinScore = double;
			using MaxScore = double;
			using ScoreRange = std::pair<MinScore, MaxScore>;
			static constexpr double negativeInfinity = -1;
			static constexpr double positiveInfinity = -1;
		public:
			bool isUsingArea = true;
			bool isUsingScore = true;
		public:
			//被剔除的项isBad设置为会被设置true

			double areaFactor{ 1 };//面积系数,可用于缩放面积
			double scoreFactor{ 1 };//置信度系数,可用于缩放置信度，用于百分制，还是万分制
			AreaRange areaRange{ negativeInfinity, positiveInfinity };//面积范围,设置时应为正值
			//如果为true，则area范围内的物体会被剔除
			//如果为false，则area范围内的物体会被保留
			//面积范围是否使用补集
			bool areaIsUsingComplementarySet{ false };
			//如果为false，则score范围内的物体会被剔除
			//如果为true，则score范围内的物体会被保留
			bool scoreIsUsingComplementarySet{ false };

			ScoreRange scoreRange{ negativeInfinity, positiveInfinity };//置信度范围，设置时应为正值
		public:
			std::unordered_map<std::string, std::any> customFields;//自定义字段，可以用于存储额外信息
		};

		using GetEliminationItemSpecialOperate = std::function<void(
			rw::imgPro::EliminationItem&,
			const rw::DetectionRectangleInfo&,
			const rw::imgPro::EliminationInfoGetConfig&
			)>;

		using ClassIdWithEliminationInfoConfigMap = std::unordered_map<ClassId, EliminationInfoGetConfig>;

		using GetEliminationItemPostOperate = std::function<void(
			const ProcessResult&, 
			const ProcessResultIndexMap&, 
			const ClassIdWithEliminationInfoConfigMap&,
			ImageProcessContext&,
			EliminationInfo &
			)>;

		struct EliminationInfoGetContext {
		public:
			GetEliminationItemSpecialOperate getEliminationItemFuncSpecialOperator;
			GetEliminationItemPostOperate getEliminationItemPostOperator;
		};

		struct EliminationInfoFunc
		{
		public:
			using Config = EliminationInfoGetConfig;
			using ClassIdWithConfigMap = ClassIdWithEliminationInfoConfigMap;
		public:
			static EliminationInfo getEliminationInfo(
				const ProcessResult& info, 
				const ProcessResultIndexMap& index,
				const ClassIdWithConfigMap& config
			);
			static EliminationInfo getEliminationInfo(
				const ProcessResult& info, 
				const ProcessResultIndexMap& index,
				const ClassIdWithConfigMap& config,
				const GetEliminationItemSpecialOperate& specialOperate
			);
			static EliminationInfo getEliminationInfo(
				const ProcessResult& info,
				const ProcessResultIndexMap& index,
				const ClassIdWithConfigMap& config,
				const GetEliminationItemSpecialOperate& specialOperate,
				const GetEliminationItemPostOperate & postOperate,
				ImageProcessContext& context
			);

		};
	}
}