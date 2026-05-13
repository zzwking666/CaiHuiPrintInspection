#pragma once

#include <QThread>

#include<QProcess>

enum class ConvertPolicy
{
	defaultPolicy,
	fp16,
	bf16,
	int8,
	fp8,
	int4,
	best
};

inline const char* to_string(ConvertPolicy e)
{
	switch (e)
	{
	case ConvertPolicy::defaultPolicy: return "defaultPolicy";
	case ConvertPolicy::fp16: return "fp16";
	case ConvertPolicy::bf16: return "bf16";
	case ConvertPolicy::int8: return "int8";
	case ConvertPolicy::fp8: return "fp8";
	case ConvertPolicy::int4: return "int4";
	case ConvertPolicy::best: return "best";
	default: return "unknown";
	}
}

class Converter
	: public QThread
{
	Q_OBJECT
public:
	void cancel();
public:
	QString inputFile;
	QString outputFile;
	QString extraPar;
	ConvertPolicy _convertPolicy{ ConvertPolicy::defaultPolicy };
public:
	explicit Converter(QObject* parent = nullptr);

	~Converter() override;
private:
	QProcess* _processExportToEngine{ nullptr };
public:
	void run() override;
signals:
	void appRunLog(QString log);
	void finish();
public slots:
	void handleOutput();
	void handleError();
	void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
};