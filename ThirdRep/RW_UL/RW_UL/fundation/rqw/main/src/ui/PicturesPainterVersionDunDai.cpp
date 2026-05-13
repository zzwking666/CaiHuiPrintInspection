#include "PicturesPainterVersionDunDai.h"

#include <QMessageBox>
#include <QPainter>

#include "ui_PicturesPainterVersionDunDai.h"

PicturesPainterVersionDunDai::PicturesPainterVersionDunDai(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::PicturesPainterVersionDunDaiClass())
{
	ui->setupUi(this);
	build_ui();
	build_connect();

	drawLabel = qobject_cast<DrawLabel*>(ui->label_imgDisplay);
}

PicturesPainterVersionDunDai::~PicturesPainterVersionDunDai()
{
	delete ui;
}

void PicturesPainterVersionDunDai::setRectangleConfigs(const std::vector<rw::rqw::RectangeConfig>& configs)
{
	_configs = configs;
}

void PicturesPainterVersionDunDai::setDrawnRectangles(const std::vector<rw::rqw::PainterRectangleInfo>& Rectangles)
{
	if (Rectangles.empty())
	{
		return;
	}

	_drawnRectangles = Rectangles;
	isSetDrawnRectangles = true;
}

void PicturesPainterVersionDunDai::setSmartRectangles(const std::vector<rw::rqw::PainterRectangleInfo>& Rectangles)
{
	if (Rectangles.empty())
	{
		return;
	}
	_smartRectangles = Rectangles;
}

void PicturesPainterVersionDunDai::setImage(const QImage& qImage)
{
	if (!qImage.isNull())
	{
		_qImage = qImage;
		img_Height = _qImage.height();
		img_Width = _qImage.width();
		isSetQImage = true;
		if (drawLabel) {
			drawLabel->setImage(_qImage);
			drawLabel->update();
		}
	}
}

QColor PicturesPainterVersionDunDai::getColorByClassId(size_t size)
{
	if (!_configs.empty())
	{
		for (const auto& item : _configs)
		{
			if (item.classid == size)
			{
				return item.color;
			}
		}
	}
}

QString PicturesPainterVersionDunDai::getNameByClassId(size_t size)
{
	if (!_configs.empty())
	{
		for (const auto& item : _configs)
		{
			if (item.classid == size)
			{
				return item.name;
			}
		}
	}
}

std::vector<rw::rqw::PainterRectangleInfo> PicturesPainterVersionDunDai::getRectangleConfigs()
{
	return _drawnRectangles;
}

std::vector<rw::rqw::PainterRectangleInfo> PicturesPainterVersionDunDai::getSmartRectangleConfigs()
{
	if (isGenerateSmartRectangles)
	{
		return _smartRectangles;
	}
	return {};
}

void PicturesPainterVersionDunDai::build_ui()
{
	ui->pbtn_ok->setEnabled(false);
}

void PicturesPainterVersionDunDai::release_ui()
{
	ui->pbtn_ok->setEnabled(true);
}

void PicturesPainterVersionDunDai::build_connect()
{
	QObject::connect(ui->pbtn_ok, &QPushButton::clicked,
		this, &PicturesPainterVersionDunDai::pbtn_ok_clicked);
	QObject::connect(ui->btn_close, &QPushButton::clicked,
		this, &PicturesPainterVersionDunDai::btn_close_clicked);
	QObject::connect(ui->label_imgDisplay, &DrawLabel::rectSelected,
		this, &PicturesPainterVersionDunDai::onRectSelected);
	QObject::connect(ui->btn_clear, &QPushButton::clicked,
		this, &PicturesPainterVersionDunDai::btn_clear_clicked);
	QObject::connect(ui->btn_zhinengpingbi, &QPushButton::clicked,
		this, &PicturesPainterVersionDunDai::btn_zhinengpingbi_clicked);
}

