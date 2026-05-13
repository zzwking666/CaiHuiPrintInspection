#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class FullKeyBoardClass; };
QT_END_NAMESPACE

// 输入模式
enum InputMode {
	LowerCase,
	UpperCase,
	Symbol
};

// 不同按钮在各个模式的内容
struct KeyEntry {
	QString lower;
	QString upper;
	QString symbol;
};

class FullKeyBoard : public QDialog
{
	Q_OBJECT

public:
	FullKeyBoard(QWidget* parent = nullptr);
	~FullKeyBoard();
private:
	QString value;
	// 记录当前的输入模式
	InputMode currentMode{ LowerCase };
	// 每个按钮对应的不同模式下的内容
	QMap<QPushButton*, KeyEntry> keyMap;
public:
	QString getValue() const;
private:
	void build_ui();
	void build_connect();
protected:
	void showEvent(QShowEvent*) override;
private:
	void appendCharacter(QPushButton* button);
	void appendCharacter(const QString& character);
	void updateButtonTexts();
	bool eventFilter(QObject* obj, QEvent* event);
	void initializeKeyMap();
	void handleButtonClicked();

private slots:
	void pbtn_num1_clicked();
	void pbtn_num2_clicked();
	void pbtn_num3_clicked();
	void pbtn_num4_clicked();
	void pbtn_num5_clicked();
	void pbtn_num6_clicked();
	void pbtn_num7_clicked();
	void pbtn_num8_clicked();
	void pbtn_num9_clicked();
	void pbtn_num0_clicked();
	void pbtn_cancel_clicked();

	void pbtn_numfenhao_clicked();
	void pbtn_daxie_clicked();
	void pbtn_numjianhao_clicked();
	void pbtn_huiche_clicked();

	void pbtn_kongge_clicked();
	void pbtn_qiehuanfuhao_clicked();
	void pbtn_zuo_clicked();
	void pbtn_you_clicked();
	void pbtn_shanchu_clicked();

	void onLineEditTextChanged(const QString& text);

private:
	Ui::FullKeyBoardClass* ui;
};
