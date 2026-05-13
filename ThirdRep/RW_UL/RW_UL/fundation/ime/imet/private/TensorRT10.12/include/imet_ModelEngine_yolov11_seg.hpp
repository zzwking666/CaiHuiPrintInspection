#pragma once

#include"ime_ModelEngine.h"

#include"NvInfer.h"

#include"ime_ModelEngineConfig.h"
#include"ime_utilty_private.hpp"

#include<string>

namespace rw {
	namespace imet {
		class ModelEngine_Yolov11_seg
			:public ModelEngine
		{
		private:
			struct DetectionSeg
			{
				float conf{};
				int class_id{};
				cv::Rect bbox{};
			};
		private:
			void preprocess(const cv::Mat& mat) override;
			void infer() override;
			std::vector<DetectionRectangleInfo> postProcess() override;
		private:
			void init(const std::string& enginePath, nvinfer1::ILogger& logger);
		public:
			ModelEngine_Yolov11_seg(const std::string& modelPath, nvinfer1::ILogger& logger);
			~ModelEngine_Yolov11_seg() override;
		private:
			std::vector<DetectionRectangleInfo> convertToDetectionRectangleInfo(const std::vector<DetectionSeg>& detections);
			std::vector<DetectionRectangleInfo> convertWhenResize(const std::vector<DetectionSeg>& detections);
			std::vector<DetectionRectangleInfo> convertWhenLetterBox(const std::vector<DetectionSeg>& detections);
			std::vector<DetectionRectangleInfo> convertWhenCentralCrop(const std::vector<DetectionSeg>& detections);
		public:
			cv::Mat draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList) override;

			float* gpu_buffers[3];               //!< The vector of device buffers needed for engine execution
			float* cpu_output_buffer;
			float* cpu_output_buffer2;
			nvinfer1::IRuntime* runtime;                 //!< The TensorRT runtime used to deserialize the engine
			nvinfer1::ICudaEngine* engine;               //!< The TensorRT engine used to run the network
			nvinfer1::IExecutionContext* context;        //!< The context for executing inference using an ICudaEngine

			// Model parameters
			int input_w;
			int input_h;
			int num_detections;
			int maskCoefficientNum;
			int detection_attribute_size;
			int num_classes = 80;
			rw::ModelEngineConfig config;
		public:
			void setConfig(const ModelEngineConfig& modelConfig)
			{
				config = modelConfig;
			}
		private:
			int _sourceWidth{};
			int _sourceHeight{};
		private:
			float _letterBoxScale{};
			int _letterBoxdw{};
			int _letterBoxdh{};
		private:
			PreProcess::CenterCropParams _centerCropParams;
		};
	}
}