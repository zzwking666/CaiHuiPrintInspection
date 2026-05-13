#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgVersionClass; };
QT_END_NAMESPACE

class DlgVersion : public QMainWindow
{
	Q_OBJECT

public:
	DlgVersion(QWidget* parent = nullptr);
	~DlgVersion();

	void loadVersionPath(QString path);
	void build_connect();

public:
	QString versionPath;

protected:
	void showEvent(QShowEvent* event) override;

private slots:
	void pbtn_exit_clicked();
	void pbtn_bigger_clicked();
	void pbtn_smaller_clicked();

private:
	Ui::DlgVersionClass* ui;
};
