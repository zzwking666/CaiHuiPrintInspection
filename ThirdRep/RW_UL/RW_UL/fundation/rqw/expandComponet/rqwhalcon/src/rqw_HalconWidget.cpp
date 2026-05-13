#include"rqw_HalconWidget.hpp"
#include <QResizeEvent>
#include <unordered_set>

#include "rqw_HalconModel.hpp"
#include "halconcpp/HalconCpp.h"

#include"rqw_HalconUtilty.hpp"

namespace rw {
	namespace rqw
	{
		HalconWidget::HalconWidget(QWidget* parent)
            : QWidget(parent)
        {
            initialize_halconWindow();
        }

        HalconWidget::~HalconWidget()
        {
            clear_shapeModels();
            clearHObject();
        }

        HalconCpp::HTuple* HalconWidget::Handle()
        {
            if (_halconWindowHandle == nullptr)
            {
                initialize_halconWindow();
            }
			return _halconWindowHandle;
        }

        HalconWidgetDisObjectId HalconWidget::appendHObject(const HalconWidgetObject& object)
        {
            HalconWidgetObject* newObject = new HalconWidgetObject(object);
            newObject->id = getVailidAppendId();
            _halconObjects.push_back(newObject);
            refresh_allObject();
			return newObject->id;
        }

        HalconWidgetDisObjectId HalconWidget::appendHObject(HalconWidgetObject* object)
        {
            if (object == nullptr)
            {
                return -1;
            }

            object->id = getVailidAppendId();

            _halconObjects.push_back(object);
			refresh_allObject();

            return object->id;
        }

        HalconWidgetDisObjectId HalconWidget::appendHObject(const HalconWidgetImg& object)
        {
            HalconWidgetImg* newObject = new HalconWidgetImg(object);
            newObject->id = getVailidAppendId();
            _halconObjects.push_back(newObject);
			refresh_allObject();
            return newObject->id;
        }

        HalconWidgetDisObjectId HalconWidget::appendHObject(HalconWidgetImg* object)
        {
            if (object == nullptr)
            {
                return -1;
            }
            object->id = getVailidAppendId();
			_halconObjects.push_back(object);
            return object->id;
        }

        HalconWidgetDisObjectId HalconWidget::appendHObject(const HalconWidgetTemplateResult& object)
        {
            HalconWidgetTemplateResult* newObject = new HalconWidgetTemplateResult(object);
            newObject->id = getVailidAppendId();
            _halconObjects.push_back(newObject);
            refresh_allObject();
            return newObject->id;
        }

        HalconWidgetDisObjectId HalconWidget::appendHObject(HalconWidgetTemplateResult* object)
        {
            if (object == nullptr)
            {
                return -1;
            }
            object->id = getVailidAppendId();
            _halconObjects.push_back(object);
            refresh_allObject();
			return object->id;
        }


        void HalconWidget::clearHObject()
        {
            _halconObjects.clear();
            if (_halconWindowHandle)
            {
                ClearWindow(*_halconWindowHandle);
			}
        }

        size_t HalconWidget::width()
        {
            if (!_halconWindowHandle)
            {
                return 0; // 如果窗口句柄未初始化，返回 0
            }

            HalconCpp::HTuple row1, col1, row2, col2;
            GetPart(*_halconWindowHandle, &row1, &col1, &row2, &col2);

            // 计算宽度
            return static_cast<size_t>(col2.D() - col1.D() + 1);
        }

        size_t HalconWidget::height()
        {
            if (!_halconWindowHandle)
            {
                return 0; // 如果窗口句柄未初始化，返回 0
            }

            HalconCpp::HTuple row1, col1, row2, col2;
            GetPart(*_halconWindowHandle, &row1, &col1, &row2, &col2);

            // 计算高度
            return static_cast<size_t>(row2.D() - row1.D() + 1);
        }

        HalconWidgetObject* HalconWidget::getObjectPtrById(int id)
        {
            for (auto& object : _halconObjects)
            {
                if (object->id == id)
                {
                    return object;
                }
            }
            return new HalconWidgetObject(nullptr);
        }

        HalconWidgetObject HalconWidget::getObjectById(int id)
        {
            for (auto& object : _halconObjects)
            {
                if (object->id == id)
                {
                    return HalconWidgetObject(*object);
                }
            }
            return HalconWidgetObject(nullptr);
        }

