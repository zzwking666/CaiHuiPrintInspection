#include "HalconDisplay.hpp"

#include <algorithm>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QString>
#include <QWheelEvent>
#include <halconcpp/HalconCpp.h>
#include <opencv2/opencv.hpp>

namespace rw {
namespace rqw {

HalconDisplay::HalconDisplay(QWidget* parentWidget)
    : QObject(parentWidget)
    , _parentWidget(parentWidget)
{
}

HalconDisplay::~HalconDisplay()
{
    closeWindow();
}

HalconDisplay::HalconDisplay(HalconDisplay&& other) noexcept
    : QObject(other.parent())
    , _parentWidget(other._parentWidget)
    , _windowHandle(other._windowHandle)
    , _lastImage(other._lastImage)
    , _isInitialized(other._isInitialized)
    , _imageWidth(other._imageWidth)
    , _imageHeight(other._imageHeight)
    , _partRow1(other._partRow1)
    , _partCol1(other._partCol1)
    , _partRow2(other._partRow2)
    , _partCol2(other._partCol2)
    , _maxPartWidth(other._maxPartWidth)
    , _maxPartHeight(other._maxPartHeight)
    , _isPanning(other._isPanning)
    , _lastMousePos(other._lastMousePos)
{
    if (_parentWidget) {
        _parentWidget->removeEventFilter(&other);
        _parentWidget->installEventFilter(this);
    }

    other._parentWidget = nullptr;
    other._windowHandle = nullptr;
    other._lastImage = nullptr;
    other._isInitialized = false;
    other._imageWidth = 0;
    other._imageHeight = 0;
    other._partRow1 = 0.0;
    other._partCol1 = 0.0;
    other._partRow2 = 0.0;
    other._partCol2 = 0.0;
    other._maxPartWidth = 0.0;
    other._maxPartHeight = 0.0;
    other._isPanning = false;
}

HalconDisplay& HalconDisplay::operator=(HalconDisplay&& other) noexcept
{
    if (this != &other) {
        closeWindow();
        
        _parentWidget = other._parentWidget;
        _windowHandle = other._windowHandle;
        _lastImage = other._lastImage;
        _isInitialized = other._isInitialized;
        _imageWidth = other._imageWidth;
        _imageHeight = other._imageHeight;
        _partRow1 = other._partRow1;
        _partCol1 = other._partCol1;
        _partRow2 = other._partRow2;
        _partCol2 = other._partCol2;
        _maxPartWidth = other._maxPartWidth;
        _maxPartHeight = other._maxPartHeight;
        _isPanning = other._isPanning;
        _lastMousePos = other._lastMousePos;

        if (_parentWidget) {
            _parentWidget->removeEventFilter(&other);
            _parentWidget->installEventFilter(this);
        }
        
        other._parentWidget = nullptr;
        other._windowHandle = nullptr;
        other._lastImage = nullptr;
        other._isInitialized = false;
        other._imageWidth = 0;
        other._imageHeight = 0;
        other._partRow1 = 0.0;
        other._partCol1 = 0.0;
        other._partRow2 = 0.0;
        other._partCol2 = 0.0;
        other._maxPartWidth = 0.0;
        other._maxPartHeight = 0.0;
        other._isPanning = false;
    }
    return *this;
}

bool HalconDisplay::initialize()
{
    if (!_parentWidget) {
        qWarning() << "HalconDisplay: 父控件为空，无法初始化";
        return false;
    }

    // 如果已经初始化，先关闭
    if (_isInitialized) {
        closeWindow();
    }

    try {
        using namespace HalconCpp;

        // 获取父控件的窗口句柄
        HWND hwnd = reinterpret_cast<HWND>(_parentWidget->winId());
        
        // 创建 Halcon 窗口句柄
        if (!_windowHandle) {
            _windowHandle = new HTuple();
        }

        // 创建 Halcon 窗口
        HTuple hvFatherWindow(reinterpret_cast<Hlong>(hwnd));
        OpenWindow(0, 0, 
            _parentWidget->width(), 
            _parentWidget->height(), 
            hvFatherWindow, "visible", "", _windowHandle);

        _isInitialized = true;
        _isPanning = false;
        if (_parentWidget) {
            _parentWidget->installEventFilter(this);
            _parentWidget->setMouseTracking(true);
        }
        qDebug() << "HalconDisplay: 窗口初始化成功，大小:" 
                 << _parentWidget->width() << "x" << _parentWidget->height();
        return true;
    }
    catch (const HalconCpp::HException& e) {
        qWarning() << "HalconDisplay 初始化错误:" << e.ErrorMessage().Text();
        return false;
    }
    catch (...) {
        qWarning() << "HalconDisplay: 初始化时发生未知错误";
        return false;
    }
}

void HalconDisplay::closeWindow()
{
    if (_parentWidget) {
        _parentWidget->removeEventFilter(this);
    }

    try {
        if (_windowHandle && _windowHandle->TupleLength() > 0) {
            HalconCpp::CloseWindow(*_windowHandle);
        }
    }
    catch (...) {
        // 忽略关闭窗口时的错误
    }

    delete _windowHandle;
    _windowHandle = nullptr;

    delete _lastImage;
    _lastImage = nullptr;

    _isInitialized = false;
    _isPanning = false;
    _imageWidth = 0;
    _imageHeight = 0;
    _partRow1 = 0.0;
    _partCol1 = 0.0;
    _partRow2 = 0.0;
    _partCol2 = 0.0;
    _maxPartWidth = 0.0;
    _maxPartHeight = 0.0;
}

bool HalconDisplay::isValid() const
{
    return _isInitialized && _windowHandle && _windowHandle->TupleLength() > 0;
}

bool HalconDisplay::displayImage(const HalconCpp::HObject& image, bool fitToWindow)
{
    if (!isValid()) {
        qWarning() << "HalconDisplay: 窗口未初始化，无法显示图片";
        return false;
    }

    try {
        using namespace HalconCpp;

        // 同步 Halcon 子窗口大小到父控件，避免显示区域过小
        if (_parentWidget) {
            const QRect rect = _parentWidget->contentsRect();
            const int width = rect.width() > 1 ? rect.width() : 1;
            const int height = rect.height() > 1 ? rect.height() : 1;
            SetWindowExtents(*_windowHandle, 0, 0, width, height);
        }

        // 获取图片尺寸
        HTuple hvWidth, hvHeight;
        GetImageSize(image, &hvWidth, &hvHeight);

        _imageWidth = hvWidth[0].I();
        _imageHeight = hvHeight[0].I();

        if (fitToWindow && _parentWidget) {
            const QRect rect = _parentWidget->contentsRect();
            const double winWidth = rect.width() > 1 ? static_cast<double>(rect.width()) : 1.0;
            const double winHeight = rect.height() > 1 ? static_cast<double>(rect.height()) : 1.0;
            const double imgWidth = static_cast<double>(hvWidth[0].I());
            const double imgHeight = static_cast<double>(hvHeight[0].I());

            // 保持图像纵横比，不拉伸失真（必要时留黑边）
            const double winAspect = winWidth / winHeight;
            const double imgAspect = imgWidth / imgHeight;

            if (winAspect > imgAspect) {
                const double displayWidth = imgHeight * winAspect;
                const double colPadding = (displayWidth - imgWidth) * 0.5;
                _partRow1 = 0.0;
                _partCol1 = -colPadding;
                _partRow2 = imgHeight - 1.0;
                _partCol2 = imgWidth - 1.0 + colPadding;
            }
            else {
                const double displayHeight = imgWidth / winAspect;
                const double rowPadding = (displayHeight - imgHeight) * 0.5;
                _partRow1 = -rowPadding;
                _partCol1 = 0.0;
                _partRow2 = imgHeight - 1.0 + rowPadding;
                _partCol2 = imgWidth - 1.0;
            }

            _maxPartWidth = _partCol2 - _partCol1;
            _maxPartHeight = _partRow2 - _partRow1;
        }
        else {
            _partRow1 = 0.0;
            _partCol1 = 0.0;
            _partRow2 = static_cast<double>(hvHeight[0].I()) - 1.0;
            _partCol2 = static_cast<double>(hvWidth[0].I()) - 1.0;

            _maxPartWidth = _partCol2 - _partCol1;
            _maxPartHeight = _partRow2 - _partRow1;
        }

        SetPart(*_windowHandle, _partRow1, _partCol1, _partRow2, _partCol2);
        DispObj(image, *_windowHandle);

        // 保存图片引用
        if (_lastImage) {
            delete _lastImage;
        }
        _lastImage = new HObject(image);

        return true;
    }
    catch (const HalconCpp::HException& e) {
        qWarning() << "HalconDisplay 显示错误:" << e.ErrorMessage().Text();
        return false;
    }
    catch (...) {
        qWarning() << "HalconDisplay: 显示图片时发生未知错误";
        return false;
    }
}

bool HalconDisplay::displayImageFromFile(const QString& imagePath, bool fitToWindow)
{
    try {
        using namespace HalconCpp;

        // 读取图片
        HObject hoImage;
        ReadImage(&hoImage, imagePath.toStdString().c_str());

        // 显示图片
        return displayImage(hoImage, fitToWindow);
    }
    catch (const HalconCpp::HException& e) {
        qWarning() << "HalconDisplay 读取图片错误:" << e.ErrorMessage().Text();
        return false;
    }
    catch (...) {
        qWarning() << "HalconDisplay: 读取图片时发生未知错误";
        return false;
    }
}

void HalconDisplay::clearDisplay()
{
    if (!isValid()) {
        return;
    }

    try {
        HalconCpp::ClearWindow(*_windowHandle);
    }
    catch (...) {
        // 忽略清空时的错误
    }
}

void HalconDisplay::setDisplayPart(int row1, int col1, int row2, int col2)
{
    if (!isValid()) {
        return;
    }

    clampAndApplyPart(static_cast<double>(row1), static_cast<double>(col1),
        static_cast<double>(row2), static_cast<double>(col2));
}

void HalconDisplay::resetDisplayPart(int imageWidth, int imageHeight)
{
    setDisplayPart(0, 0, imageHeight - 1, imageWidth - 1);
}

bool HalconDisplay::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != _parentWidget || !_parentWidget) {
        return QObject::eventFilter(watched, event);
    }

