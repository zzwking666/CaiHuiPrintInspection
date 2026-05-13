#pragma once
#include"opencv2/opencv.hpp"

#include"ime_ModelEngine.h"
#include"ime_utilty_private.hpp"
#include"ime_ModelEngineConfig.h"

#include"onnxruntime_cxx_api.h"
#include <string>
#include <locale>

namespace rw {
	namespace imeo {
		class ModelEngine_Yolov11_det
			:public ModelEngine {
		private:
			struct Detection
			{
				float conf;
				int class_id;
				cv::Rect bbox;
			};
		public:
			ModelEngine_Yolov11_det(const std::string& modelPath);
			~ModelEngine_Yolov11_det() override;
		public:
			std::string  input_name;
			std::string  output_name;
		private:
			void preprocess(const cv::Mat& mat) override;
			void infer() override;
			std::vector<DetectionRectangleInfo> postProcess() override;
		private:
			void init(const std::string& engine_path);
		private:
			Ort::Env _env;
			Ort::Session _session = Ort::Session(nullptr);
			std::vector<Ort::Value>_output_tensors;
			std::vector<const char*> _input_node_names;
			std::vector<const char*> _output_node_names;
			Ort::Value _input_tensor = Ort::Value(nullptr);
			std::vector<Ort::Value> _ort_inputs;
			cv::Mat _infer_image;
		private:
			float* _cpu_output_buffer;
			int _input_w;
			int _input_h;
			int _num_detections;
			int _detection_attribute_size;
			int _num_classes = 80;

			ModelEngineConfig _config;
		private:
			std::vector<DetectionRectangleInfo> convertDetectionToDetectionRectangleInfo(const std::vector<Detection>& detections);
			std::vector<DetectionRectangleInfo> convertWhenResize(const std::vector<Detection>& detections);
			std::vector<DetectionRectangleInfo> convertWhenLetterBox(const std::vector<Detection>& detections);
			std::vector<DetectionRectangleInfo> convertWhenCentralCrop(const std::vector<Detection>& detections);
		public:
			cv::Mat draw(const cv::Mat& mat, const std::vector<DetectionRectangleInfo>& infoList) override;

		private:
			int _sourceWidth{};
			int _sourceHeight{};
		private:
			static std::wstring stringToWString(const std::string& str);
		public:
			void setConfig(const ModelEngineConfig& modelConfig)
			{
				this->_config = modelConfig;
			}
		private:
			float _letterBoxScale{};
			int _letterBoxdw{};
			int _letterBoxdh{};
		private:
			PreProcess::CenterCropParams _centerCropParams;
		};
	}
}