        bool HalconWidget::eraseObjectById(int id)
        {
            for (auto it = _halconObjects.begin(); it != _halconObjects.end(); ++it)
            {
                if ((*it)->id == id)
                {
                    delete *it; 
                    _halconObjects.erase(it); 
                    refresh_allObject(); 
                    return true; 
                }
            }
			return false; // 未找到对象
        }

        bool HalconWidget::eraseObjectsByType(HalconObjectType objectType)
        {
            auto ids=getIdsByType(objectType);
            if (ids.empty())
            {
                return false; // 没有找到指定类型的对象
			}
            for (auto it = _halconObjects.begin(); it != _halconObjects.end();)
            {
                if ((*it)->type == objectType)
                {
                    delete *it; // 释放对象内存
                    it = _halconObjects.erase(it); // 删除对象并更新迭代器
                }
                else
                {
                    ++it; // 继续迭代
                }
            }
            refresh_allObject(); // 刷新显示
			return true; // 成功删除指定类型的对象
        }

        std::vector<HalconWidgetDisObjectId> HalconWidget::getAllIds() const
        {
            std::vector<HalconWidgetDisObjectId> ids;
            for (const auto& object : _halconObjects)
            {
                ids.push_back(object->id);
            }
			return ids;
        }

        std::vector<HalconWidgetDisObjectId> HalconWidget::getIdsByType(const HalconObjectType objectType) const
        {
            std::vector<HalconWidgetDisObjectId> ids;
            for (const auto& object : _halconObjects)
            {
                if (object->type==objectType)
                {
                    ids.push_back(object->id);
                }
            }
            return ids;
        }

        HalconWidgetDisObjectId HalconWidget::getVailidAppendId()
        {
            std::unordered_set<HalconWidgetDisObjectId> existingIds;
            for (const auto& object : _halconObjects)
            {
                existingIds.insert(object->id);
            }

            HalconWidgetDisObjectId newId = 0;
            while (existingIds.find(newId) != existingIds.end())
            {
                ++newId;
            }

            return newId;
        }

        void HalconWidget::reset_display()
        {
			_isChange = false;
            _newCol1 = 0;
            _newRow1 = 0;
            _newCol2 = 0;
			_newRow2 = 0;
            refresh_allObject();
        }

        void HalconWidget::refresh_allObject()
        {
            if (!_halconWindowHandle)
            {
                return;
            }
            HalconCpp::SetSystem("flush_graphic", "false");
            // 清除窗口内容
            ClearWindow(*_halconWindowHandle);

            if (_halconObjects.empty())
            {
                return; // 如果没有对象需要显示，直接返回
            }

            // 获取窗口大小
            auto size = this->size();
            int windowWidth = size.width();
            int windowHeight = size.height();

            // 获取第一个对象的大小（假设所有对象的大小一致）
            auto ids=getIdsByType(HalconObjectType::Image);
            if (ids.empty())
            {
                return;
            }
            HalconCpp::HTuple width, height;
            GetImageSize(*getObjectPtrById(ids.front())->value(), &width, &height);

            // 计算图像和窗口的宽高比
            double imgAspectRatio = static_cast<double>(width.I()) / height.I();
            double windowAspectRatio = static_cast<double>(windowWidth) / windowHeight;

            // 设置 Halcon 窗口铺满整个 QWidget
            try
            {
                //SetWindowExtents(*_halconWindowHandle, 0, 0, windowWidth, windowHeight);
            }
            catch (HalconCpp::HException & e)
            {
                return;
            }
            

            // 计算显示区域
            int partWidth, partHeight, offsetX, offsetY;
            if (imgAspectRatio > windowAspectRatio)
            {
                // 图像更宽，以宽度为基准
                partWidth = width.I();
                partHeight = static_cast<int>(width.I() / windowAspectRatio);
                offsetX = 0;
                offsetY = (partHeight - height.I()) / 2;
            }
            else
            {
                // 图像更高，以高度为基准
                partHeight = height.I();
                partWidth = static_cast<int>(height.I() * windowAspectRatio);
                offsetX = (partWidth - width.I()) / 2;
                offsetY = 0;
            }
            if (!_isChange)
            {
                _standardCol1 = -offsetX;
                _standardCol2 = width.I() - 1 + offsetX;
                _standardRow1 = -offsetY;
                _standardRow2 = height.I() - 1 + offsetY;
                SetPart(*_halconWindowHandle, _standardRow1, _standardCol1, _standardRow2, _standardCol2);
            }
            else
            {
                SetPart(*_halconWindowHandle, _newRow1, _newCol1, _newRow2, _newCol2);
            }
            

            // 显示所有对象
            for (auto& object : _halconObjects)
            {
                if (object->isShow)
                {
                    display_HalconWidgetDisObject(object);
                }
            }
            HalconCpp::SetSystem("flush_graphic", "true");
            HalconCpp::WriteString(*_halconWindowHandle, "");

        }

