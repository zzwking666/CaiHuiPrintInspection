#pragma once

#include"ime_ModelEngine.h"
#include"ime_ModelEngineConfig.h"
#include<memory>

namespace rw {
	namespace imeo {
		class ModelEngineFactory_OnnxRuntime
		{
		public:
			static std::unique_ptr<ModelEngine> createModelEngine(const ModelEngineConfig& config, ModelType modelType);
		};
	}
}