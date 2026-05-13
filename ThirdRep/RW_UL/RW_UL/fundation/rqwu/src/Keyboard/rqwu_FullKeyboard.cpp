#include "rqwu/Keyboard/rqwu_FullKeyboard.h"

#include"rqwu/rqwu_MessageBox.h"

#include<QTabBar>
#include <QLineEdit>
#include <QMouseEvent>


#include "ui_rqwu_FullKeyBoard.h"

namespace rw
{
	namespace rqwu
	{
		FullKeyboard::FullKeyboard(QWidget* parent)
			: rqwu::DialogDraggable(parent)
			, ui(new Ui::FullKeyBoardClass())
		{
			ui->setupUi(this);

			build_ui();
			build_connect();
			initializeKeyMap();
		}

		FullKeyboard::~FullKeyboard()
		{
			delete ui;
		}

		QString FullKeyboard::getValue() const
		{
			return value;
		}

		void FullKeyboard::build_ui()
		{
			ui->tabWidget->tabBar()->hide();
			ui->tabWidget->setStyleSheet(
				"QTabWidget::pane { border: none; }"
				"QTabBar { border: none; }"
				"QTabBar::tab { border: none; background: transparent; }"
			);
			ui->tabWidget->setCurrentIndex(0);

			// Set cursor position and keep focus
			ui->lineEdit->setFocus();
			ui->lineEdit->setCursorPosition(value.length());

			// Remove input restrictions, allow keyboard input and paste
			// ui->lineEdit->setAttribute(Qt::WA_TransparentForMouseEvents, true); // Commented out, allow mouse operation
			ui->lineEdit->setFocusPolicy(Qt::StrongFocus);
			ui->lineEdit->installEventFilter(this);
		}

		void FullKeyboard::build_connect()
		{
			// Connect number buttons
			connect(ui->pbtn_num1, &QPushButton::clicked, this, &FullKeyboard::pbtn_num1_clicked);
			connect(ui->pbtn_num2, &QPushButton::clicked, this, &FullKeyboard::pbtn_num2_clicked);
			connect(ui->pbtn_num3, &QPushButton::clicked, this, &FullKeyboard::pbtn_num3_clicked);
			connect(ui->pbtn_num4, &QPushButton::clicked, this, &FullKeyboard::pbtn_num4_clicked);
			connect(ui->pbtn_num5, &QPushButton::clicked, this, &FullKeyboard::pbtn_num5_clicked);
			connect(ui->pbtn_num6, &QPushButton::clicked, this, &FullKeyboard::pbtn_num6_clicked);
			connect(ui->pbtn_num7, &QPushButton::clicked, this, &FullKeyboard::pbtn_num7_clicked);
			connect(ui->pbtn_num8, &QPushButton::clicked, this, &FullKeyboard::pbtn_num8_clicked);
			connect(ui->pbtn_num9, &QPushButton::clicked, this, &FullKeyboard::pbtn_num9_clicked);
			connect(ui->pbtn_num0, &QPushButton::clicked, this, &FullKeyboard::pbtn_num0_clicked);

			// List all buttons to be connected
			QList<QPushButton*> buttons = {
				ui->pbtn_numq, ui->pbtn_numw, ui->pbtn_nume, ui->pbtn_numr, ui->pbtn_numt,
				ui->pbtn_numy, ui->pbtn_numu, ui->pbtn_numi, ui->pbtn_numo, ui->pbtn_nump,
				ui->pbtn_numa, ui->pbtn_nums, ui->pbtn_numd, ui->pbtn_numf, ui->pbtn_numg,
				ui->pbtn_numh, ui->pbtn_numj, ui->pbtn_numk, ui->pbtn_numl, ui->pbtn_numz,
				ui->pbtn_numx, ui->pbtn_numc, ui->pbtn_numv, ui->pbtn_numb, ui->pbtn_numn,
				ui->pbtn_numm, ui->pbtn_numdunhao, ui->pbtn_numjuhao
			};

			// Connect all buttons to the same slot
			for (auto button : buttons) {
				connect(button, &QPushButton::clicked, this, &FullKeyboard::handleButtonClicked);
			}

			// Special button connections
			connect(ui->pbtn_cancel, &QPushButton::clicked, this, &FullKeyboard::pbtn_cancel_clicked);

			// Connect unconnected number and symbol button slots
			connect(ui->pbtn_numjianhao, &QPushButton::clicked, this, &FullKeyboard::pbtn_numjianhao_clicked);
			connect(ui->pbtn_numfenhao, &QPushButton::clicked, this, &FullKeyboard::pbtn_numfenhao_clicked);

			// Connect other control button slots
			connect(ui->pbtn_huiche, &QPushButton::clicked, this, &FullKeyboard::pbtn_huiche_clicked);
			connect(ui->pbtn_kongge, &QPushButton::clicked, this, &FullKeyboard::pbtn_kongge_clicked);
			connect(ui->pbtn_qiehuanfuhao, &QPushButton::clicked, this, &FullKeyboard::pbtn_qiehuanfuhao_clicked);
			connect(ui->pbtn_zuo, &QPushButton::clicked, this, &FullKeyboard::pbtn_zuo_clicked);
			connect(ui->pbtn_you, &QPushButton::clicked, this, &FullKeyboard::pbtn_you_clicked);
			connect(ui->pbtn_shanchu, &QPushButton::clicked, this, &FullKeyboard::pbtn_shanchu_clicked);
			connect(ui->pbtn_daxie, &QPushButton::clicked, this, &FullKeyboard::pbtn_daxie_clicked);

			connect(ui->lineEdit, &QLineEdit::textChanged, this, &FullKeyboard::onLineEditTextChanged);
		}

