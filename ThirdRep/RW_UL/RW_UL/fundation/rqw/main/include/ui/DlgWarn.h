#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgWarnClass; };
QT_END_NAMESPACE

class DlgWarn : public QDialog
{
	Q_OBJECT

public:
	DlgWarn(QWidget* parent = nullptr);
	~DlgWarn();
private:
	void build_ui();
	void build_connect();
public:
	void setTitle(const QString& Tile);
	void setText(const QString& text);
	void setTime(const QString& time);
signals:
	void isProcess();
private:
	Ui::DlgWarnClass* ui;
private slots:
	void pbtn_ignore_clicked();
	void pbtn_accept_clicked();
};
