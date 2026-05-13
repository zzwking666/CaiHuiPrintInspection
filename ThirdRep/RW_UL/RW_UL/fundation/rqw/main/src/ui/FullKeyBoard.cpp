#include "FullKeyBoard.h"

#include <QMessageBox>
#include <qtabbar.h>

#include "ui_FullKeyBoard.h"

FullKeyBoard::FullKeyBoard(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::FullKeyBoardClass())
{
	ui->setupUi(this);

	build_ui();
	build_connect();
	initializeKeyMap();
}

FullKeyBoard::~FullKeyBoard()
{
	delete ui;
}

QString FullKeyBoard::getValue() const
{
	if (value.isEmpty())
	{
		return "0";
	}
	return value;
}

void FullKeyBoard::build_ui()
{
	ui->tabWidget->tabBar()->hide();
	ui->tabWidget->setStyleSheet(
		"QTabWidget::pane { border: none; }"
		"QTabBar { border: none; }"
		"QTabBar::tab { border: none; background: transparent; }"
	);
	ui->tabWidget->setCurrentIndex(0);

	// 设置光标位置并保持焦点
	ui->lineEdit->setFocus();
	ui->lineEdit->setCursorPosition(value.length());

	// 移除输入限制,允许键盘输入和粘贴
	// ui->lineEdit->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 已注释,允许鼠标操作
	ui->lineEdit->setFocusPolicy(Qt::StrongFocus);
	ui->lineEdit->installEventFilter(this);
}

void FullKeyBoard::build_connect()
{
	// 数字按钮连接
	connect(ui->pbtn_num1, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num1_clicked);
	connect(ui->pbtn_num2, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num2_clicked);
	connect(ui->pbtn_num3, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num3_clicked);
	connect(ui->pbtn_num4, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num4_clicked);
	connect(ui->pbtn_num5, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num5_clicked);
	connect(ui->pbtn_num6, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num6_clicked);
	connect(ui->pbtn_num7, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num7_clicked);
	connect(ui->pbtn_num8, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num8_clicked);
	connect(ui->pbtn_num9, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num9_clicked);
	connect(ui->pbtn_num0, &QPushButton::clicked, this, &FullKeyBoard::pbtn_num0_clicked);

	// 列出所有需要连接的按钮
	QList<QPushButton*> buttons = {
		ui->pbtn_numq, ui->pbtn_numw, ui->pbtn_nume, ui->pbtn_numr, ui->pbtn_numt,
		ui->pbtn_numy, ui->pbtn_numu, ui->pbtn_numi, ui->pbtn_numo, ui->pbtn_nump,
		ui->pbtn_numa, ui->pbtn_nums, ui->pbtn_numd, ui->pbtn_numf, ui->pbtn_numg,
		ui->pbtn_numh, ui->pbtn_numj, ui->pbtn_numk, ui->pbtn_numl, ui->pbtn_numz,
		ui->pbtn_numx, ui->pbtn_numc, ui->pbtn_numv, ui->pbtn_numb, ui->pbtn_numn,
		ui->pbtn_numm, ui->pbtn_numdunhao, ui->pbtn_numjuhao
	};

	// 连接所有按钮到同一个槽
	for (auto button : buttons) {
		connect(button, &QPushButton::clicked, this, &FullKeyBoard::handleButtonClicked);
	}

	// 特殊按钮连接
	connect(ui->pbtn_cancel, &QPushButton::clicked, this, &FullKeyBoard::pbtn_cancel_clicked);

	// 连接未连接的数字和符号按钮槽函数
	connect(ui->pbtn_numjianhao, &QPushButton::clicked, this, &FullKeyBoard::pbtn_numjianhao_clicked);
	connect(ui->pbtn_numfenhao, &QPushButton::clicked, this, &FullKeyBoard::pbtn_numfenhao_clicked);

	// 连接其他控制按钮槽函数
	connect(ui->pbtn_huiche, &QPushButton::clicked, this, &FullKeyBoard::pbtn_huiche_clicked);
	connect(ui->pbtn_kongge, &QPushButton::clicked, this, &FullKeyBoard::pbtn_kongge_clicked);
	connect(ui->pbtn_qiehuanfuhao, &QPushButton::clicked, this, &FullKeyBoard::pbtn_qiehuanfuhao_clicked);
	connect(ui->pbtn_zuo, &QPushButton::clicked, this, &FullKeyBoard::pbtn_zuo_clicked);
	connect(ui->pbtn_you, &QPushButton::clicked, this, &FullKeyBoard::pbtn_you_clicked);
	connect(ui->pbtn_shanchu, &QPushButton::clicked, this, &FullKeyBoard::pbtn_shanchu_clicked);
	connect(ui->pbtn_daxie, &QPushButton::clicked, this, &FullKeyBoard::pbtn_daxie_clicked);

	connect(ui->lineEdit, &QLineEdit::textChanged, this, &FullKeyBoard::onLineEditTextChanged);
}