		void FullKeyboard::showEvent(QShowEvent* show_event)
		{
			QDialog::showEvent(show_event);
			value.clear();
			ui->lineEdit->clear();
		}

		void FullKeyboard::appendCharacter(QPushButton* button)
		{
			if (!keyMap.contains(button)) {
				return; // Button not in keyMap, prevent error
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
					character = entry.lower; // Or other default behavior
				}
				break;
			}

			// Get current cursor position
			int pos = ui->lineEdit->cursorPosition();

			// Insert character at current cursor position in 'value'
			value.insert(pos, character);

			// Update QLineEdit text
			ui->lineEdit->setText(value);

			// Move cursor to after the inserted character
			ui->lineEdit->setCursorPosition(pos + character.length());
		}

		void FullKeyboard::appendCharacter(const QString& character)
		{
			// Get current cursor position
			int pos = ui->lineEdit->cursorPosition();

			// Insert character at current cursor position in 'value'
			value.insert(pos, character);

			// Update QLineEdit text
			ui->lineEdit->setText(value);

			// Move cursor to after the inserted character
			ui->lineEdit->setCursorPosition(pos + character.length());
		}

		void FullKeyboard::pbtn_numfenhao_clicked()
		{
			appendCharacter(";");
		}

		void FullKeyboard::pbtn_numjianhao_clicked()
		{
			appendCharacter("-");
		}

		void FullKeyboard::pbtn_huiche_clicked()
		{
			if (value.isEmpty())
			{
				if (emptyInputPolicy == Keyboard::EmptyInputPolicy::DisableAndMessageBox)
				{
					rqwu::MessageBox::warning(this, "错误", "未输入任何数值");
					return;
				}
				else if (emptyInputPolicy == Keyboard::EmptyInputPolicy::EnableButReject)
				{
					this->reject();
				}
				else if (emptyInputPolicy == Keyboard::EmptyInputPolicy::EnableAndAccept)
				{
					this->accept();
				}
			}
			else
			{
				this->accept();
			}
		}

		void FullKeyboard::pbtn_kongge_clicked()
		{
			appendCharacter(" ");
		}

		void FullKeyboard::pbtn_qiehuanfuhao_clicked()
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

		void FullKeyboard::pbtn_zuo_clicked()
		{
			// Get current cursor position
			int pos = ui->lineEdit->cursorPosition();

			// If cursor is not at the far left, move left
			if (pos > 0) {
				pos--;
				ui->lineEdit->setCursorPosition(pos);
			}
		}

		void FullKeyboard::pbtn_you_clicked()
		{
			// Get current cursor position
			int pos = ui->lineEdit->cursorPosition();

			// Get text length
			int textLength = ui->lineEdit->text().length();

			// If cursor is not at the far right, move right
			if (pos < textLength) {
				pos++;
				ui->lineEdit->setCursorPosition(pos);
			}
		}

