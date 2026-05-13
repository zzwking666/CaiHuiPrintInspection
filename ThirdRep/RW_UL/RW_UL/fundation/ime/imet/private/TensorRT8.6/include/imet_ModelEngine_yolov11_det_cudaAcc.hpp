#pragma once

#include"ime_ModelEngine.h"
#include"ime_ModelEngineConfig.h"

#include"ime_utilty_private.hpp"

#include"NvInfer.h"

#include<string>

#include "imet_PreProcess.cuh"

namespace rw
{
	namespace imet
	{
		class ModelEngine_yolov11_det_cudaAcc
			: public ModelEngine
		{
		public:
			ModelEngine_yolov11_det_cudaAcc(const std::string& modelPath, nvinfer1::ILogger& logger);
			ModelEngine_yolov11_det_cudaAcc(const ModelEngineConfig& modelCfg, nvinfer1::ILogger& logger);
			~ModelEngine_yolov11_det_cudaAcc() override;
		private:
			ModelEngineConfig _config;
		private:
			void init(const std::string & enginePath, nvinfer1::ILogger& logger);
		private:
			nvinfer1::IRuntime* _runtime;
			nvinfer1::ICudaEngine* _engine;
			nvinfer1::IExecutionContext* _context;
			cudaStream_t _stream;
			void init_engineRuntime(const std::string& enginePath, nvinfer1::ILogger& logger);
			void destroy_engineRuntime();
		private:
			nvinfer1::Dims _inputShape;
			nvinfer1::Dims _outputShape;
			void init_shapeInfo();
			size_t _classNum;
			size_t _detectionsNum;
			size_t _inputHeight;
			size_t _inputWidth;
			size_t _channelsNum;
			size_t _inputSize{1};
			size_t _outputSize{ 1 };
		private:
			const int kMaxNumOutputBbox = 1000;  // assume the box outputs no more than kMaxNumOutputBbox boxes that conf >= kNmsThresh;
			const int kNumBoxElement = 7;  // left, top, right, bottom, confidence, class, keepflag(whether drop when NMS)
		private:
			float* _hostOutputBuffer = nullptr;
			float* _deviceInputBuffer = nullptr;
			float* _deviceOutputBuffer = nullptr;
			float* _deviceTransposeBuffer = nullptr;
			float* _deviceDecodeBuffer = nullptr;
			void init_buffer();
			void destroy_buffer();
		private:
			size_t* _deviceClassIdNmsTogether = nullptr;
			void ini_cfg();
			void destroy_cfg();
		private:
			void warm_up();
		private:
			LetterBoxInfo _letterBoxInfo;
		private:
			void preprocess(const cv::Mat& mat) override;
			void infer() override;
			std::vector<DetectionRectangleInfo> postProcess() override;
		public:
			cv::Mat draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList) override;
		};
	}
}
