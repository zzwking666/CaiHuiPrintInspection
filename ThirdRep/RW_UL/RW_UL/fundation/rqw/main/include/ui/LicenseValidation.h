#pragma once

#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LicenseValidationClass; };
QT_END_NAMESPACE

class FullKeyBoard;
class LicenseValidation : public QDialog
{
	Q_OBJECT
private:
	FullKeyBoard* fullKeyBoard = nullptr;
public:
	LicenseValidation(QWidget* parent = nullptr);
	~LicenseValidation();
private:
	QString hwid{};
public:
	QString getHWID() const;
	void setHWID(const QString& value);
	QString getInputValue()const;
private:
	void build_ui();
	void build_connect();
private:
	Ui::LicenseValidationClass* ui;
public slots:
	void pbtn_exit_clicked();
	void pbtn_activative_clicked();
	void pbtn_ok_clicked();
	void pbtn_activationCode_clicked();
};
