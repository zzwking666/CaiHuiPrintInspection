#include "PicturesPainter.h"
#include "ui_PicturesPainter.h"
#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include "PicturesPainter_SetAspectRatio.h"
#include <QTreeWidgetItem>

PicturesPainter::PicturesPainter(QWidget* parent)
	: QDialog(parent),
	ui(new Ui::PicturesPainterClass())
{
	ui->setupUi(this);
	build_ui();
	build_connect();

	drawLabel = qobject_cast<DrawLabel*>(ui->label_imgDisplay);
}

PicturesPainter::~PicturesPainter()
{
	delete ui;
}

void PicturesPainter::setRectangleConfigs(const std::vector<rw::rqw::RectangeConfig>& configs)
{
	_configs = configs;
}

std::vector<rw::rqw::PainterRectangleInfo> PicturesPainter::getRectangleConfigs()
{
	return _drawnRectangles;
}

void PicturesPainter::setDrawnRectangles(const std::vector<rw::rqw::PainterRectangleInfo>& Rectangles)
{
	if (Rectangles.empty())
	{
		return;
	}

	_drawnRectangles = Rectangles;
	isSetDrawnRectangles = true;
}

void PicturesPainter::setImage(const QImage& qImage)
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
		hide_ui();
	}
}

void PicturesPainter::setAspectRatio(double width, double height)
{
	if (width > 0 && height > 0)
	{
		img_Height = height;
		img_Width = width;
		isSetAspectRatio = true;
	}
}

void PicturesPainter::build_ui()
{
	ui->pbtn_ok->setEnabled(false);
	ui->btn_set->setEnabled(false);
	ui->btn_draw->setEnabled(false);
}

void PicturesPainter::release_ui()
{
	ui->pbtn_ok->setEnabled(true);
	ui->btn_set->setEnabled(true);
	ui->btn_draw->setEnabled(true);
}

void PicturesPainter::build_connect()
{
	QObject::connect(ui->btn_set, &QPushButton::clicked,
		this, &PicturesPainter::btn_set_clicked);
	QObject::connect(ui->btn_draw, &QPushButton::clicked,
		this, &PicturesPainter::btn_draw_clicked);
	QObject::connect(ui->pbtn_ok, &QPushButton::clicked,
		this, &PicturesPainter::pbtn_ok_clicked);
	QObject::connect(ui->pbtn_exit, &QPushButton::clicked,
		this, &PicturesPainter::pbtn_exit_clicked);
	QObject::connect(ui->pbtn_setAspectRatio, &QPushButton::clicked,
		this, &PicturesPainter::pbtn_setAspectRatio_clicked);
	QObject::connect(ui->pbtn_openPicture, &QPushButton::clicked,
		this, &PicturesPainter::pbtn_openPicture_clicked);
	QObject::connect(ui->label_imgDisplay, &DrawLabel::rectSelected,
		this, &PicturesPainter::onRectSelected);
	QObject::connect(ui->btn_clear, &QPushButton::clicked,
		this, &PicturesPainter::btn_clear_clicked);
}

void PicturesPainter::hide_ui()
{
	ui->btn_set->setVisible(false);
	ui->pbtn_setAspectRatio->setVisible(false);
	ui->pbtn_openPicture->setVisible(false);
}

void PicturesPainter::showWhiteImageOnLabel(double width, double height)
{
	if (width <= 0 || height <= 0) return;

	// 创建白色 QPixmap
	QPixmap whitePixmap(static_cast<int>(width), static_cast<int>(height));
	whitePixmap.fill(Qt::white);

	// 假设你的label叫ui->label
	ui->label_imgDisplay->setPixmap(whitePixmap);
	// 使label自适应图片大小
	ui->label_imgDisplay->setFixedSize(static_cast<int>(width), static_cast<int>(height));
}

QColor PicturesPainter::getColorByClassId(size_t size)
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

QString PicturesPainter::getNameByClassId(size_t size)
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