        void HalconWidget::updateWidget()
        {
            refresh_allObject();
        }


        HalconWidgetDisObjectId HalconWidget::appendVerticalLine(int position, const PainterConfig& config)
        {
            if (!_halconWindowHandle)
            {
                return -1;
            }

          
            

            // 生成垂直线
            auto verticalLine = new HalconCpp::HObject;
            HalconCpp::GenRectangle1(verticalLine, 0, position- config.thickness / 2, 5000, position+ config.thickness / 2);

            // 设置颜色
            auto [r, g, b] = RQWColorToRGB(config.color);
            SetRgb(*_halconWindowHandle, r, g, b);

            // 创建并添加对象
            HalconWidgetObject object(verticalLine);
            object.isShow = true;
            object.painterConfig = config;
            object.type = HalconObjectType::Region;
            object.id = getVailidAppendId();
            appendHObject(object);
            return object.id;
        }

        HalconWidgetDisObjectId HalconWidget::appendHorizontalLine(int position, const PainterConfig& config)
        {
            if (!_halconWindowHandle)
            {
                return -1;
            }

            

           
            auto horizontalLine = new HalconCpp::HObject;
            HalconCpp::GenRectangle1(horizontalLine, position - config.thickness / 2, 0, position + config.thickness / 2, 5000);

            auto [r,g,b] = RQWColorToRGB(config.color);
            SetRgb(*_halconWindowHandle, r, g, b);

            HalconWidgetObject object(horizontalLine);
            object.isShow = true;
            object.painterConfig = config;
            object.type = HalconObjectType::Region;
            object.id = getVailidAppendId();
            appendHObject(object);
            return object.id;
        }

        bool HalconWidget::setObjectVisible(const HalconWidgetDisObjectId id, const bool visible)
        {
            auto object=getObjectPtrById(id);
            if (object == nullptr || !object->has_value())
            {
                return false; 
			}
            object->isShow = visible; 
            refresh_allObject(); 
			return true; 
        }

        bool HalconWidget::isDrawing()
        {
            return _isDrawingRect;
        }


        void HalconWidget::initialize_halconWindow()
        {
            if (_halconWindowHandle)
            {
                delete _halconWindowHandle;
            }
            _halconWindowHandle = new HalconCpp::HTuple();
            Hlong winId = this->winId();
            auto size = this->size();

            OpenWindow(0, 0, size.width(), size.height(), winId, "visible", "", _halconWindowHandle);
        }

        void HalconWidget::wheelEvent(QWheelEvent* event)
        {
            if (_isDrawingRect) {
                event->ignore(); 
                return;
            }

            if (rect().contains(event->position().toPoint())) { 
                int delta = event->angleDelta().y(); 
                double scaleFactor = (delta > 0) ? 1.1 : 0.9; 
                _wheelSize *= scaleFactor;
                if (_wheelSize<0.95)
                {
                    reset_display();
                    _wheelSize = 1;
                    return;
                }
                QPointF mousePos = event->position();
                int mouseX = static_cast<int>(mousePos.x());
                int mouseY = static_cast<int>(mousePos.y());

                HalconCpp::HTuple row1, col1, row2, col2;
                GetPart(*_halconWindowHandle, &row1, &col1, &row2, &col2);

                double currentWidth = col2.D() - col1.D() + 1;
                double currentHeight = row2.D() - row1.D() + 1;

                double relativeX = col1.D() + (mouseX / static_cast<double>(width())) * currentWidth;
                double relativeY = row1.D() + (mouseY / static_cast<double>(height())) * currentHeight;

                double newWidth = currentWidth / scaleFactor;
                double newHeight = currentHeight / scaleFactor;
                _newCol1 = relativeX - (mouseX / static_cast<double>(width())) * newWidth;
                _newRow1 = relativeY - (mouseY / static_cast<double>(height())) * newHeight;
                _newCol2 = _newCol1 + newWidth - 1;
                _newRow2 = _newRow1 + newHeight - 1;
                _isChange = true;
                refresh_allObject();
            }
            else {
                event->ignore(); 
            }
        }

