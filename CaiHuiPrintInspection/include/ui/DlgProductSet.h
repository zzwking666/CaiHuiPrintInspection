#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class DlgProductSetClass; };
QT_END_NAMESPACE

class DlgProductSet : public QDialog
{
	Q_OBJECT

public:
	DlgProductSet(QWidget* parent = nullptr);
	~DlgProductSet();

public:
	void build_ui();
	void read_config();
	void build_connect();

signals:
	void paramsChanged();

	void emit_changeLanguage(int index);
public slots:
	void changeLanguage(int index);
private slots:
	void btn_close_clicked();

	void btn_xiangjiguangdianpingbishijian_clicked();
	void btn_shuchuxinhaoyanshi_clicked();
	void btn_shuchuxinhaochixushijian_clicked();

	void btn_testTrigger1_1_clicked();
	void btn_testTrigger2_1_clicked();
	void btn_testTrigger1_2_clicked();
	void btn_testTrigger2_2_clicked();

	void btn_shangxianwei1_clicked();
	void btn_xiaxianwei1_clicked();
	void btn_zuoxianwei1_clicked();
	void btn_youxianwei1_clicked();
	void btn_baoguang1_clicked();
	void btn_zengyi1_clicked();
	void btn_xiangsudangliang1_clicked();
	void btn_shangxianwei2_clicked();
	void btn_xiaxianwei2_clicked();
	void btn_zuoxianwei2_clicked();
	void btn_youxianwei2_clicked();
	void btn_baoguang2_clicked();
	void btn_zengyi2_clicked();
	void btn_xiangsudangliang2_clicked();

public:
	Ui::DlgProductSetClass* ui;
};

