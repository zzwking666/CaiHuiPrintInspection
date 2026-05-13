#include"imet_ModelEngineFactory_TensorRT.hpp"

#include"imet_ModelEngine_yolov11_det.hpp"
#include "imet_ModelEngine_yolov11_det_cudaAcc.hpp"
#include"imet_ModelEngine_yolov11_seg.hpp"
#include"imet_ModelEngine_yolov11_obb.hpp"
#include "imet_ModelEngine_yolov11_seg_cudaAcc.hpp"
#include "imet_ModelEngine_yolov11_seg_mask.hpp"
#include "imet_ModelEngine_yolov11_seg_mask_cudaAcc.hpp"
#include <fmt/format.h>  
#include <fmt/ranges.h>  

#include "lgm_PreDef.hpp"

class Logger : public nvinfer1::ILogger {
	void log(Severity severity, const char* msg) noexcept override {
	}
}logger;

namespace rw {
	namespace imet {
		static ModelEngine_Yolov11_det* createModelEngine_Yolov11_det(const ModelEngineConfig& config);
		static ModelEngine_yolov11_det_cudaAcc* createModelEngine_Yolov11_det_cuda_acc(const ModelEngineConfig& config);
		static ModelEngine_Yolov11_seg* createModelEngine_Yolov11_seg(const ModelEngineConfig& config);
		static ModelEngine_yolov11_seg_cudaAcc* createModelEngine_yolov11_seg_cudaAcc(const ModelEngineConfig& config);
		static ModelEngine_Yolov11_obb* createModelEngine_Yolov11_obb(const ModelEngineConfig& config);
		static ModelEngine_Yolov11_seg_mask* createModelEngine_Yolov11_seg_with_mask(const ModelEngineConfig& config);
		static ModelEngine_yolov11_seg_mask_cudaAcc* createModelEngine_Yolov11_seg_with_mask_cudaAcc(const ModelEngineConfig& config);

