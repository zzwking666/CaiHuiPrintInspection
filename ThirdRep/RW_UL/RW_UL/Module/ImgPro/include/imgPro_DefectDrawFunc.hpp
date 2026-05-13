#pragma once

#include "imgPro_DefectResultInfoFunc.hpp"
#include"imgPro_ImageProcessUtilty.hpp"

#include"rqw_rqwColor.hpp"

#include<QImage>

#include "imgPro_ImagePainter.hpp"
#include<unordered_set>

namespace rw
{
	namespace imgPro
	{

		struct DefectDrawConfigItem
		{
		public:
			bool isDisName{true};
			bool isDisScoreText{ true };
			bool isDisAreaText{ true };
		public:
			bool isDrawMask{ false };
			double alpha{ 0.3 };
			double thresh{ 0.5 };
			double maxVal{ 1.0 };
			bool hasFrame{ true };
		public:
			int areaDisPrecision{ 1 };
			int scoreDisPrecision{ 1 };
		public:
			ConfigDrawRect::TextLocate textLocate{ ConfigDrawRect::TextLocate::LeftTopOut };
		public:
			int fontSize{ 30 };
			int thickness{ 3 };
		public:
			Color defectColorGood{Color::Green};
			Color defectColorBad{ Color::Red };
		};

		struct DefectDrawConfig
		{
		public:
			bool isDisName{ true };
			bool isDrawDefects{ true };
			bool isDrawDisableDefects{ true };
		public:
			std::unordered_map<ClassId, ClassIdName> classIdNameMap;
		public:
			bool isDisScoreText{ true };
			bool isDisAreaText{ true };
		public:
			void setAllIdsWithSameColor(
				const std::vector<ClassId>& ids,
				Color color, 
				bool isGood
			);
		public:
			std::unordered_map<ClassId, DefectDrawConfigItem> classIdWithConfigMap;
		public:
			int fontSize{ 30 };
			int thickness{ 3 };
		public:
			ConfigDrawRect::TextLocate textLocate{ ConfigDrawRect::TextLocate::LeftTopOut };
		public:
			bool isDrawMask{ false };
			double alpha{ 0.3 };
			double thresh{ 0.5 };
			double maxVal{ 1.0 };
			bool hasFrame{ true };
		public:
			int areaDisPrecision{1};
			int scoreDisPrecision{ 1};
		public:
			std::unordered_set<ClassId> classIdIgnoreDrawSet{};
		};

		struct RunTextConfig
		{
		public:
			bool isDrawExtraText{ true };
			QVector<QString> extraTexts{};
			Color extraTextColor{ Color::Red };
		public:
			bool isDisProcessImgTime{ true };
			QString processImgTimeText{};
			Color processImgTimeTextColor{ Color::Blue };
		public:
			bool isDisOperatorTime{ true };
			QString operatorTimeText{};
			Color operatorTimeTextColor{ Color::Green };
		public:
			double runTextProportion = 0.06;
		};

		struct ImageProcessContext;

		using DefectDrawFuncPostOperate=std::function<void(
			QImage& img,
			ImageProcessContext& context
			)>;

		struct DefectDrawFuncContext
		{
		public:
			DefectDrawFuncPostOperate postOperateFunc{ };
		public:
			std::unordered_map<ClassId, std::vector<EliminationItem>> ignoreItems{};
		};

		struct DefectDrawFunc
		{
		public:
			using ConfigDefectDraw = DefectDrawConfig;
			using ConfigRunText = RunTextConfig;
		public:
			static void drawDefectRecs(
				QImage& img,
				const DefectResultInfo& info,
				const ProcessResult& processResult,
				const ConfigDefectDraw& config, DefectDrawFuncContext& context
			);

			static void drawRunText(
				QImage& img, 
				const ConfigRunText& config
			);
		private:
			static void drawDefectGroup(
				QImage& img,
				const std::unordered_map<ClassId, std::vector<EliminationItem>>& group,
				const ProcessResult& processResult,
				const DefectDrawFunc::ConfigDefectDraw& config,
				Color defaultColor, DefectDrawFuncContext& context, bool isDefect
			);
            
		};
	}
}