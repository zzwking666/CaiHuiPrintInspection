#pragma once

#include "imgPro_EliminationInfoFunc.hpp"
#include"imgPro_ImageProcessUtilty.hpp"

namespace rw
{
	namespace imgPro
	{
		struct DefectResultInfo
		{
		public:
			bool isBad{};
		public:
			std::unordered_map<ClassId, std::vector<EliminationItem>> defects;
			std::unordered_map<ClassId, std::vector<EliminationItem>> disableDefects;
		};

		struct DefectResultGetConfig
		{
		public:
			bool isEnable{ false };
		};

		using GetDefectResultExtraOperateWhichIsDefects = 
			std::function<void(
				const EliminationItem&,
				const rw::DetectionRectangleInfo&
				)>;

		using GetDefectResultExtraOperateWhichIsDisableDefects = 
			std::function<void(
			const EliminationItem&,
			const rw::DetectionRectangleInfo&
			)>;

		using GetDefectResultExtraOperateWithFullInfo = 
			std::function<void(
			const rw::DetectionRectangleInfo&,
			const EliminationInfoGetConfig &,
			const EliminationItem&,
			const DefectResultGetConfig&,
			DefectResultInfo&,
			ImageProcessContext&
			)>;

		using ClassIdWithDefectResultInfoFuncConfigMap= std::unordered_map<ClassId, DefectResultGetConfig>;

		using GetDefectResultExtraPostOperate =
			std::function<void(
				const ProcessResult&,
				const ClassIdWithEliminationInfoConfigMap&,
				const EliminationInfo&,
				const ClassIdWithDefectResultInfoFuncConfigMap&,
				DefectResultInfo&,
				ImageProcessContext&
				)>;

		struct DefectResultGetContext
		{
		public:
			GetDefectResultExtraOperateWhichIsDefects getDefectResultExtraOperate{};
			GetDefectResultExtraOperateWhichIsDisableDefects getDefectResultExtraOperateDisable{};
			GetDefectResultExtraOperateWithFullInfo getDefectResultExtraOperateWithFullInfo{};
			GetDefectResultExtraPostOperate getDefectResultExtraPostOperate{};
		};


		struct DefectResultInfoFunc
		{
		public:
			using Config = DefectResultGetConfig;
			using ClassIdWithConfigMap = ClassIdWithDefectResultInfoFuncConfigMap;
		public:
			static DefectResultInfo getDefectResultInfo(
				const EliminationInfo& eliminationInfo,
				const ClassIdWithConfigMap& config,
				const GetDefectResultExtraOperateWhichIsDefects& getDefectResultExtraOperate,
				const GetDefectResultExtraOperateWhichIsDisableDefects& getDefectResultExtraOperateDisable, const ProcessResult&
				processResult
			);

			//Extra operate with full info
			static DefectResultInfo getDefectResultInfo(
				const ProcessResult& processResult,
				const ClassIdWithEliminationInfoConfigMap& classIdWithEliminationInfoConfigMap,
				const EliminationInfo& eliminationInfo,
				const ClassIdWithConfigMap& config,
				const GetDefectResultExtraOperateWhichIsDefects& getDefectResultExtraOperate,
				const GetDefectResultExtraOperateWhichIsDisableDefects& getDefectResultExtraOperateDisable,
				const GetDefectResultExtraOperateWithFullInfo& getDefectResultExtraOperateWithFullInfo
			);

			static DefectResultInfo getDefectResultInfo(
				const ProcessResult& processResult,
				const ClassIdWithEliminationInfoConfigMap& classIdWithEliminationInfoConfigMap,
				const EliminationInfo& eliminationInfo,
				const ClassIdWithConfigMap& config,
				const GetDefectResultExtraOperateWhichIsDefects& getDefectResultExtraOperate,
				const GetDefectResultExtraOperateWhichIsDisableDefects& getDefectResultExtraOperateDisable,
				const GetDefectResultExtraOperateWithFullInfo& getDefectResultExtraOperateWithFullInfo,
				const GetDefectResultExtraPostOperate& getDefectResultExtraPostOperate, ImageProcessContext& context
			);
		};
	}
}