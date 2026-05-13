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
		class ModelEngine_Yolov11_obb
			: public ModelEngine
		{
		private:
			struct Detection
			{
			public:
				double conf;
				int class_id;
				float angle;
				float central_x;       // x-coordinate of the center
				float central_y;       // y-coordinate of the center
				float width;   // width of the box
				float height;  // height of the box
			};
		public:
			ModelEngine_Yolov11_obb(const std::string& modelPath, nvinfer1::ILogger& logger);
		public:
			~ModelEngine_Yolov11_obb() override;
		private:
			void init(std::string enginePath, nvinfer1::ILogger& logger);
		private:
			void infer() override;
			std::vector<DetectionRectangleInfo> postProcess() override;
			void preprocess(const cv::Mat& mat) override;
		private:
			std::vector<Detection> rotatedNmsWithKeepClass(
				const std::vector<Detection>& dets,
				float confThreshold,
				float nmsThreshold,
				const std::vector<size_t>& needKeepClassids);
		private:
			std::vector<ModelEngine_Yolov11_obb::Detection> rotatedNMS(const std::vector<ModelEngine_Yolov11_obb::Detection>& dets, double iouThreshold);
			double rotatedIoU(const ModelEngine_Yolov11_obb::Detection& a, const ModelEngine_Yolov11_obb::Detection& b);
			cv::RotatedRect toRotatedRect(const ModelEngine_Yolov11_obb::Detection& det);
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
			std::vector<DetectionRectangleInfo> convertDetectionToDetectionRectangleInfo(const std::vector<Detection>& detections);
			std::vector<Detection> convertWhenResize(const std::vector<Detection>& detections);
			std::vector<Detection> convertWhenLetterBox(const std::vector<Detection>& detections);
			std::vector<Detection> convertWhenCentralCrop(const std::vector<Detection>& detections);

		public:
			void setConfig(const ModelEngineConfig& config)
			{
				this->_config = config;
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