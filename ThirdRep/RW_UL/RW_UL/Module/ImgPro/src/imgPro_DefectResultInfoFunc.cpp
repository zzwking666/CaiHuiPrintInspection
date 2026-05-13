#include"imgPro_DefectResultInfoFunc.hpp"

#include "imgPro_ImageProcess.hpp"

namespace rw
{
	namespace imgPro
	{
		DefectResultInfo DefectResultInfoFunc::getDefectResultInfo(const EliminationInfo& eliminationInfo,
		                                                           const ClassIdWithConfigMap& config, const GetDefectResultExtraOperateWhichIsDefects& getDefectResultExtraOperate,
		                                                           const GetDefectResultExtraOperateWhichIsDisableDefects& getDefectResultExtraOperateDisable, const ProcessResult&
		                                                           processResult
		)
		{
			DefectResultInfo result;
			result.isBad = false;

			for (const auto& kv : config)
			{
				ClassId classId = kv.first;
				const Config& cfg = kv.second;

				auto it = eliminationInfo.defectItems.find(classId);
				if (it != eliminationInfo.defectItems.end())
				{
					const auto& items = it->second;

					for (const auto& item : items)
					{
						if (cfg.isEnable)
						{
							if (item.isBad)
							{
								result.isBad = true;
								if (getDefectResultExtraOperate)
								{
									getDefectResultExtraOperate(item, processResult[item.index]);
								}
								result.defects[classId].push_back(item);
							}
							else
							{
								if (getDefectResultExtraOperateDisable)
								{
									getDefectResultExtraOperateDisable(item, processResult[item.index]);
								}
								result.disableDefects[classId].push_back(item);
							}
						}
						else
						{
							if (getDefectResultExtraOperateDisable)
							{
								getDefectResultExtraOperateDisable(item, processResult[item.index]);
							}
							result.disableDefects[classId].push_back(item);
						}
					}
				}
			}

			return result;
		}

		DefectResultInfo DefectResultInfoFunc::getDefectResultInfo(
			const ProcessResult& processResult,
			const ClassIdWithEliminationInfoConfigMap& classIdWithEliminationInfoConfigMap,
			const EliminationInfo& eliminationInfo, 
			const ClassIdWithConfigMap& config,
			const GetDefectResultExtraOperateWhichIsDefects& getDefectResultExtraOperate,
			const GetDefectResultExtraOperateWhichIsDisableDefects& getDefectResultExtraOperateDisable,
			const GetDefectResultExtraOperateWithFullInfo& getDefectResultExtraOperateWithFullInfo)
		{
			ImageProcessContext context{};
			return getDefectResultInfo(
				processResult,
				classIdWithEliminationInfoConfigMap,
				eliminationInfo,
				config,
				getDefectResultExtraOperate,
				getDefectResultExtraOperateDisable,
				getDefectResultExtraOperateWithFullInfo,
				GetDefectResultExtraPostOperate{}, context
			);
		}

		DefectResultInfo DefectResultInfoFunc::getDefectResultInfo(
			const ProcessResult& processResult,
			const ClassIdWithEliminationInfoConfigMap& classIdWithEliminationInfoConfigMap,
			const EliminationInfo& eliminationInfo,
			const ClassIdWithConfigMap& config,
			const GetDefectResultExtraOperateWhichIsDefects& getDefectResultExtraOperate,
			const GetDefectResultExtraOperateWhichIsDisableDefects& getDefectResultExtraOperateDisable,
			const GetDefectResultExtraOperateWithFullInfo& getDefectResultExtraOperateWithFullInfo,
			const GetDefectResultExtraPostOperate& getDefectResultExtraPostOperate, ImageProcessContext& context)
		{
			DefectResultInfo result;
			result.isBad = false;

			for (const auto& kv : config)
			{
				ClassId classId = kv.first;
				const Config& cfg = kv.second;

				auto it = eliminationInfo.defectItems.find(classId);
				auto itEliCfg = classIdWithEliminationInfoConfigMap.find(classId);
				if (it != eliminationInfo.defectItems.end())
				{
					const auto& items = it->second;

					for (const auto& item : items)
					{
						if (cfg.isEnable)
						{
							if (item.isBad)
							{
								result.isBad = true;
								if (getDefectResultExtraOperate)
								{
									getDefectResultExtraOperate(item, processResult[item.index]);
								}
								result.defects[classId].push_back(item);
							}
							else
							{
								if (getDefectResultExtraOperateDisable)
								{
									getDefectResultExtraOperateDisable(item, processResult[item.index]);
								}
								result.disableDefects[classId].push_back(item);
							}
						}
						else
						{
							if (getDefectResultExtraOperateDisable)
							{
								getDefectResultExtraOperateDisable(item, processResult[item.index]);
							}
							result.disableDefects[classId].push_back(item);
						}

						if (itEliCfg != classIdWithEliminationInfoConfigMap.end())
						{
							if (getDefectResultExtraOperateWithFullInfo)
							{
								getDefectResultExtraOperateWithFullInfo(
									processResult[item.index],
									itEliCfg->second,
									item,
									cfg,
									result,
									context
								);
							}
						}
					}
				}
			}

			if (getDefectResultExtraPostOperate)
			{
				getDefectResultExtraPostOperate(
					processResult,
					classIdWithEliminationInfoConfigMap,
					eliminationInfo,
					config,
					result,
					context

				);
			}

			return result;
		}
	}
}