void FullKeyBoard::showEvent(QShowEvent* show_event)
{
	QDialog::showEvent(show_event);
	value.clear();
	ui->lineEdit->clear();
}

void FullKeyBoard::appendCharacter(QPushButton* button)
{
	if (!keyMap.contains(button)) {
		return; // 按钮不在 keyMap 中，防止错误
	}

	QString character;
	KeyEntry entry = keyMap[button];

	switch (currentMode) {
	case LowerCase:
		character = entry.lower;
		break;
	case UpperCase:
		character = entry.upper;
		break;
	case Symbol:
		if (!entry.symbol.isEmpty()) {
			character = entry.symbol;
		}
		else {
			character = entry.lower; // 或者其他默认行为
		}
		break;
	}

	// 获取当前光标的位置
	int pos = ui->lineEdit->cursorPosition();

	// 在 'value' 的当前光标位置插入字符
	value.insert(pos, character);

	// 更新 QLineEdit 的文本
	ui->lineEdit->setText(value);

	// 将光标移动到插入字符的后面
	ui->lineEdit->setCursorPosition(pos + character.length());
}

void FullKeyBoard::appendCharacter(const QString& character)
{
	// 获取当前光标的位置
	int pos = ui->lineEdit->cursorPosition();

	// 在 'value' 的当前光标位置插入字符
	value.insert(pos, character);

	// 更新 QLineEdit 的文本
	ui->lineEdit->setText(value);

	// 将光标移动到插入字符的后面
	ui->lineEdit->setCursorPosition(pos + character.length());
}

void FullKeyBoard::pbtn_numfenhao_clicked()
{
	appendCharacter(";");
}

void FullKeyBoard::pbtn_numjianhao_clicked()
{
	appendCharacter("-");
}

void FullKeyBoard::pbtn_huiche_clicked()
{
	this->accept();
}

void FullKeyBoard::pbtn_kongge_clicked()
{
	appendCharacter(" ");
}

void FullKeyBoard::pbtn_qiehuanfuhao_clicked()
{
	switch (currentMode)
	{
	case LowerCase:
		currentMode = Symbol;
		break;
	case UpperCase:
		currentMode = Symbol;
		break;
	case Symbol:
		currentMode = LowerCase;
		break;
	default:
		break;
	}
	updateButtonTexts();
}

void FullKeyBoard::pbtn_zuo_clicked()
{
	// 获取当前光标的位置
	int pos = ui->lineEdit->cursorPosition();

	// 如果光标不在最左端，则向左移动
	if (pos > 0) {
		pos--;
		ui->lineEdit->setCursorPosition(pos);
	}
}

void FullKeyBoard::pbtn_you_clicked()
{
	// 获取当前光标的位置
	int pos = ui->lineEdit->cursorPosition();

	// 获取文本的长度
	int textLength = ui->lineEdit->text().length();

	// 如果光标不在最右端，则向右移动
	if (pos < textLength) {
		pos++;
		ui->lineEdit->setCursorPosition(pos);
	}
}

void FullKeyBoard::pbtn_shanchu_clicked()
{
	// 获取当前光标的位置
	int pos = ui->lineEdit->cursorPosition();

	// 确保光标不在最左端
	if (pos > 0) {
		// 在 'value' 中删除光标左侧的一个字符
		value.remove(pos - 1, 1);

		// 更新 QLineEdit 的文本
		ui->lineEdit->setText(value);

		// 将光标向左移动一个位置
		ui->lineEdit->setCursorPosition(pos - 1);
	}
}

void FullKeyBoard::onLineEditTextChanged(const QString& text)
{
	value = text;
}

void FullKeyBoard::pbtn_num1_clicked()
{
	appendCharacter("1");
}

void FullKeyBoard::pbtn_num2_clicked()
{
	appendCharacter("2");
}

void FullKeyBoard::pbtn_num3_clicked()
{
	appendCharacter("3");
}

void FullKeyBoard::pbtn_num4_clicked()
{
	appendCharacter("4");
}

void FullKeyBoard::pbtn_num5_clicked()
{
	appendCharacter("5");
}

void FullKeyBoard::pbtn_num6_clicked()
{
	appendCharacter("6");
}

void FullKeyBoard::pbtn_num7_clicked()
{
	appendCharacter("7");
}

void FullKeyBoard::pbtn_num8_clicked()
{
	appendCharacter("8");
}