void PicturesPainterVersionDunDai::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	isGenerateSmartRectangles = false;
	// 设置UI为false
	build_ui();

	// 如果传入了图片并且参数有效
	if (isSetQImage && img_Height > 0 && img_Width > 0)
	{
		ui->label_imgDisplay->setPixmap(QPixmap::fromImage(_qImage).scaled(
			ui->label_imgDisplay->size(),
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation
		));
		release_ui();
	}
	// 1. 检查pixmap
	if (!drawLabel->pixmap()) return;

	QPixmap origPix = drawLabel->pixmap().copy();
	QPixmap pix = origPix.copy();
	QPainter painter(&pix);

	if (isDisSmartRectangleForFirstShow)
	{
		// 2. 遍历所有绘画框
		isGenerateSmartRectangles = true;
		for (const auto& info : _smartRectangles)
		{
			QColor color = getColorByClassId(info.classId);
			QString name = getNameByClassId(info.classId);

			int imgW = drawLabel->pixmap().width();
			int imgH = drawLabel->pixmap().height();

			QRect rect(
				static_cast<int>(info.leftTop.first * imgW),
				static_cast<int>(info.leftTop.second * imgH),
				static_cast<int>(info.width * imgW),
				static_cast<int>(info.height * imgH)
			);

			painter.setPen(QPen(color, 2));
			painter.drawRect(rect);

			QFont font = painter.font();
			font.setPointSize(16);
			painter.setFont(font);
			painter.setPen(color);
			painter.drawText(rect.topLeft() + QPoint(3, 15), name + "绘制屏蔽");
		}
	}

	// 如果传入了绘画框
	if (isSetDrawnRectangles)
	{


		// 2. 遍历所有绘画框
		for (const auto& info : _drawnRectangles)
		{
			QColor color = getColorByClassId(info.classId);
			QString name = getNameByClassId(info.classId);

			int imgW = drawLabel->pixmap().width();
			int imgH = drawLabel->pixmap().height();

			QRect rect(
				static_cast<int>(info.leftTop.first * imgW),
				static_cast<int>(info.leftTop.second * imgH),
				static_cast<int>(info.width * imgW),
				static_cast<int>(info.height * imgH)
			);

			painter.setPen(QPen(color, 2));
			painter.drawRect(rect);

			QFont font = painter.font();
			font.setPointSize(16);
			painter.setFont(font);
			painter.setPen(color);
			painter.drawText(rect.topLeft() + QPoint(3, 15), name + "绘制屏蔽");
		}
	}
	else
	{
		// 初始化绘画框
		_drawnRectangles.clear();
	}

	drawLabel->setPixmap(pix);

	// 只初始化一次
	if (!m_listModel) {
		m_listModel = new QStandardItemModel(this);
		ui->listView->setModel(m_listModel);
	}
	else {
		m_listModel->clear();
	}

	// 填充ListView
	for (const auto& cfg : _configs) {
		auto* item = new QStandardItem(cfg.name);
		item->setData(cfg.classid, Qt::UserRole + 1);  // 可根据需求存储更多数据
		item->setData(cfg.color, Qt::UserRole + 2);    // 存颜色
		item->setData(cfg.descrption, Qt::UserRole + 3);  // 存描述
		m_listModel->appendRow(item);
	}

	// 关键变更1：不自动触发绘画。可以（可选）不设置当前索引，保持“未选择”状态。
	// 如需默认选中第一项但仍不绘画，可取消下一行注释:
	// if (m_listModel->rowCount() > 0) ui->listView->setCurrentIndex(m_listModel->index(0,0));

	// 确保信号只连接一次
	static bool connected = false;
	if (!connected) {
		// 仅用于需要时跟踪当前变化（不启动绘画）
		connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &PicturesPainterVersionDunDai::onListViewCurrentChanged);
		// 新增：显式点击触发单次绘画（可重复点击同一项）
		connect(ui->listView, &QListView::clicked,
			this, &PicturesPainterVersionDunDai::onListViewItemClicked);
		connected = true;
	}

	// 关键变更2：初始不允许绘画
	if (drawLabel) {
		drawLabel->setDrawingEnabled(false);
	}
	m_isDrawing = false;
	m_singleDrawPending = false;
}

void PicturesPainterVersionDunDai::pbtn_ok_clicked()
{
	m_isDrawing = false;
	if (drawLabel) {
		drawLabel->setDrawingEnabled(false);
	}
	this->accept();
}

void PicturesPainterVersionDunDai::btn_close_clicked()
{
	m_isDrawing = false;
	if (drawLabel) {
		drawLabel->setDrawingEnabled(false);
	}
	this->reject();
}

void PicturesPainterVersionDunDai::btn_clear_clicked()
{
	isGenerateSmartRectangles = false;
	_drawnRectangles.clear();
	updateDrawLabel();
}

