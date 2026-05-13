#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgShutdownWarnClass; };
QT_END_NAMESPACE

class DlgShutdownWarn : public QDialog
{
	Q_OBJECT

public:
	DlgShutdownWarn(QWidget* parent = nullptr);
	~DlgShutdownWarn();

private:
	void build_ui();
	void build_connect();
public:
	void setTimeValue(size_t s);

private:
	Ui::DlgShutdownWarnClass* ui;
};
