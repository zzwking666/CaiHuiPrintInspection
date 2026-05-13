#include"imgPro_DefectDrawFunc.hpp"

#include "imgPro_DefectResultInfoFunc.hpp"
#include"imgPro_ImagePainter.hpp"

namespace rw
{
	namespace imgPro
	{
		void DefectDrawFunc::ConfigDefectDraw::setAllIdsWithSameColor(const std::vector<ClassId>& ids, Color color,
			bool isGood)
		{
			for (const auto& id : ids)
			{
				if (classIdWithConfigMap.find(id)!= classIdWithConfigMap.end())
				{
					if (isGood)
					{
						classIdWithConfigMap[id].defectColorGood = color;
					}
					else
					{
						classIdWithConfigMap[id].defectColorBad = color;
					}
				}
				
			}
		}

		void DefectDrawFunc::drawDefectRecs(
			QImage& img, 
			const DefectResultInfo& info,
			const ProcessResult& processResult,
			const ConfigDefectDraw& config, 
			DefectDrawFuncContext& context
		)
		{
			if (img.isNull() || processResult.empty()) {
				return; 
			}

			context.ignoreItems.clear();

			if (config.isDrawDefects)
			{
				drawDefectGroup(
					img,
					info.defects,
					processResult,
					config,
					Color::Red,
					context, 
					true
				);
			}

			if (config.isDrawDisableDefects)
			{
				drawDefectGroup(
					img,
					info.disableDefects,
					processResult,
					config,
					Color::Green,
					context, 
					false
				);
			}
		}

		void DefectDrawFunc::drawRunText(QImage& img, const ConfigRunText& config)
		{
			QVector<QString> textList;
			std::vector<Color> configList;
			auto textColor = config.operatorTimeTextColor;
			if (config.isDisOperatorTime)
			{
				configList.push_back(textColor);
				textList.push_back(config.operatorTimeText);
			}
			textColor = config.processImgTimeTextColor;
			if (config.isDisProcessImgTime)
			{
				configList.push_back(textColor);
				textList.push_back(config.processImgTimeText);
			}

			if (config.isDrawExtraText)
			{
				textColor = config.extraTextColor;
				configList.push_back(textColor);
				for (const auto& item : config.extraTexts)
				{
					textList.push_back(item);
				}
			}

			rw::imgPro::ImagePainter::drawTextOnImage(img, textList, configList, config.runTextProportion);
		}

		void DefectDrawFunc::drawDefectGroup(
			QImage& img,
			const std::unordered_map<ClassId, std::vector<EliminationItem>>& group,
			const ProcessResult& processResult,
			const DefectDrawFunc::ConfigDefectDraw& config,
			Color defaultColor,
			DefectDrawFuncContext& context, bool isDefect
		)
		{
			DefectDrawFunc::ConfigDefectDraw cfg = config;

			rw::imgPro::ConfigDrawRect recCfg;
			recCfg.fontSize = cfg.fontSize;
			recCfg.thickness = cfg.thickness;
			recCfg.textLocate = cfg.textLocate;
			recCfg.isRegion = cfg.isDrawMask;
			recCfg.alpha = cfg.alpha;
			recCfg.thresh = cfg.thresh;
			recCfg.maxVal = cfg.maxVal;
			recCfg.hasFrame = cfg.hasFrame;
			recCfg.rectColor = defaultColor;
			recCfg.textColor = defaultColor;

			for (const auto& pairs : group)
			{
				auto& classId = pairs.first;

				if (cfg.classIdIgnoreDrawSet.find(classId) != cfg.classIdIgnoreDrawSet.end())
				{
					auto& vec = context.ignoreItems[classId];
					vec.insert(vec.end(), pairs.second.begin(), pairs.second.end());
					continue;
				}
				QString processTextPre = (cfg.classIdNameMap.find(classId) != cfg.classIdNameMap.end()) ?
					cfg.classIdNameMap.at(classId) : QString::number(classId);

				rw::imgPro::ConfigDrawRect tempCfg = recCfg;
				if (cfg.classIdWithConfigMap.find(classId) != cfg.classIdWithConfigMap.end())
				{
					auto& idCfg = cfg.classIdWithConfigMap.at(classId);
					tempCfg.alpha = idCfg.alpha;
					tempCfg.thresh = idCfg.thresh;
					tempCfg.maxVal = idCfg.maxVal;
					tempCfg.hasFrame = idCfg.hasFrame;
					tempCfg.fontSize = idCfg.fontSize;
					tempCfg.thickness = idCfg.thickness;
					tempCfg.textLocate = idCfg.textLocate;
					tempCfg.isRegion = idCfg.isDrawMask;
					if (isDefect)
					{
						tempCfg.rectColor = idCfg.defectColorBad;
						tempCfg.textColor = idCfg.defectColorBad;
					}
					else
					{
						tempCfg.rectColor = idCfg.defectColorGood;
						tempCfg.textColor = idCfg.defectColorGood;
					}

					cfg.isDisAreaText = idCfg.isDisAreaText;
					cfg.isDisScoreText = idCfg.isDisScoreText;
					cfg.areaDisPrecision = idCfg.areaDisPrecision;
					cfg.scoreDisPrecision = idCfg.scoreDisPrecision;
					cfg.alpha = idCfg.alpha;
					cfg.thresh = idCfg.thresh;
					cfg.maxVal = idCfg.maxVal;
					cfg.hasFrame = idCfg.hasFrame;
					cfg.isDrawMask = idCfg.isDrawMask;
					cfg.isDisName = idCfg.isDisName;
				}
				else
				{
					cfg.isDisAreaText = config.isDisAreaText;
					cfg.isDisScoreText = config.isDisScoreText;
					cfg.areaDisPrecision = config.areaDisPrecision;
					cfg.scoreDisPrecision = config.scoreDisPrecision;
					cfg.alpha = config.alpha;
					cfg.thresh = config.thresh;
					cfg.maxVal = config.maxVal;
					cfg.hasFrame = config.hasFrame;
					cfg.isDrawMask = config.isDrawMask;
					cfg.isDisName = config.isDisName;
				}

				for (const auto& item : pairs.second)
				{
					tempCfg.text.clear();
					if (cfg.isDisName)
					{
						tempCfg.text = processTextPre;
					}
					if (cfg.isDisScoreText)
					{
						tempCfg.text = tempCfg.text + " : " + QString::number(item.score, 'f', cfg.scoreDisPrecision);
					}
					if (cfg.isDisAreaText)
					{
						tempCfg.text = tempCfg.text + " , " + QString::number(item.area, 'f', cfg.areaDisPrecision);
					}

					auto& proResult = processResult[item.index];

					if (proResult.segMaskValid && cfg.isDrawMask)
					{
						rw::imgPro::ConfigDrawMask maskCfg;
						maskCfg.alpha = cfg.alpha;
						maskCfg.thresh = cfg.thresh;
						maskCfg.maxVal = cfg.maxVal;
						maskCfg.maskColor = tempCfg.rectColor;
						maskCfg.hasFrame = cfg.hasFrame;
						maskCfg.rectCfg = tempCfg;
						maskCfg.rectCfg.rectColor = tempCfg.rectColor;
						maskCfg.rectCfg.isRegion = false;
						rw::imgPro::ImagePainter::drawMaskOnSourceImg(img, proResult, maskCfg);
					}
					else
					{
						rw::imgPro::ImagePainter::drawShapesOnSourceImg(img, proResult, tempCfg);
					}
				}
			}
		}
	}
}