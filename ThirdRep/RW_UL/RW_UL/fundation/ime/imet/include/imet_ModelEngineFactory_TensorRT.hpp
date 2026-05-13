#pragma once

#include"ime_ModelEngine.h"
#include"ime_ModelEngineConfig.h"
#include<memory>

namespace rw {
	namespace imet {
		class ModelEngineFactory_TensorRT
		{
		public:
			static std::unique_ptr<ModelEngine> createModelEngine(const ModelEngineConfig& config, ModelType modelType);
		};
	}
}