        void HalconWidget::showEvent(QShowEvent* event)
        {
            initialize_halconWindow();
            refresh_allObject();
        }

        void HalconWidget::resizeEvent(QResizeEvent* event)
        {
            // 定义最小宽度和高度
            const int minWidth = 100;  // 最小宽度
            const int minHeight = 100; // 最小高度

            // 获取当前窗口大小
            QSize newSize = event->size();

            // 检查是否小于最小尺寸
            if (newSize.width() < minWidth || newSize.height() < minHeight)
            {
                // 调整窗口大小到最小值
                resize((std::max)(newSize.width(), minWidth), (std::max)(newSize.height(), minHeight));
                return;
            }

            // 刷新所有对象
            refresh_allObject();
        }

        void HalconWidget::mousePressEvent(QMouseEvent* event)
        {
            if (_isDrawingRect) {
                event->ignore(); // 如果正在绘制矩形，忽略鼠标事件
                return;
            }

            if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
                _isDragging = true;
                _lastMousePos = event->pos(); // 记录鼠标按下时的位置
                event->accept();
            }
            else {
                event->ignore();
            }
        }

        void HalconWidget::mouseMoveEvent(QMouseEvent* event)
        {
            if (_isDrawingRect) {
                event->ignore(); // 如果正在绘制矩形，忽略鼠标事件
                return;
            }

            if (_isDragging && _halconWindowHandle && !_halconObjects.empty()) {
                QPoint currentMousePos = event->pos();
                QPoint delta = currentMousePos - _lastMousePos; // 计算鼠标移动的偏移量

                // 获取当前显示区域
                HalconCpp::HTuple row1, col1, row2, col2;
                GetPart(*_halconWindowHandle, &row1, &col1, &row2, &col2);

                // 根据鼠标移动的偏移量调整显示区域
                double deltaX = -delta.x() * (col2.D() - col1.D() + 1) / width();
                double deltaY = -delta.y() * (row2.D() - row1.D() + 1) / height();

                _newCol1 = col1.D() + deltaX;
                _newCol2 = col2.D() + deltaX;
                _newRow1 = row1.D() + deltaY;
                _newRow2 = row2.D() + deltaY;


                _isChange = true;
                refresh_allObject();

                _lastMousePos = currentMousePos; // 更新鼠标位置
                event->accept();
            }
            else {
                event->ignore();
            }
        }

        void HalconWidget::mouseReleaseEvent(QMouseEvent* event)
        {
            if (_isDrawingRect) {
                event->ignore(); // 如果正在绘制矩形，忽略鼠标事件
                return;
            }

            if (event->button() == Qt::LeftButton) {
                _isDragging = false; // 停止拖拽
                event->accept();
            }
            else {
                event->ignore();
            }
        }

        void HalconWidget::display_HalconWidgetDisObject(HalconWidgetObject* object)
        {
            if (object == nullptr || !object->has_value())
            {
                return; // 如果对象无效，直接返回
            }
            HalconCpp::HObject* halconObject = object->value();
            if (halconObject == nullptr || !halconObject->IsInitialized())
            {
                return; 
            }
            prepare_display(object->painterConfig);
            HalconCpp::HTuple hv_WindowHandle = *_halconWindowHandle;
			HalconCpp::DispObj(*halconObject, hv_WindowHandle);
        }

        void HalconWidget::prepare_display(const PainterConfig& config)
        {
            auto [r, g, b] = RQWColorToRGB(config.color);
            SetRgb(*_halconWindowHandle, r, g, b);
        }

        HalconWidgetObject HalconWidget::drawRect(PainterConfig config)
        {
            bool isDraw{false};
            return drawRect(config,true, 0, 0, isDraw);
        }

        HalconWidgetObject HalconWidget::drawRect(PainterConfig config, bool isShow)
        {
            bool isDraw{false};
			return drawRect(config, isShow, 0, 0, isDraw);
        }

        HalconWidgetObject HalconWidget::drawRect(PainterConfig config, bool isShow, double minHeight,
	        double minWidth)
        {
            bool isDraw{ false };
            return drawRect(config, isShow, minHeight, minWidth, isDraw);
        }

