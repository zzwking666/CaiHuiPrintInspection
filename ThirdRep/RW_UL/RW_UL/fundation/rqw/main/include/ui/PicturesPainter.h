#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include <QLabel>
#include "PicturesPainterUtilty.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PicturesPainterClass; };
QT_END_NAMESPACE

// 绘画类
class DrawLabel : public QLabel
{
	Q_OBJECT
public:
	explicit DrawLabel(QWidget* parent = nullptr);
	QRectF getNormalizedRect() const;
	void setImage(const QImage& img); // 新增
	void updateScaledPixmap();
protected:
	void resizeEvent(QResizeEvent* event) override;
private:
	QImage m_srcImage; // 新增，保存原始图片
signals:
	void rectSelected(const QRectF& normalizedRect);
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
public slots:
	void setDrawingEnabled(bool enabled);
private:
	QPoint m_startPoint;
	QPoint m_endPoint;
	bool m_isDrawing = false;
private:
	bool m_drawingEnabled = false; // 是否允许绘画
};



class PicturesPainter : public QDialog
{
	Q_OBJECT

public:
	PicturesPainter(QWidget* parent = nullptr);
	~PicturesPainter();

	// 绘画信息结构体
public:
	void setRectangleConfigs(const std::vector<rw::rqw::RectangeConfig>& configs);
	void setDrawnRectangles(const std::vector<rw::rqw::PainterRectangleInfo>& Rectangles);
	void setImage(const QImage& qImage);
	void setAspectRatio(double width, double height);

	QColor getColorByClassId(size_t size);
	QString getNameByClassId(size_t size);
	std::vector<rw::rqw::PainterRectangleInfo> getRectangleConfigs();

private:
	void build_ui();
	void release_ui();
	void build_connect();
	void hide_ui();
private:
	void showWhiteImageOnLabel(double width, double height);

protected:
	void showEvent(QShowEvent* event) override;

private slots:
	void btn_set_clicked();
	void btn_draw_clicked();
	void pbtn_ok_clicked();
	void pbtn_exit_clicked();
	void pbtn_setAspectRatio_clicked();
	void pbtn_openPicture_clicked();
	void btn_clear_clicked();

	void onListViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous = QModelIndex());

	void onRectSelected(const QRectF& rect);
	void updateDrawLabel();

private:
	Ui::PicturesPainterClass* ui;
	double img_Width{ 0 };
	double img_Height{ 0 };

	std::vector<rw::rqw::RectangeConfig> _configs{};
	std::vector<rw::rqw::PainterRectangleInfo> _drawnRectangles{};

	QStandardItemModel* m_listModel = nullptr;   // 对应ListView
	QStandardItemModel* m_tableModel = nullptr;  // 对应TableView

	DrawLabel* drawLabel = nullptr;

	QRectF m_lastNormalizedRect; // 记录最后一次框选的标准化矩形
	bool m_hasSelectedRect = false; // 判断是否有有效区域

	bool isSetDrawnRectangles{ false };
	bool isSetAspectRatio{ false };
	bool isSetQImage{ false };

	QImage _qImage{ nullptr };

	bool m_isDrawing = false;
};
