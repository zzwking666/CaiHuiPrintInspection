#pragma once

#include"rqw_HalconUtilty.hpp"
#include"rqw_HalconWidgetDisObject.hpp"
#include"rqw_HalconModelConfig.hpp"

namespace rw {
	namespace rqw
	{
		class HalconShapeModel {
		public:
			static HalconShapeId create(const HalconWidgetImg& img, const HalconWidgetObject& rec);
			static HalconShapeId create(const HalconWidgetObject* img, const HalconWidgetObject* rec);
			static HalconShapeId create(const HalconWidgetObject& img, const HalconWidgetObject* rec);
			static HalconShapeId create(const HalconWidgetObject* img, const HalconWidgetObject& rec);
		public:
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& modelId, const HalconWidgetObject& img);
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& modelId, const HalconWidgetObject* img);
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& modelId, const HalconWidgetObject* img, const PainterConfig& config);
		public:
			static void saveModel(const HalconShapeId& id, const std::string& filePath);
			static HalconShapeId readModel(const std::string& filePath);
		};

		class HalconShapeXLDModel {
		public:
			static HalconShapeId createXLD(const HalconWidgetObject& img, const std::vector<HalconWidgetObject>& recs);
			static HalconShapeId createXLD(const HalconWidgetObject& img, const std::vector<HalconWidgetObject>& recs, bool& isCreate);
			static HalconShapeId createXLD(const HalconWidgetObject& img, const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& config);
			static HalconShapeId createXLD(const HalconWidgetObject* img, const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& config);
			static HalconShapeId createXLD(const HalconWidgetObject* img, const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& config, bool& isCreate);
		public:
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& id, const HalconWidgetObject& img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig, const PainterConfig& config);
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& id, const HalconWidgetObject* img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig, const PainterConfig& config);
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& id, const HalconWidgetObject* img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig);
			static std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& id, const HalconWidgetObject& img, const HalconShapeXLDFindConfig& halconShapeXldFindConfig);
		public:
			static void saveModel(const HalconShapeId& id, const std::string& filePath);
			static HalconShapeId readModel(const std::string& filePath);
		};
	}
}