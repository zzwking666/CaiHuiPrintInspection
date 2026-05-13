#pragma once

#include"ime_ModelEngine.h"
#include"ime_ModelEngineConfig.h"

#include"ime_utilty_private.hpp"

#include"NvInfer.h"

#include<string>

namespace rw
{
	namespace imet
	{
		class ModelEngine_Yolov11_det
			: public ModelEngine
		{
		private:
			struct Detection
			{
			public:
				float conf;
				int class_id;
				cv::Rect rect;
			};
		public:
			ModelEngine_Yolov11_det(const std::string& modelPath, nvinfer1::ILogger& logger);
		public:
			~ModelEngine_Yolov11_det() override;
		private:
			void init(std::string enginePath, nvinfer1::ILogger& logger);
		private:
			void infer() override;
			std::vector<DetectionRectangleInfo> postProcess() override;
			void preprocess(const cv::Mat& mat) override;
		public:
			cv::Mat draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList) override;
		private:
			float* _gpu_buffers[2];               //!< The vector of device buffers needed for engine execution
			float* _cpu_output_buffer;
			nvinfer1::IRuntime* _runtime;                 //!< The TensorRT runtime used to deserialize the engine
			nvinfer1::ICudaEngine* _engine;               //!< The TensorRT engine used to run the network
			nvinfer1::IExecutionContext* _context;        //!< The context for executing inference using an ICudaEngine

			// Model parameters
			int _input_w;
			int _input_h;
			int _num_detections;
			int _detection_attribute_size;
			int _num_classes = 80;

			ModelEngineConfig _config;
		public:
			void setConfig(const ModelEngineConfig& config)
			{
				this->_config = config;
			}
		private:
			std::vector<DetectionRectangleInfo> convertDetectionToDetectionRectangleInfo(const std::vector<Detection>& detections);
			std::vector<DetectionRectangleInfo> convertWhenResize(const std::vector<Detection>& detections);
			std::vector<DetectionRectangleInfo> convertWhenLetterBox(const std::vector<Detection>& detections);
			std::vector<DetectionRectangleInfo> convertWhenCentralCrop(const std::vector<Detection>& detections);
		private:
			int _sourceWidth{};
			int _sourceHeight{};
		private:
			float letterBoxScale{};
			int letterBoxdw{};
			int letterBoxdh{};
		private:
			PreProcess::CenterCropParams _centerCropParams;
		};
	}
}