void PicturesPainterVersionDunDai::btn_zhinengpingbi_clicked()
{
	// 1. 检查pixmap
	if (!drawLabel->pixmap()) return;

	QPixmap origPix = drawLabel->pixmap().copy();
	QPixmap pix = origPix.copy();
	QPainter painter(&pix);

	// 2. 遍历所有绘画框
	for (const auto& info : _smartRectangles)
	{
		QColor color = getColorByClassId(info.classId);
		QString name = getNameByClassId(info.classId);

		int imgW = drawLabel->pixmap().width();
		int imgH = drawLabel->pixmap().height();

		QRect rect(
			static_cast<int>(info.leftTop.first * imgW),
			static_cast<int>(info.leftTop.second * imgH),
			static_cast<int>(info.width * imgW),
			static_cast<int>(info.height * imgH)
		);

		painter.setPen(QPen(color, 2));
		painter.drawRect(rect);

		QFont font = painter.font();
		font.setPointSize(16);
		painter.setFont(font);
		painter.setPen(color);
		painter.drawText(rect.topLeft() + QPoint(3, 15), name + "智能屏蔽");
	}
	isGenerateSmartRectangles = true;
	drawLabel->setPixmap(pix);
}

void PicturesPainterVersionDunDai::onRectSelected(const QRectF& rect)
{
	// 仅在等待单次绘制时响应
	if (!m_singleDrawPending) {
		return;
	}

	m_lastNormalizedRect = rect;
	m_hasSelectedRect = true;

	// 1. 获取当前选中项数据
	QModelIndex curIndex = ui->listView->currentIndex();
	if (!curIndex.isValid()) return;

	int classid = curIndex.data(Qt::UserRole + 1).toInt();
	QColor color = qvariant_cast<QColor>(curIndex.data(Qt::UserRole + 2));
	QString name = curIndex.data(Qt::DisplayRole).toString();

	auto imgW = drawLabel->pixmap().width();
	auto imgH = drawLabel->pixmap().height();
	int w = drawLabel->width();
	int h = drawLabel->height();

	QRectF scaledRect;

	auto scaleX = static_cast<double>(imgW) / w;
	auto scaleY = static_cast<double>(imgH) / h;

	if (imgW==w)
	{
		// 图片宽度与控件宽度一致，Y轴居中
		int offsetY = (h - imgH) / 2;
		// 先还原为控件像素坐标，再减去 offsetY
		double rectX = m_lastNormalizedRect.x() * w;
		double rectY = m_lastNormalizedRect.y() * h - offsetY;
		double rectWidth = m_lastNormalizedRect.width() * w;
		double rectHeight = m_lastNormalizedRect.height() * h;

		// 构造原始框和图片区域
		QRectF rect(rectX, rectY, rectWidth, rectHeight);
		QRectF imgRect(0, 0, imgW, imgH);

		// 取交集
		QRectF intersectedRect = rect.intersected(imgRect);

		if (intersectedRect.isEmpty()) {
			QMessageBox::information(this, "提示", "框选区域在图片外部！");
			return;
		}

		// 转为归一化坐标
		scaledRect = QRectF(
			intersectedRect.x() / imgW,
			intersectedRect.y() / imgH,
			intersectedRect.width() / imgW,
			intersectedRect.height() / imgH
		);
	}
	else if (imgH == h)
	{
		// 图片高度与控件高度一致，图片靠左显示
// m_lastNormalizedRect 是控件坐标系下的归一化数据
// 直接映射到图片坐标系，但要做越界处理
		double rectX = m_lastNormalizedRect.x() * w;
		double rectY = m_lastNormalizedRect.y() * h;
		double rectWidth = m_lastNormalizedRect.width() * w;
		double rectHeight = m_lastNormalizedRect.height() * h;

		// 检查是否在图片外部
		if (rectX >= imgW || rectX + rectWidth <= 0 ||
			rectY >= imgH || rectY + rectHeight <= 0) {
			QMessageBox::information(this, "提示", "框选区域在图片外部！");
			return;
		}

		// 保证 X 不越界
		rectX = std::max(0.0, rectX);
		if (rectX + rectWidth > imgW) {
			rectWidth = imgW - rectX;
		}

		// 保证 Y 不越界
		rectY = std::max(0.0, rectY);
		if (rectY + rectHeight > imgH) {
			rectHeight = imgH - rectY;
		}

		scaledRect = QRectF(
			rectX / imgW,
			rectY / imgH,
			rectWidth / imgW,
			rectHeight / imgH
		);
	}
	else
	{
		scaledRect = rect;
		
	}



	m_lastNormalizedRect = scaledRect;

	QRect drawRect(
		int(m_lastNormalizedRect.left() * imgW),
		int(m_lastNormalizedRect.top() * imgH),
		int(m_lastNormalizedRect.width() * imgW),
		int(m_lastNormalizedRect.height() * imgH)
	);

	if (drawRect.width() <= 0 || drawRect.height() <= 0) {
		QMessageBox::information(this, "提示", "框选区域无效！");
		return;
	}

	// 3. 在 DrawLabel 的 pixmap 上画
	if (!drawLabel->pixmap()) {
		QMessageBox::warning(this, "错误", "还没有加载图片！");
		return;
	}
	QPixmap origPix = drawLabel->pixmap().copy(); // copy自带深拷贝
	QPixmap pix = origPix.copy();
	QPainter painter(&pix);
	painter.setPen(QPen(color, 2));
	painter.drawRect(drawRect);
	painter.setPen(color);
	QFont font = painter.font();
	font.setPointSize(16);
	painter.setFont(font);
	painter.drawText(drawRect.topLeft() + QPoint(3, 15), name + "绘制屏蔽");

	// 4. 展示到 DrawLabel
	drawLabel->setPixmap(pix);

	// 5. 记录 PainterRectangleInfo
	rw::rqw::PainterRectangleInfo info;
	info.leftTop = { double(drawRect.left()) / imgW, double(drawRect.top()) / imgH };
	info.rightTop = { double(drawRect.right()) / imgW, double(drawRect.top()) / imgH };
	info.leftBottom = { double(drawRect.left()) / imgW, double(drawRect.bottom()) / imgH };
	info.rightBottom = { double(drawRect.right()) / imgW, double(drawRect.bottom()) / imgH };
	info.width = double(drawRect.width()) / imgW;
	info.height = double(drawRect.height()) / imgH;
	info.center_x = double(drawRect.center().x()) / imgW;
	info.center_y = double(drawRect.center().y()) / imgH;
	info.area = drawRect.width() * drawRect.height();
	info.classId = classid;
	info.score = -1;
	_drawnRectangles.push_back(info);

	// 单次绘画结束，关闭绘画模式
	m_singleDrawPending = false;
	m_hasSelectedRect = false;
	m_isDrawing = false;
	drawLabel->setDrawingEnabled(false);
}