    if (!_interactionEnabled) {
        return QObject::eventFilter(watched, event);
    }

    if (!isValid() || !_lastImage) {
        return QObject::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::Wheel: {
        auto* wheelEvent = static_cast<QWheelEvent*>(event);

        const QPoint angleDelta = wheelEvent->angleDelta();
        if (angleDelta.y() == 0) {
            return true;
        }

        const double zoomFactor = (angleDelta.y() > 0) ? 0.9 : 1.1;
        const QRect rect = _parentWidget->contentsRect();
        if (rect.width() <= 0 || rect.height() <= 0) {
            return true;
        }

        const QPointF localPos = wheelEvent->position();
        const double ratioX = std::clamp(localPos.x() / static_cast<double>(rect.width()), 0.0, 1.0);
        const double ratioY = std::clamp(localPos.y() / static_cast<double>(rect.height()), 0.0, 1.0);

        const double currentWidth = _partCol2 - _partCol1;
        const double currentHeight = _partRow2 - _partRow1;
        const double centerX = _partCol1 + ratioX * currentWidth;
        const double centerY = _partRow1 + ratioY * currentHeight;

        const double newWidth = currentWidth * zoomFactor;
        const double newHeight = currentHeight * zoomFactor;

        const double newCol1 = centerX - ratioX * newWidth;
        const double newCol2 = newCol1 + newWidth;
        const double newRow1 = centerY - ratioY * newHeight;
        const double newRow2 = newRow1 + newHeight;

        clampAndApplyPart(newRow1, newCol1, newRow2, newCol2);
        return true;
    }
    case QEvent::MouseButtonPress: {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            _isPanning = true;
            _lastMousePos = mouseEvent->pos();
            return true;
        }
        break;
    }
    case QEvent::MouseMove: {
        if (!_isPanning) {
            break;
        }

        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        const QPoint pos = mouseEvent->pos();
        const QPoint delta = pos - _lastMousePos;
        _lastMousePos = pos;

        const QRect rect = _parentWidget->contentsRect();
        if (rect.width() <= 0 || rect.height() <= 0) {
            return true;
        }

        const double currentWidth = _partCol2 - _partCol1;
        const double currentHeight = _partRow2 - _partRow1;

        const double shiftX = -static_cast<double>(delta.x()) * (currentWidth / static_cast<double>(rect.width()));
        const double shiftY = -static_cast<double>(delta.y()) * (currentHeight / static_cast<double>(rect.height()));

        clearDisplay();
        clampAndApplyPart(_partRow1 + shiftY, _partCol1 + shiftX, _partRow2 + shiftY, _partCol2 + shiftX);
        return true;
    }
    case QEvent::MouseButtonRelease: {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            _isPanning = false;
            return true;
        }
        break;
    }
    case QEvent::Resize: {
        refreshDisplay();
        break;
    }
    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}

