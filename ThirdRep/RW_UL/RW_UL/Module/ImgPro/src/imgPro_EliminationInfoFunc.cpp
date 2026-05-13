#include"imgPro_EliminationInfoFunc.hpp"

#include "imgPro_ImageProcess.hpp"

namespace rw
{
	namespace imgPro
	{
		EliminationInfo EliminationInfoFunc::getEliminationInfo(const ProcessResult& info,
			const ProcessResultIndexMap& index, const ClassIdWithConfigMap& config)
		{
			return getEliminationInfo(info, index, config, GetEliminationItemSpecialOperate{});
		}

		EliminationInfo EliminationInfoFunc::getEliminationInfo(const ProcessResult& info,
		                                                        const ProcessResultIndexMap& index, const ClassIdWithConfigMap& config,
		                                                        const GetEliminationItemSpecialOperate& specialOperate)
		{
			ImageProcessContext context{};
			return getEliminationInfo(info, index, config, GetEliminationItemSpecialOperate{}, GetEliminationItemPostOperate{}, context);
		}

		EliminationInfo EliminationInfoFunc::getEliminationInfo(const ProcessResult& info,
		                                                        const ProcessResultIndexMap& index, const ClassIdWithConfigMap& config,
		                                                        const GetEliminationItemSpecialOperate& specialOperate, const GetEliminationItemPostOperate& postOperate, ImageProcessContext
		                                                        & context)
		{
			EliminationInfo result;

			for (const auto& kv : index)
			{
				ClassId classId = kv.first;
				const auto& indices = kv.second;

				// 获取配置，没有则用默认配置
				auto cfgIt = config.find(classId);
				const EliminationInfoGetConfig& cfg = (cfgIt != config.end()) ? cfgIt->second : EliminationInfoGetConfig();

				std::vector<EliminationItem> items;

				for (ProcessResultIndex idx : indices)
				{
					if (idx >= info.size()) continue;
					const auto& det = info[idx];

					// 应用系数
					double area = det.area * cfg.areaFactor;
					double score = det.score * cfg.scoreFactor;

					// 面积范围判断
					bool areaInRange = (cfg.areaRange.first == EliminationInfoGetConfig::negativeInfinity || area >= cfg.areaRange.first)
						&& (cfg.areaRange.second == EliminationInfoGetConfig::positiveInfinity || area <= cfg.areaRange.second);

					// 分数范围判断
					bool scoreInRange = (cfg.scoreRange.first == EliminationInfoGetConfig::negativeInfinity || score >= cfg.scoreRange.first)
						&& (cfg.scoreRange.second == EliminationInfoGetConfig::positiveInfinity || score <= cfg.scoreRange.second);

					// 是否剔除
					bool isBad = false;

					// 面积剔除逻辑
					if (cfg.areaIsUsingComplementarySet)
					{
						areaInRange = !areaInRange;
					}

					// 分数剔除逻辑
					if (cfg.scoreIsUsingComplementarySet)
					{
						scoreInRange = !scoreInRange;
					}

					bool temp{ true };
					if (cfg.isUsingArea)
					{
						temp = temp && !areaInRange;
					}
					if (cfg.isUsingScore)
					{
						temp = temp && !scoreInRange;
					}

					isBad = isBad || temp;

					EliminationItem item;
					item.score = score;
					item.area = area;
					item.index = idx;
					item.isBad = isBad;
					item.classId = classId;

					if (specialOperate)
					{
						specialOperate(item, det, cfg);
					}

					items.push_back(std::move(item));
				}

				result.defectItems[classId] = std::move(items);
			}

			if (postOperate)
			{
				postOperate(info, index, config,context, result);
			}

			return result;
		}
	}
}