void PicturesPainterVersionDunDai::updateDrawLabel()
{
	if (_qImage.isNull()) {
		drawLabel->clear();
		return;
	}
	QPixmap pix = QPixmap::fromImage(_qImage).copy();

	QPainter painter(&pix);
	for (const auto& info : _drawnRectangles) {
		int imgW = pix.width();
		int imgH = pix.height();

		// 百分比转像素
		QPoint lt(info.leftTop.first * imgW, info.leftTop.second * imgH);
		int rectWidth = info.width * imgW;
		int rectHeight = info.height * imgH;
		QRect drawRect(lt, QSize(rectWidth, rectHeight));

		// 设置画笔颜色和宽度
		QColor color = Qt::red;
		color = getColorByClassId(info.classId);

		painter.setPen(QPen(color, 2));
		painter.drawRect(drawRect);

		// 绘制类别名，如果有的话
		QString name = "";
		name = getNameByClassId(info.classId);

		painter.setPen(color);
		QFont font = painter.font();
		font.setPointSize(16);
		painter.setFont(font);

		painter.drawText(drawRect.topLeft() + QPoint(3, 15), name);
	}

	if (isGenerateSmartRectangles)
	{
		for (const auto& info : _smartRectangles) {
			int imgW = pix.width();
			int imgH = pix.height();

			// 百分比转像素
			QPoint lt(info.leftTop.first * imgW, info.leftTop.second * imgH);
			int rectWidth = info.width * imgW;
			int rectHeight = info.height * imgH;
			QRect drawRect(lt, QSize(rectWidth, rectHeight));

			// 设置画笔颜色和宽度
			QColor color = Qt::red;
			color = getColorByClassId(info.classId);

			painter.setPen(QPen(color, 2));
			painter.drawRect(drawRect);

			// 绘制类别名，如果有的话
			QString name = "";
			name = getNameByClassId(info.classId);

			painter.setPen(color);
			QFont font = painter.font();
			font.setPointSize(16);
			painter.setFont(font);

			painter.drawText(drawRect.topLeft() + QPoint(3, 15), name);
		}
	}

	drawLabel->setPixmap(pix.scaled(
		drawLabel->size(),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation
	));
}

void PicturesPainterVersionDunDai::onListViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	// 现在不在这里启动绘画，只是保留接口（如需显示详情可在此扩展）
	if (!current.isValid()) return;
}

void PicturesPainterVersionDunDai::onListViewItemClicked(const QModelIndex& index)
{
	if (!index.isValid()) return;
	if (!isSetQImage || !drawLabel) return;

	// 每次显式点击都准备“单次绘制”
	m_singleDrawPending = true;
	m_isDrawing = true;
	drawLabel->setDrawingEnabled(true);
	// 如果当前未选中该索引，则设置（重复点击同一项也允许）
	ui->listView->setCurrentIndex(index);
}