void PicturesPainter::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	// 设置UI为false
	build_ui();

	// 如果传入了纵横比并且参数有效
	if (isSetAspectRatio && img_Height > 0 && img_Width > 0)
	{
		showWhiteImageOnLabel(img_Width, img_Height);
		release_ui();
	}
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
	// 如果传入了绘画框
	if (isSetDrawnRectangles)
	{
		// 1. 检查pixmap
		if (!drawLabel->pixmap()) return;

		QPixmap origPix = drawLabel->pixmap().copy();
		QPixmap pix = origPix.copy();
		QPainter painter(&pix);

		// 2. 遍历所有绘画框
		for (const auto& info : _drawnRectangles)
		{
			QColor color = getColorByClassId(info.classId);
			QString name = getNameByClassId(info.classId);

			int imgW = drawLabel->width();
			int imgH = drawLabel->height();

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
			painter.drawText(rect.topLeft() + QPoint(3, 15), name);
		}
		drawLabel->setPixmap(pix);
	}
	else
	{
		// 初始化绘画框
		_drawnRectangles.clear();
	}

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

	// 默认选择第一项
	if (!m_listModel->rowCount()) return;
	QModelIndex firstIndex = m_listModel->index(0, 0);
	ui->listView->setCurrentIndex(firstIndex);

	// TableView模型
	if (!m_tableModel) {
		m_tableModel = new QStandardItemModel(this);
		m_tableModel->setColumnCount(2);
		m_tableModel->setRowCount(3); // classid, color, description
		ui->tableView->horizontalHeader()->setVisible(false);
		ui->tableView->setModel(m_tableModel);
	}

	// 初始化table内容
	onListViewCurrentChanged(firstIndex);

	// 连接信号槽：ListView项切换时更新TableView
	static bool connected = false;
	if (!connected) {
		connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &PicturesPainter::onListViewCurrentChanged);
		connected = true;
	}
}

void PicturesPainter::btn_set_clicked()
{
}

void PicturesPainter::btn_draw_clicked()
{
	m_isDrawing = !m_isDrawing;

	if (drawLabel) {
		drawLabel->setDrawingEnabled(m_isDrawing);
	}

	// 更新按钮文本
	if (m_isDrawing) {
		ui->btn_draw->setText("停止绘画");
	}
	else {
		ui->btn_draw->setText("开始绘画");
	}
}

void PicturesPainter::pbtn_ok_clicked()
{
	m_isDrawing = false;
	if (drawLabel) {
		drawLabel->setDrawingEnabled(false);
	}
	ui->btn_draw->setText("开始绘画");
	this->accept();
}

void PicturesPainter::pbtn_exit_clicked()
{
	m_isDrawing = false;
	if (drawLabel) {
		drawLabel->setDrawingEnabled(false);
	}
	ui->btn_draw->setText("开始绘画");
	this->reject();
}

void PicturesPainter::pbtn_setAspectRatio_clicked()
{
	PicturesPainterSetAspectRatio dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		img_Width = dlg.getImageWidth();
		img_Height = dlg.getImageHeight();
		// 在label上显示指定宽高的白色图片
		showWhiteImageOnLabel(img_Width, img_Height);
		release_ui();
	}
}

void PicturesPainter::pbtn_openPicture_clicked()
{
	// 1. 打开文件选择对话框
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("选择图片"),
		"",
		tr("Images (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)")
	);

	// 2. 检查是否选择了文件
	if (fileName.isEmpty())
		return;

	// 3. 加载图片
	QPixmap pixmap;
	if (!pixmap.load(fileName)) {
		QMessageBox::warning(this, tr("错误"), tr("无法加载图片！"));
		return;
	}

	// 给成员变量赋值
	_qImage = pixmap.toImage();

	// 4. 显示到label上（适当缩放以适应label尺寸）
	ui->label_imgDisplay->setPixmap(pixmap.scaled(
		ui->label_imgDisplay->size(),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation
	));

	// 5. 保存图片长宽
	img_Width = pixmap.width();
	img_Height = pixmap.height();

	release_ui();
}

void PicturesPainter::btn_clear_clicked()
{
	_drawnRectangles.clear();
	updateDrawLabel();
}

void PicturesPainter::onListViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (!current.isValid()) return;

	auto classid = current.data(Qt::UserRole + 1).toInt();
	auto color = qvariant_cast<QColor>(current.data(Qt::UserRole + 2));
	auto description = current.data(Qt::UserRole + 3).toString();

	m_tableModel->setItem(0, 0, new QStandardItem("classid"));
	m_tableModel->setItem(0, 1, new QStandardItem(QString::number(classid)));

	m_tableModel->setItem(1, 0, new QStandardItem("颜色"));
	m_tableModel->setItem(1, 0, new QStandardItem("颜色"));
	QStandardItem* colorItem = new QStandardItem(color.name());
	colorItem->setBackground(color); // 设置背景色
	m_tableModel->setItem(1, 1, colorItem);

	m_tableModel->setItem(2, 0, new QStandardItem("描述"));
	m_tableModel->setItem(2, 1, new QStandardItem(description));
}