void HalconDisplay::refreshDisplay()
{
    if (!isValid() || !_lastImage) {
        return;
    }

    try {
        if (_parentWidget) {
            const QRect rect = _parentWidget->contentsRect();
            const int width = rect.width() > 1 ? rect.width() : 1;
            const int height = rect.height() > 1 ? rect.height() : 1;
            HalconCpp::SetWindowExtents(*_windowHandle, 0, 0, width, height);
        }

        HalconCpp::SetPart(*_windowHandle, _partRow1, _partCol1, _partRow2, _partCol2);
        HalconCpp::DispObj(*_lastImage, *_windowHandle);
        if (_overlayDrawer) {
            _overlayDrawer(_windowHandle);
        }
    }
    catch (...) {
    }
}

void HalconDisplay::clampAndApplyPart(double row1, double col1, double row2, double col2)
{
    if (!isValid() || !_lastImage || _imageWidth <= 0 || _imageHeight <= 0) {
        return;
    }

    double width = col2 - col1;
    double height = row2 - row1;

    if (width <= 1.0 || height <= 1.0) {
        return;
    }

    const double minWidth = 1.0;
    const double minHeight = 1.0;

    if (width < minWidth) {
        const double center = (col1 + col2) * 0.5;
        width = minWidth;
        col1 = center - width * 0.5;
        col2 = center + width * 0.5;
    }

    if (height < minHeight) {
        const double center = (row1 + row2) * 0.5;
        height = minHeight;
        row1 = center - height * 0.5;
        row2 = center + height * 0.5;
    }

    if (_maxPartWidth > 0.0 && width > _maxPartWidth) {
        const double center = (col1 + col2) * 0.5;
        width = _maxPartWidth;
        col1 = center - width * 0.5;
        col2 = center + width * 0.5;
    }

    if (_maxPartHeight > 0.0 && height > _maxPartHeight) {
        const double center = (row1 + row2) * 0.5;
        height = _maxPartHeight;
        row1 = center - height * 0.5;
        row2 = center + height * 0.5;
    }

    _partRow1 = row1;
    _partCol1 = col1;
    _partRow2 = row2;
    _partCol2 = col2;

    refreshDisplay();
}