void FullKeyBoard::pbtn_num9_clicked()
{
	appendCharacter("9");
}

void FullKeyBoard::pbtn_num0_clicked()
{
	appendCharacter("0");
}

void FullKeyBoard::pbtn_cancel_clicked()
{
	this->reject();
}

void FullKeyBoard::pbtn_daxie_clicked()
{
	switch (currentMode)
	{
	case LowerCase:
		currentMode = UpperCase;
		break;
	case UpperCase:
		currentMode = LowerCase;
		break;
	case Symbol:
		QMessageBox::warning(this, "警告", "请先将输入模式切换到字母!");
		break;
	default:
		break;
	}
	updateButtonTexts();
}

void FullKeyBoard::updateButtonTexts()
{
	for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
		QPushButton* button = it.key();
		const KeyEntry& entry = it.value();

		switch (currentMode) {
		case LowerCase:
			button->setText(entry.lower);
			break;
		case UpperCase:
			button->setText(entry.upper);
			break;
		case Symbol:
			QString displayText = entry.symbol;
			// 替换单个 & 为 && 以正确显示
			displayText.replace("&", "&&");
			button->setText(displayText);
			break;
		}
	}
}

// 事件过滤器实现
bool FullKeyBoard::eventFilter(QObject* obj, QEvent* event)
{
	// 不再拦截任何事件,允许正常的键盘输入、粘贴等操作
	return QWidget::eventFilter(obj, event);
}

void FullKeyBoard::initializeKeyMap()
{
	// 字母 Q-W-E-R-T-Y-U-I-O-P
	keyMap[ui->pbtn_numq] = KeyEntry{ "q", "Q", "!" };
	keyMap[ui->pbtn_numw] = KeyEntry{ "w", "W", "@" };
	keyMap[ui->pbtn_nume] = KeyEntry{ "e", "E", "#" };
	keyMap[ui->pbtn_numr] = KeyEntry{ "r", "R", "$" };
	keyMap[ui->pbtn_numt] = KeyEntry{ "t", "T", "%" };
	keyMap[ui->pbtn_numy] = KeyEntry{ "y", "Y", "^" };
	keyMap[ui->pbtn_numu] = KeyEntry{ "u", "U", "&" };
	keyMap[ui->pbtn_numi] = KeyEntry{ "i", "I", "*" };
	keyMap[ui->pbtn_numo] = KeyEntry{ "o", "O", "(" };
	keyMap[ui->pbtn_nump] = KeyEntry{ "p", "P", ")" };

	// 字母 A-S-D-F-G-H-J-K-L
	keyMap[ui->pbtn_numa] = KeyEntry{ "a", "A", "?" };
	keyMap[ui->pbtn_nums] = KeyEntry{ "s", "S", "[" };
	keyMap[ui->pbtn_numd] = KeyEntry{ "d", "D", "]" };
	keyMap[ui->pbtn_numf] = KeyEntry{ "f", "F", "{" };
	keyMap[ui->pbtn_numg] = KeyEntry{ "g", "G", "}" };
	keyMap[ui->pbtn_numh] = KeyEntry{ "h", "H", ":" };
	keyMap[ui->pbtn_numj] = KeyEntry{ "j", "J", ";" };
	keyMap[ui->pbtn_numk] = KeyEntry{ "k", "K", "\"" };
	keyMap[ui->pbtn_numl] = KeyEntry{ "l", "L", "'" };

	// 字母 Z-X-C-V-B-N-M
	keyMap[ui->pbtn_numz] = KeyEntry{ "z", "Z", "/" };
	keyMap[ui->pbtn_numx] = KeyEntry{ "x", "X", "-" };
	keyMap[ui->pbtn_numc] = KeyEntry{ "c", "C", "_" };
	keyMap[ui->pbtn_numv] = KeyEntry{ "v", "V", "<" };
	keyMap[ui->pbtn_numb] = KeyEntry{ "b", "B", ">" };
	keyMap[ui->pbtn_numn] = KeyEntry{ "n", "N", "=" };
	keyMap[ui->pbtn_numm] = KeyEntry{ "m", "M", "+" };

	// 特殊字符
	keyMap[ui->pbtn_numdunhao] = KeyEntry{ "、", "、", "," };
	keyMap[ui->pbtn_numjuhao] = KeyEntry{ "。", "。", "." };
	keyMap[ui->pbtn_daxie] = KeyEntry{ "abc", "ABC", " " };
	keyMap[ui->pbtn_qiehuanfuhao] = KeyEntry{ "?#&&", "?#&&", "abc" };
}

void FullKeyBoard::handleButtonClicked()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());
	if (button && keyMap.contains(button)) {
		appendCharacter(button);
	}
}