void PicturesPainter::onRectSelected(const QRectF& rect)
{
	m_lastNormalizedRect = rect;
	m_hasSelectedRect = true;

	// 1. 获取当前选中项数据
	QModelIndex curIndex = ui->listView->currentIndex();
	if (!curIndex.isValid()) return;

	int classid = curIndex.data(Qt::UserRole + 1).toInt();
	QColor color = qvariant_cast<QColor>(curIndex.data(Qt::UserRole + 2));
	QString name = curIndex.data(Qt::DisplayRole).toString();

	// 2. 用 DrawLabel 框选的区域，转换为像素坐标
	int w = drawLabel->width();
	int h = drawLabel->height();

	QRect drawRect(
		int(rect.left() * w),
		int(rect.top() * h),
		int(rect.width() * w),
		int(rect.height() * h)
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
	painter.drawText(drawRect.topLeft() + QPoint(3, 15), name);

	// 4. 展示到 DrawLabel
	drawLabel->setPixmap(pix);

	// 5. 记录 PainterRectangleInfo
	rw::rqw::PainterRectangleInfo info;
	info.leftTop = { double(drawRect.left()) / w, double(drawRect.top()) / h };
	info.rightTop = { double(drawRect.right()) / w, double(drawRect.top()) / h };
	info.leftBottom = { double(drawRect.left()) / w, double(drawRect.bottom()) / h };
	info.rightBottom = { double(drawRect.right()) / w, double(drawRect.bottom()) / h };
	info.width = double(drawRect.width()) / w;
	info.height = double(drawRect.height()) / h;
	info.center_x = double(drawRect.center().x()) / w;
	info.center_y = double(drawRect.center().y()) / h;
	info.area = drawRect.width() * drawRect.height();
	info.classId = classid;
	info.score = -1;
	_drawnRectangles.push_back(info);

	// 清空已用的框选
	m_hasSelectedRect = false;
}

DrawLabel::DrawLabel(QWidget* parent)
	: QLabel(parent) {
	setMouseTracking(true);
	//setStyleSheet("border: 2px solid red;");
}

QRectF DrawLabel::getNormalizedRect() const
{
	QRect rect = QRect(m_startPoint, m_endPoint).normalized();
	return QRectF(
		rect.left() / static_cast<float>(width()),
		rect.top() / static_cast<float>(height()),
		rect.width() / static_cast<float>(width()),
		rect.height() / static_cast<float>(height())
	);
}

void DrawLabel::setImage(const QImage& img)
{
	m_srcImage = img;
	//setFixedSize(img.size()); // 固定label大小为图片原始大小
	updateScaledPixmap();
}

void DrawLabel::updateScaledPixmap()
{
	if (m_srcImage.isNull())
		return;

	QPixmap pix = QPixmap::fromImage(m_srcImage);

	if (width() > 0 && height() > 0) {
		pix = pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	setPixmap(pix);
}

void DrawLabel::resizeEvent(QResizeEvent* event)
{
	QLabel::resizeEvent(event);
}

void DrawLabel::mousePressEvent(QMouseEvent* event)
{
	QLabel::mousePressEvent(event);
	if (!m_drawingEnabled)
		return QLabel::mousePressEvent(event);

	if (event->button() == Qt::LeftButton) {
		m_isDrawing = true;
		m_startPoint = m_endPoint = event->pos();
		update();
	}
}

void DrawLabel::mouseMoveEvent(QMouseEvent* event)
{
	QLabel::mouseMoveEvent(event);
	if (!m_drawingEnabled)
		return QLabel::mouseMoveEvent(event);

	if (m_isDrawing) {
		m_endPoint = event->pos();
		update();
	}
}

void DrawLabel::mouseReleaseEvent(QMouseEvent* event)
{
	QLabel::mouseReleaseEvent(event);
	if (!m_drawingEnabled)
		return QLabel::mouseReleaseEvent(event);

	if (m_isDrawing && event->button() == Qt::LeftButton) {
		m_endPoint = event->pos();
		m_isDrawing = false;
		update();

		QRect rect = QRect(m_startPoint, m_endPoint).normalized();
		QRectF normRect(
			rect.left() / static_cast<float>(width()),
			rect.top() / static_cast<float>(height()),
			rect.width() / static_cast<float>(width()),
			rect.height() / static_cast<float>(height())
		);
		emit rectSelected(normRect); // 通知外部百分比坐标
	}
}

void DrawLabel::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);
	if (m_isDrawing) {
		QPainter painter(this);
		painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
		QRect rect = QRect(m_startPoint, m_endPoint).normalized();
		painter.drawRect(rect);
	}
}

void DrawLabel::setDrawingEnabled(bool enabled)
{
	m_drawingEnabled = enabled;
}

void PicturesPainter::updateDrawLabel()
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
	drawLabel->setScaledContents(false);        // 不让label跟随图片变化
	drawLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 锁死label大小
	drawLabel->setPixmap(pix.scaled(
		drawLabel->size(),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation
	));
}