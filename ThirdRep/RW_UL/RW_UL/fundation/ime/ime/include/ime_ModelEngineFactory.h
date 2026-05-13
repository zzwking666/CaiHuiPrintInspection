#pragma once

#include"ime_ModelEngineConfig.h"
#include"ime_ModelEngine.h"

namespace rw {
	class ModelEngineFactory {
	public:
		static std::unique_ptr<ModelEngine>
			createModelEngine(const ModelEngineConfig& config,
				ModelType modelType,
				ModelEngineDeployType deployType);
	};
}