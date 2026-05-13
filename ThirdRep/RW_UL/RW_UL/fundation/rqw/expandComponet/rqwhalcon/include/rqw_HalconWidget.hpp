#pragma once

#include"rqw_HalconUtilty.hpp"
#include"rqw_HalconWidgetDisObject.hpp"

#include <QWidget>

#include "rqw_HalconModelConfig.hpp"
#include"opencv2/opencv.hpp"

namespace rw {
	namespace rqw {
		class HalconWidget : public QWidget
		{
			Q_OBJECT
		public:
			explicit HalconWidget(QWidget* parent = nullptr);
			~HalconWidget() override;
		private:
			HalconCpp::HTuple* _halconWindowHandle{ nullptr };
		public:
			HalconCpp::HTuple* Handle();
		public:
			HalconWidgetDisObjectId appendHObject(const HalconWidgetObject& object);
			HalconWidgetDisObjectId appendHObject(HalconWidgetObject* object);
			HalconWidgetDisObjectId appendHObject(const HalconWidgetImg& object);
			HalconWidgetDisObjectId appendHObject(HalconWidgetImg* object);
			HalconWidgetDisObjectId appendHObject(const HalconWidgetTemplateResult& object);
			HalconWidgetDisObjectId appendHObject(HalconWidgetTemplateResult* object);
			void clearHObject();
		public:
			size_t width();
			size_t height();
		public:
			HalconWidgetObject* getObjectPtrById(HalconWidgetDisObjectId id);
			HalconWidgetObject getObjectById(HalconWidgetDisObjectId id);
			bool eraseObjectById(int id);
			bool eraseObjectsByType(HalconObjectType objectType);
		public:
			[[nodiscard]] std::vector<HalconWidgetDisObjectId> getAllIds() const;
			[[nodiscard]] std::vector<HalconWidgetDisObjectId> getIdsByType(HalconObjectType objectType) const;
			HalconWidgetDisObjectId getVailidAppendId();
		public:
			void updateWidget();
		public:
			HalconWidgetDisObjectId appendVerticalLine(int position, const PainterConfig& config = {});
			HalconWidgetDisObjectId appendHorizontalLine(int position, const PainterConfig& config = {});
		public:
			bool setObjectVisible(HalconWidgetDisObjectId id, const bool visible);
		public:
			bool isDrawing();
		protected:
			void showEvent(QShowEvent* event) override;
			void resizeEvent(QResizeEvent* event) override;
		protected:
			void wheelEvent(QWheelEvent* event) override;
		private:
			bool _isChange{ false };
			double _newCol1{ 0 };
			double _newRow1{ 0 };
			double _newCol2{ 0 };
			double _newRow2{ 0 };
		private:
			double _standardCol1{ 0 };
			double _standardRow1{ 0 };
			double _standardCol2{ 0 };
			double _standardRow2{ 0 };
		private:
			double _wheelSize{ 1 };
		private:
			void reset_display();
		protected:
			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;
			void mouseReleaseEvent(QMouseEvent* event) override;
		private:
			void display_HalconWidgetDisObject(HalconWidgetObject* object);
			void prepare_display(const PainterConfig& config);
		private:
			std::vector<HalconWidgetObject*> _halconObjects;
		private:
			void initialize_halconWindow();
		private:
			void refresh_allObject();
		private:
			bool _isDragging{ false };
			QPoint _lastMousePos;
		private:
			bool _isDrawingRect{ false };
		private:
			bool _isLearning{ false };
		public:
			bool isLearning();
		private:
			std::vector<HalconCpp::HTuple> _shapeModelIds;
			void clear_shapeModels();
		public:
			HalconWidgetObject drawRect();
			HalconWidgetObject drawRect(PainterConfig config);
			HalconWidgetObject drawRect(PainterConfig config, bool isShow);
			HalconWidgetObject drawRect(PainterConfig config, double minHeight, double minWidth);
			HalconWidgetObject drawRect(PainterConfig config, bool isShow, double minHeight, double minWidth);
			HalconWidgetObject drawRect(PainterConfig config, bool isShow, double minHeight, double minWidth, bool& isDraw);
		public:
			HalconShapeId createShapeXLDModel(const std::vector<HalconWidgetObject>& recs);
			HalconShapeId createShapeXLDModel(const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& halconShapeXLDConfig);
			HalconShapeId createShapeXLDModel(const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& halconShapeXLDConfig, bool& isCreate);
			std::vector<HalconWidgetTemplateResult> findShapeModel(const HalconShapeId& id, const HalconShapeXLDFindConfig& halconShapeXldFindConfig, const PainterConfig& painterConfig);
		};
	}
}