HalconCpp::HObject HalconDisplay::matToHObject(const cv::Mat& mat)
{
    using namespace HalconCpp;
    
    if (mat.empty()) {
        throw std::runtime_error("matToHObject: 输入 Mat 为空");
    }

    HObject hoImage;
    
    // 获取 Mat 的尺寸
    int width = mat.cols;
    int height = mat.rows;
    
    // 根据 Mat 类型进行转换
    switch (mat.type()) {
        case CV_8UC1: {
            // 8位灰度图
            GenImage1(&hoImage, "byte", width, height, (Hlong)mat.data);
            break;
        }
        case CV_8UC3: {
            // 8位彩色图 (BGR -> RGB)
            // Halcon 使用 RGB 格式，OpenCV 使用 BGR 格式，需要转换
            cv::Mat matRGB;
            cv::cvtColor(mat, matRGB, cv::COLOR_BGR2RGB);
            // 使用 GenImage3 分别传入 R, G, B 通道
            std::vector<cv::Mat> channels;
            cv::split(matRGB, channels);
            GenImage3(&hoImage, "byte", width, height, 
                     (Hlong)channels[0].data,  // R
                     (Hlong)channels[1].data,  // G
                     (Hlong)channels[2].data); // B
            break;
        }
        case CV_16UC1: {
            // 16位灰度图
            GenImage1(&hoImage, "uint2", width, height, (Hlong)mat.data);
            break;
        }
        case CV_32FC1: {
            // 32位浮点灰度图
            GenImage1(&hoImage, "real", width, height, (Hlong)mat.data);
            break;
        }
        default: {
            // 不支持的格式，尝试转换为 8UC3 后处理
            qWarning() << "matToHObject: 不支持的 Mat 类型:" << mat.type() << "，尝试转换为 8UC3";
            cv::Mat matConverted;
            if (mat.channels() == 1) {
                cv::cvtColor(mat, matConverted, cv::COLOR_GRAY2RGB);
            } else if (mat.channels() == 3) {
                cv::cvtColor(mat, matConverted, cv::COLOR_BGR2RGB);
            } else if (mat.channels() == 4) {
                cv::cvtColor(mat, matConverted, cv::COLOR_BGRA2RGB);
            } else {
                throw std::runtime_error("matToHObject: 不支持的通道数");
            }
            // 使用 GenImage3 分别传入 R, G, B 通道
            std::vector<cv::Mat> channels;
            cv::split(matConverted, channels);
            GenImage3(&hoImage, "byte", width, height, 
                     (Hlong)channels[0].data,  // R
                     (Hlong)channels[1].data,  // G
                     (Hlong)channels[2].data); // B
            break;
        }
    }
    
    return hoImage;
}

bool HalconDisplay::displayMat(const cv::Mat& mat, bool fitToWindow)
{
    if (!isValid()) {
        qWarning() << "HalconDisplay: 窗口未初始化，无法显示图片";
        return false;
    }

    try {
        using namespace HalconCpp;
        
        // 转换 Mat 为 HObject
        HObject hoImage = matToHObject(mat);
        
        // 显示图片
        return displayImage(hoImage, fitToWindow);
    }
    catch (const std::exception& e) {
        qWarning() << "HalconDisplay Mat 显示错误:" << e.what();
        return false;
    }
    catch (const HalconCpp::HException& e) {
        qWarning() << "HalconDisplay Mat 显示错误:" << e.ErrorMessage().Text();
        return false;
    }
    catch (...) {
        qWarning() << "HalconDisplay: 显示 Mat 时发生未知错误";
        return false;
    }
}

} // namespace rqw
} // namespace rw

