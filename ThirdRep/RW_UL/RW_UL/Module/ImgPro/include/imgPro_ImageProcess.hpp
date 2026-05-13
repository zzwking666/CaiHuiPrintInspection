#pragma once

#include"ime_ModelEngine.h"
#include "imgPro_DefectDrawFunc.hpp"
#include "imgPro_DefectResultInfoFunc.hpp"
#include "imgPro_EliminationInfoFunc.hpp"
#include "imgPro_IndexFunc.hpp"

namespace rw
{
	namespace imgPro
	{
		class ImageProcess;
		struct ImageProcessContext;
		using ImageProcessPrepare = std::function<void(ImageProcessContext&)>;
		

		struct ImageProcessContext
		{
			friend ImageProcess;
		public:
			ImageProcessPrepare imageProcessPrepare{};
		public:
			IndexGetContext indexGetContext{};
		public:
			EliminationInfoFunc::ClassIdWithConfigMap eliminationCfg{};
			EliminationInfoGetContext eliminationInfoGetContext{};
		public:
			DefectResultInfoFunc::ClassIdWithConfigMap defectCfg{};
			DefectResultGetContext defectResultGetContext{};
		public:
			DefectDrawFunc::ConfigDefectDraw defectDrawCfg{};
			DefectDrawFunc::ConfigRunText runTextCfg{};
		public:
			DefectDrawFuncContext defectDrawFuncContext{};
		private:
			ProcessResult _processResult{};
			RunTime _processImgTime{};
			RunTime _operatorTime{};
			ProcessResultIndexMap _processResultIndexMap{};
		public:
			const ProcessResult& getProcessResult() const
			{
				return _processResult;
			}
			ProcessResult& processResult()
			{
				return _processResult;
			}
			const RunTime & getProcessImgTime() const
			{
				return _processImgTime;
			}
			const RunTime & getOperatorTime() const
			{
				return _operatorTime;
			}

			const ProcessResultIndexMap& getProcessResultIndexMap() const
			{
				return _processResultIndexMap;
			}
		public:
			std::unordered_map<std::string, std::any> customFields;
		private:
			cv::Mat _currentMat;
		public:
			bool isUseCurrentMat{false};
		public:
			const cv::Mat& getCurrentMat() const
			{
				return _currentMat;
			}
			cv::Mat& currentMat()
			{
				return _currentMat;
			}
		};

		class ImageProcess
		{
		public:
			explicit ImageProcess(std::unique_ptr<rw::ModelEngine>& engine);
			~ImageProcess();
		private:
			ImageProcessContext _context{};
		public:
			const ImageProcessContext& getContext() const
			{
				return _context;
			}

			ImageProcessContext& context()
			{
				return _context;
			}
		private:
			std::unique_ptr<rw::ModelEngine> _engine = nullptr;
		public:
			std::unique_ptr<rw::ModelEngine>& getModelEngine();
		public:
			RunTime getProcessImgTime() const
			{
				return _context._processImgTime;
			}
			RunTime getOperatorTime() const
			{
				return _context._operatorTime;
			}
		public:
			ProcessResult processImg(const cv::Mat& mat);
			ProcessResultIndexMap getIndex(const ProcessResult& processResult);
			EliminationInfo getEliminationInfo(
				const ProcessResult& processResult, 
				const ProcessResultIndexMap& indexMap,
				const rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap& configs
			);
			DefectResultInfo getDefectResultInfo(
				const ProcessResult& processResult,
				const EliminationInfo& eliminationInfo, 
				const rw::imgPro::DefectResultInfoFunc::ClassIdWithConfigMap& configs,
				const rw::imgPro::EliminationInfoFunc::ClassIdWithConfigMap& eliConfigs
			);
		public:
			void operator()(const cv::Mat& mat);
			QImage getMaskImg(const cv::Mat& mat);
			static QImage getMaskImg(const cv::Mat& mat,
				const DefectResultInfo& defectResultInfo,
				const ProcessResult& processResult,
				ImageProcessContext& context,
				RunTime operatorTime,
				RunTime processImgTime
			);
			static void getMaskImg(QImage& img,
				const DefectResultInfo& defectResultInfo,
				const ProcessResult& processResult,
				ImageProcessContext& context,
				RunTime operatorTime,
				RunTime processImgTime
			);
		private:
			EliminationInfo _eliminationInfo{};
			DefectResultInfo _defectResultInfo{};
		public:
			const ProcessResultIndexMap& getProcessResultIndexMap() const
			{
				return _context._processResultIndexMap;
			}
			const ProcessResult& getProcessResult() const
			{
				return _context._processResult;
			}
			const EliminationInfo& getEliminationInfo() const
			{
				return _eliminationInfo;
			}
			const DefectResultInfo& getDefectResultInfo() const
			{
				return _defectResultInfo;
			}

			ProcessResultIndexMap& processResultIndexMap()
			{
				return _context._processResultIndexMap;
			}
			ProcessResult& processResult()
			{
				return _context._processResult;
			}
			EliminationInfo& eliminationInfo()
			{
				return _eliminationInfo;
			}
			DefectResultInfo& defectResultInfo()
			{
				return _defectResultInfo;
			}
		};
	}
}