        HalconWidgetObject HalconWidget::drawRect(PainterConfig config, bool isShow, double minHeight, double minWidth, bool& isDraw)
        {
            prepare_display(config);
            _isDrawingRect = true;
            HalconCpp::HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2;

            auto ho_Rectangle = new HalconCpp::HObject;

            // 绘制矩形
            HalconCpp::DrawRectangle1(*_halconWindowHandle, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
            HalconCpp::GenRectangle1(ho_Rectangle, hv_Row1, hv_Column1, hv_Row2, hv_Column2);

            auto height = hv_Row2.D() - hv_Row1.D();
            auto width = hv_Column2.D() - hv_Column1.D();

            if (height < minHeight || width < minWidth)
            {
                isDraw = false;
                return HalconWidgetObject(nullptr);
            }

            auto resultObject = new HalconWidgetObject(ho_Rectangle); // 使用边框对象
            resultObject->id = getVailidAppendId();
            resultObject->painterConfig = config;
            resultObject->isShow = isShow;
            resultObject->type = HalconObjectType::Region;
            resultObject->descrption = "drawRect";

            // 将矩形转换为轮廓（边框）
            auto ho_Contour = new HalconCpp::HObject;
            HalconCpp::GenContourRegionXld(*ho_Rectangle, ho_Contour, "border");
            // 创建 HalconWidgetDisObject 对象
            auto object = new HalconWidgetObject(ho_Contour); // 使用轮廓对象
            object->id = getVailidAppendId();
            object->painterConfig = config;
            object->isShow = isShow;
            object->type = HalconObjectType::Region;
            object->descrption = "drawRect";
            appendHObject(object);
            isDraw = true;

            _isDrawingRect = false;

            return *resultObject;
        }

        HalconShapeId HalconWidget::createShapeXLDModel(const std::vector<HalconWidgetObject>& recs)
        {
            bool isCreate = false;
            HalconShapeId id = createShapeXLDModel(recs, HalconShapeXLDConfig(), isCreate);
            if (!isCreate)
            {
                throw std::runtime_error("Failed to create shape XLD model.");
            }
			return id;
        }

        HalconShapeId HalconWidget::createShapeXLDModel(const std::vector<HalconWidgetObject>& recs,
                                                        const HalconShapeXLDConfig& halconShapeXLDConfig)
        {
            bool isCreate = false;
            HalconShapeId id = createShapeXLDModel(recs, halconShapeXLDConfig, isCreate);
            if (!isCreate)
            {
                throw std::runtime_error("Failed to create shape XLD model.");
            }
            return id;
        }

        bool HalconWidget::isLearning()
        {
			return _isLearning;
        }

        void HalconWidget::clear_shapeModels()
        {
            for (const auto& id : _shapeModelIds)
            {
                HalconCpp::ClearShapeModel(id); 
            }
            _shapeModelIds.clear();
        }

        HalconWidgetObject HalconWidget::drawRect()
        {
            PainterConfig config;
            config.color = RQWColor::Red; 
            config.thickness = 2; 
            bool isDraw = false;
			return drawRect(config, true, 0, 0, isDraw);
        }

        HalconWidgetObject HalconWidget::drawRect(PainterConfig config, double minHeight, double minWidth)
        {
            bool isDraw = false;
			return drawRect(config, true, minHeight, minWidth, isDraw);
        }

        HalconShapeId HalconWidget::createShapeXLDModel(const std::vector<HalconWidgetObject>& recs, const HalconShapeXLDConfig& halconShapeXLDConfig, bool& isCreate)
        {
            auto ids=getIdsByType(HalconObjectType::Image);
            if (ids.empty())
            {
				throw std::runtime_error("No image object available for creating shape model.");
            }
			auto id = HalconShapeXLDModel::createXLD(getObjectPtrById(ids.front()), recs, halconShapeXLDConfig, isCreate);
            _shapeModelIds.push_back(id);
            return id;
        }

        std::vector<HalconWidgetTemplateResult> HalconWidget::findShapeModel(const HalconShapeId& id, const HalconShapeXLDFindConfig& halconShapeXldFindConfig, const PainterConfig& painterConfig)
        {
            _isLearning = true;
            if (_halconObjects.empty())
            {
                return  std::vector<HalconWidgetTemplateResult>();
            }

            auto imgs = getIdsByType(HalconObjectType::Image);
            if (imgs.empty())
            {
                return std::vector<HalconWidgetTemplateResult>();
            }


            auto results = HalconShapeXLDModel::findShapeModel(id, getObjectPtrById(imgs.front()), halconShapeXldFindConfig,painterConfig);

            for (auto& result : results)
            {
                result.id = getVailidAppendId();
                appendHObject(result);
            }
            _isLearning = false;
            return results;
        }
	}
}