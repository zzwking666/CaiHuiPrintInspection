#include"ime_ModelEngineFactory.h"

#include"imet_ModelEngineFactory_TensorRT.hpp"
#include"imeo_ModelEngineFactory_OnnxRuntime.hpp"

namespace rw {
	std::unique_ptr<ModelEngine> ModelEngineFactory::createModelEngine(const ModelEngineConfig& config, ModelType modelType, ModelEngineDeployType deployType)
	{
		switch (deployType)
		{
		case rw::ModelEngineDeployType::TensorRT:
			return imet::ModelEngineFactory_TensorRT::createModelEngine(config, modelType);
		case rw::ModelEngineDeployType::OnnxRuntime:
			return imeo::ModelEngineFactory_OnnxRuntime::createModelEngine(config, modelType);
		default:
			return nullptr;
		}
	}
}