		std::unique_ptr<ModelEngine>
			ModelEngineFactory_TensorRT::createModelEngine
			(const ModelEngineConfig& config, ModelType modelType)
		{
			std::unique_ptr<ModelEngine> modelEngine = nullptr;
			switch (modelType)
			{
			case ModelType::Yolov11_Det:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_det(config));
			case ModelType::Yolov11_Det_CudaAcc:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_det_cuda_acc(config));
			case ModelType::Yolov11_Seg:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_seg(config));
			case ModelType::Yolov11_Seg_CudaAcc:
				return std::unique_ptr<ModelEngine>(createModelEngine_yolov11_seg_cudaAcc(config));
			case ModelType::Yolov11_Obb:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_obb(config));
			case ModelType::Yolov11_Seg_Mask:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_seg_with_mask(config));
			case ModelType::Yolov11_Seg_Mask_CudaAcc:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_seg_with_mask_cudaAcc(config));
			default:
				return nullptr;
			}
		}

		ModelEngine_Yolov11_det* createModelEngine_Yolov11_det(const ModelEngineConfig& config)
		{

			LOG_INFO("imet:Building Yolov11 detection ....");
			ModelEngine_Yolov11_det* modelEngine = new ModelEngine_Yolov11_det(config.modelPath, logger);
			if (!modelEngine) {
				return nullptr;
			}

			modelEngine->setConfig(config);

			LOG_INFO("imet:modelPath is \"{}\" ",config.modelPath);
			LOG_INFO("imet:conf_threshold is {}",config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));
			LOG_INFO("imet:Build success");

			return modelEngine;
		}

		ModelEngine_yolov11_det_cudaAcc* createModelEngine_Yolov11_det_cuda_acc(const ModelEngineConfig& config)
		{
			LOG_INFO("imet:Building Yolov11 detection cudaAcc ....");
			ModelEngine_yolov11_det_cudaAcc* modelEngine = new ModelEngine_yolov11_det_cudaAcc(config, logger);
			if (!modelEngine) {
				return nullptr;
			}
			LOG_INFO("imet:modelPath is \"{}\" ", config.modelPath);
			LOG_INFO("imet:conf_threshold is {}", config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));			LOG_INFO("imet:Build Yolov11 detection success!");
			LOG_INFO("imet:Build success");

			return modelEngine;
		}

		ModelEngine_Yolov11_seg* createModelEngine_Yolov11_seg(const ModelEngineConfig& config)
		{
			LOG_INFO("imet:Building Yolov11 segment ....");
			ModelEngine_Yolov11_seg* modelEngine = new ModelEngine_Yolov11_seg(config.modelPath, logger);
			if (!modelEngine) {
				return nullptr;
			}
			modelEngine->setConfig(config);

			LOG_INFO("imet:modelPath is \"{}\" ", config.modelPath);
			LOG_INFO("imet:conf_threshold is {}", config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));			LOG_INFO("imet:Build Yolov11 detection success!");
			LOG_INFO("imet:Build success");
			return modelEngine;
		}

		ModelEngine_yolov11_seg_cudaAcc* createModelEngine_yolov11_seg_cudaAcc(const ModelEngineConfig& config)
		{
			LOG_INFO("imet:Building Yolov11 segment cudaAcc ....");

			ModelEngine_yolov11_seg_cudaAcc* modelEngine = new ModelEngine_yolov11_seg_cudaAcc(config, logger);
			if (!modelEngine) {
				return nullptr;
			}

			LOG_INFO("imet:modelPath is \"{}\" ", config.modelPath);
			LOG_INFO("imet:conf_threshold is {}", config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));			LOG_INFO("imet:Build Yolov11 detection success!");
			LOG_INFO("imet:Build success");

			return modelEngine;
		}

		ModelEngine_Yolov11_obb* createModelEngine_Yolov11_obb(const ModelEngineConfig& config)
		{
			LOG_INFO("imet:Building Yolov11 obb ....");


			ModelEngine_Yolov11_obb* modelEngine = new ModelEngine_Yolov11_obb(config.modelPath, logger);
			if (!modelEngine) {
				return nullptr;
			}
			modelEngine->setConfig(config);

			LOG_INFO("imet:modelPath is \"{}\" ", config.modelPath);
			LOG_INFO("imet:conf_threshold is {}", config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));			LOG_INFO("imet:Build Yolov11 detection success!");
			LOG_INFO("imet:Build success");

			return modelEngine;
		}

		ModelEngine_Yolov11_seg_mask* createModelEngine_Yolov11_seg_with_mask(const ModelEngineConfig& config)
		{
			LOG_INFO("imet:Building Yolov11 segment with mask ....");

			ModelEngine_Yolov11_seg_mask* modelEngine = new ModelEngine_Yolov11_seg_mask(config.modelPath, logger);
			if (!modelEngine) {
				return nullptr;
			}

			LOG_INFO("imet:modelPath is \"{}\" ", config.modelPath);
			LOG_INFO("imet:conf_threshold is {}", config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));			LOG_INFO("imet:Build Yolov11 detection success!");
			LOG_INFO("imet:Build success");

			modelEngine->setConfig(config);
			return modelEngine;
		}

		ModelEngine_yolov11_seg_mask_cudaAcc* createModelEngine_Yolov11_seg_with_mask_cudaAcc(
			const ModelEngineConfig& config)
		{
			LOG_INFO("imet:Building Yolov11 segment with mask and cudaAcc ....");

			ModelEngine_yolov11_seg_mask_cudaAcc* modelEngine = new ModelEngine_yolov11_seg_mask_cudaAcc(config, logger);
			if (!modelEngine) {
				return nullptr;
			}

			LOG_INFO("imet:modelPath is \"{}\" ", config.modelPath);
			LOG_INFO("imet:conf_threshold is {}", config.conf_threshold);
			LOG_INFO("imet:nms_threshold is {}", config.nms_threshold);
			LOG_INFO("imet:classids_nms_together is [{}]", fmt::join(config.classids_nms_together, ", "));			LOG_INFO("imet:Build Yolov11 detection success!");
			LOG_INFO("imet:Build success");

			return modelEngine;
		}
	}
}