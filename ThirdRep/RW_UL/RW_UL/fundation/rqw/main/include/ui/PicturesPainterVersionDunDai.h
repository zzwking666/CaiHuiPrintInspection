#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include <QLabel>
#include "PicturesPainter.h"
#include "PicturesPainterUtilty.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PicturesPainterVersionDunDaiClass; };
QT_END_NAMESPACE

class PicturesPainterVersionDunDai : public QDialog
{
	Q_OBJECT

public:
	PicturesPainterVersionDunDai(QWidget* parent = nullptr);
	~PicturesPainterVersionDunDai();

public:
	void setRectangleConfigs(const std::vector<rw::rqw::RectangeConfig>& configs);
	void setDrawnRectangles(const std::vector<rw::rqw::PainterRectangleInfo>& Rectangles);
	void setSmartRectangles(const std::vector<rw::rqw::PainterRectangleInfo>& Rectangles);
	void setImage(const QImage& qImage);

	QColor getColorByClassId(size_t size);
	QString getNameByClassId(size_t size);
	std::vector<rw::rqw::PainterRectangleInfo> getRectangleConfigs();
	std::vector<rw::rqw::PainterRectangleInfo> getSmartRectangleConfigs();

	void setDisSmartRectangleForFirstShow(bool isShow) { isDisSmartRectangleForFirstShow = isShow; }
	bool isDisSmartRectangleForFirstShow{ false };
private:
	void build_ui();
	void release_ui();
	void build_connect();

protected:
	void showEvent(QShowEvent* event) override;

private slots:
	void pbtn_ok_clicked();
	void btn_close_clicked();
	void btn_clear_clicked();
	void btn_zhinengpingbi_clicked();

	void onRectSelected(const QRectF& rect);
	void updateDrawLabel();

	void onListViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous = QModelIndex());
	void onListViewItemClicked(const QModelIndex& index);

public:
	Ui::PicturesPainterVersionDunDaiClass* ui;
private:
	double img_Width{ 0 };
	double img_Height{ 0 };

	std::vector<rw::rqw::RectangeConfig> _configs{};
	std::vector<rw::rqw::PainterRectangleInfo> _drawnRectangles{};
	bool isGenerateSmartRectangles{ false };
	std::vector<rw::rqw::PainterRectangleInfo> _smartRectangles{};

	QStandardItemModel* m_listModel = nullptr;

	DrawLabel* drawLabel = nullptr;

	QRectF m_lastNormalizedRect;
	bool m_hasSelectedRect = false;

	bool isSetDrawnRectangles{ false };
	bool isSetAspectRatio{ false };
	bool isSetQImage{ false };

	QImage _qImage{ nullptr };

	bool m_isDrawing = false;

	// 等待一次绘制
	bool m_singleDrawPending = false;
};

