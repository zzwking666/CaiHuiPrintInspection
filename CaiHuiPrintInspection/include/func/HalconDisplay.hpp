#pragma once

#include <QObject>
#include <QPoint>
#include <QWidget>
#include <QString>
#include <opencv2/opencv.hpp>

// Halcon forward declarations
namespace HalconCpp {
class HTuple;
class HObject;
}

namespace rw {
namespace rqw {

/**
 * @brief Halcon 显示封装类
 * 
 * 封装 Halcon 窗口的创建、管理和图片显示功能
 * 在构造函数中传入 QWidget 作为父窗口，自动创建 Halcon 窗口
 */
class HalconDisplay : public QObject
{
public:
    /**
     * @brief 构造函数
     * @param parentWidget 父控件，Halcon 窗口将嵌入到此控件中
     */
    explicit HalconDisplay(QWidget* parentWidget);
    
    /**
     * @brief 析构函数
     */
    ~HalconDisplay();

    /**
     * @brief 禁止拷贝
     */
    HalconDisplay(const HalconDisplay&) = delete;
    HalconDisplay& operator=(const HalconDisplay&) = delete;

    /**
     * @brief 允许移动
     */
    HalconDisplay(HalconDisplay&& other) noexcept;
    HalconDisplay& operator=(HalconDisplay&& other) noexcept;

    /**
     * @brief 初始化 Halcon 窗口
     * @return 是否初始化成功
     */
    bool initialize();

    /**
     * @brief 关闭 Halcon 窗口
     */
    void closeWindow();

    /**
     * @brief 检查窗口是否有效
     * @return 窗口是否有效
     */
    bool isValid() const;

    /**
     * @brief 显示图片
     * @param image Halcon 图片对象
     * @param fitToWindow 是否自适应窗口大小
     * @return 是否显示成功
     */
    bool displayImage(const HalconCpp::HObject& image, bool fitToWindow = true);

    /**
     * @brief 从文件读取并显示图片
     * @param imagePath 图片路径
     * @param fitToWindow 是否自适应窗口大小
     * @return 是否显示成功
     */
    bool displayImageFromFile(const QString& imagePath, bool fitToWindow = true);

    /**
     * @brief 清空显示
     */
    void clearDisplay();

    /**
     * @brief 设置显示区域（用于缩放和平移）
     * @param row1 左上角行坐标
     * @param col1 左上角列坐标
     * @param row2 右下角行坐标
     * @param col2 右下角列坐标
     */
    void setDisplayPart(int row1, int col1, int row2, int col2);

    /**
     * @brief 重置显示区域为全图
     * @param imageWidth 图片宽度
     * @param imageHeight 图片高度
     */
    void resetDisplayPart(int imageWidth, int imageHeight);

    /**
     * @brief 获取窗口句柄
     * @return Halcon 窗口句柄指针
     */
    HalconCpp::HTuple* getWindowHandle() const { return _windowHandle; }

    /**
     * @brief 获取父控件
     * @return 父控件指针
     */
    QWidget* getParentWidget() const { return _parentWidget; }

    /**
     * @brief 获取最后显示的图片
     * @return 最后显示的图片指针（可能为 nullptr）
     */
    HalconCpp::HObject* getLastImage() const { return _lastImage; }

    /**
     * @brief 将 OpenCV Mat 转换为 Halcon HObject
     * @param mat OpenCV Mat 图像
     * @return Halcon HObject 图像
     * @note 支持 CV_8UC1(灰度), CV_8UC3(BGR), CV_16UC1(16位灰度) 格式
     */
    static HalconCpp::HObject matToHObject(const cv::Mat& mat);

    /**
     * @brief 显示 OpenCV Mat 图片
     * @param mat OpenCV Mat 图像
     * @param fitToWindow 是否自适应窗口大小
     * @return 是否显示成功
     */
    bool displayMat(const cv::Mat& mat, bool fitToWindow = true);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void refreshDisplay();
    void clampAndApplyPart(double row1, double col1, double row2, double col2);

private:
    QWidget* _parentWidget = nullptr;           ///< 父控件
    HalconCpp::HTuple* _windowHandle = nullptr; ///< Halcon 窗口句柄
    HalconCpp::HObject* _lastImage = nullptr;   ///< 最后显示的图片
    bool _isInitialized = false;                ///< 是否已初始化

    int _imageWidth = 0;
    int _imageHeight = 0;

    double _partRow1 = 0.0;
    double _partCol1 = 0.0;
    double _partRow2 = 0.0;
    double _partCol2 = 0.0;

    double _maxPartWidth = 0.0;
    double _maxPartHeight = 0.0;

    bool _isPanning = false;
    QPoint _lastMousePos;
};

} // namespace rqw
} // namespace rw