		void FullKeyboard::pbtn_shanchu_clicked()
		{
			// Get current cursor position
			int pos = ui->lineEdit->cursorPosition();

			// Ensure cursor is not at the far left
			if (pos > 0) {
				// Delete one character to the left of the cursor in 'value'
				value.remove(pos - 1, 1);

				// Update QLineEdit text
				ui->lineEdit->setText(value);

				// Move cursor one position to the left
				ui->lineEdit->setCursorPosition(pos - 1);
			}
		}

		void FullKeyboard::onLineEditTextChanged(const QString& text)
		{
			value = text;
		}

		void FullKeyboard::pbtn_num1_clicked()
		{
			appendCharacter("1");
		}

		void FullKeyboard::pbtn_num2_clicked()
		{
			appendCharacter("2");
		}

		void FullKeyboard::pbtn_num3_clicked()
		{
			appendCharacter("3");
		}

		void FullKeyboard::pbtn_num4_clicked()
		{
			appendCharacter("4");
		}

		void FullKeyboard::pbtn_num5_clicked()
		{
			appendCharacter("5");
		}

		void FullKeyboard::pbtn_num6_clicked()
		{
			appendCharacter("6");
		}

		void FullKeyboard::pbtn_num7_clicked()
		{
			appendCharacter("7");
		}

		void FullKeyboard::pbtn_num8_clicked()
		{
			appendCharacter("8");
		}

		void FullKeyboard::pbtn_num9_clicked()
		{
			appendCharacter("9");
		}

		void FullKeyboard::pbtn_num0_clicked()
		{
			appendCharacter("0");
		}

		void FullKeyboard::pbtn_cancel_clicked()
		{
			this->reject();
		}

		void FullKeyboard::pbtn_daxie_clicked()
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
				rqwu::MessageBox::warning(this, "警告", "请先将输入模式切换到字母!");
				break;
			default:
				break;
			}
			updateButtonTexts();
		}

		void FullKeyboard::updateButtonTexts()
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

		bool FullKeyboard::eventFilter(QObject* obj, QEvent* event)
		{
			return QWidget::eventFilter(obj, event);
		}

		void FullKeyboard::initializeKeyMap()
		{
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

			keyMap[ui->pbtn_numa] = KeyEntry{ "a", "A", "?" };
			keyMap[ui->pbtn_nums] = KeyEntry{ "s", "S", "[" };
			keyMap[ui->pbtn_numd] = KeyEntry{ "d", "D", "]" };
			keyMap[ui->pbtn_numf] = KeyEntry{ "f", "F", "{" };
			keyMap[ui->pbtn_numg] = KeyEntry{ "g", "G", "}" };
			keyMap[ui->pbtn_numh] = KeyEntry{ "h", "H", ":" };
			keyMap[ui->pbtn_numj] = KeyEntry{ "j", "J", ";" };
			keyMap[ui->pbtn_numk] = KeyEntry{ "k", "K", "\"" };
			keyMap[ui->pbtn_numl] = KeyEntry{ "l", "L", "'" };

			keyMap[ui->pbtn_numz] = KeyEntry{ "z", "Z", "/" };
			keyMap[ui->pbtn_numx] = KeyEntry{ "x", "X", "-" };
			keyMap[ui->pbtn_numc] = KeyEntry{ "c", "C", "_" };
			keyMap[ui->pbtn_numv] = KeyEntry{ "v", "V", "<" };
			keyMap[ui->pbtn_numb] = KeyEntry{ "b", "B", ">" };
			keyMap[ui->pbtn_numn] = KeyEntry{ "n", "N", "=" };
			keyMap[ui->pbtn_numm] = KeyEntry{ "m", "M", "+" };

			keyMap[ui->pbtn_numdunhao] = KeyEntry{ "、", "、", "," };
			keyMap[ui->pbtn_numjuhao] = KeyEntry{ "。", "。", "." };
			keyMap[ui->pbtn_daxie] = KeyEntry{ "abc", "ABC", " " };
			keyMap[ui->pbtn_qiehuanfuhao] = KeyEntry{ "?#&&", "?#&&", "abc" };
		}

		void FullKeyboard::handleButtonClicked()
		{
			QPushButton* button = qobject_cast<QPushButton*>(sender());
			if (button && keyMap.contains(button)) {
				appendCharacter(button);
			}
		}
	}
}


