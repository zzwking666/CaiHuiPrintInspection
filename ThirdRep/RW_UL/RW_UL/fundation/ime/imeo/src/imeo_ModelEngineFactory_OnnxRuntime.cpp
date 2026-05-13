#include "imeo_ModelEngineFactory_OnnxRuntime.hpp"

#include"imeo_ModelEngine_yolov11_det.hpp"
#include"imeo_ModelEngine_yolov11_seg.hpp"
#include"imeo_ModelEngine_yolov11_obb.hpp"

#include<memory>

namespace rw
{
	namespace imeo
	{
		static ModelEngine_Yolov11_det* createModelEngine_Yolov11_det(const ModelEngineConfig& config);
		static ModelEngine_Yolov11_Seg* createModelEngine_Yolov11_Seg(const ModelEngineConfig& config);
		static ModelEngine_Yolov11_obb* createModelEngine_Yolov11_obb(const ModelEngineConfig& config);

		std::unique_ptr<ModelEngine> ModelEngineFactory_OnnxRuntime::createModelEngine(const ModelEngineConfig& config, ModelType modelType)
		{
			switch (modelType)
			{
			case ModelType::Yolov11_Det:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_det(config));
			case ModelType::Yolov11_Seg:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_Seg(config));
			case ModelType::Yolov11_Obb:
				return std::unique_ptr<ModelEngine>(createModelEngine_Yolov11_obb(config));
			default:
				return nullptr;
			}
		}

		ModelEngine_Yolov11_det* createModelEngine_Yolov11_det(const ModelEngineConfig& config)
		{
			ModelEngine_Yolov11_det* modelEngine = new ModelEngine_Yolov11_det(config.modelPath);
			if (!modelEngine) {
				return nullptr;
			}
			modelEngine->setConfig(config);

			return modelEngine;
		}

		ModelEngine_Yolov11_Seg* createModelEngine_Yolov11_Seg(const ModelEngineConfig& config)
		{
			ModelEngine_Yolov11_Seg* modelEngine = new ModelEngine_Yolov11_Seg(config.modelPath);
			if (!modelEngine) {
				return nullptr;
			}
			modelEngine->setConfig(config);

			return modelEngine;
		}

		ModelEngine_Yolov11_obb* createModelEngine_Yolov11_obb(const ModelEngineConfig& config)
		{
			ModelEngine_Yolov11_obb* modelEngine = new ModelEngine_Yolov11_obb(config.modelPath);
			if (!modelEngine) {
				return nullptr;
			}
			modelEngine->setConfig(config);
			return modelEngine;
		}
	}
}