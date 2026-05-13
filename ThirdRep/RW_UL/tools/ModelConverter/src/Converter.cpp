#include"Converter.hpp"

void Converter::cancel()
{
	if (_processExportToEngine)
	{
		_processExportToEngine->kill();
		_processExportToEngine->waitForFinished();
	}
	if (isRunning())
	{
		wait();
	}
	if (_processExportToEngine)
	{
		delete _processExportToEngine;
	}
	_processExportToEngine = nullptr;
}

Converter::Converter(QObject* parent) : QThread(parent)
{
	_processExportToEngine = new QProcess(this);
	connect(_processExportToEngine, &QProcess::readyReadStandardOutput,
		this, &Converter::handleOutput);
	connect(_processExportToEngine, &QProcess::readyReadStandardError,
		this, &Converter::handleError);
	connect(_processExportToEngine, &QProcess::finished,
		this, &Converter::handleFinished);
}

Converter::~Converter()
{
}

void Converter::run()
{
	std::string str = R"(.\trtexec.exe --onnx=)";
	str += inputFile.toStdString();
	str += R"( --saveEngine=)";
	str += outputFile.toStdString();

	switch (_convertPolicy)
	{
	case ConvertPolicy::defaultPolicy:
		break;
	case ConvertPolicy::fp16:
		str += " --fp16";
		break;
	case ConvertPolicy::bf16:
		str += " --bf16";
		break;
	case ConvertPolicy::int8:
		str += " --int8";
		break;
	case ConvertPolicy::fp8:
		str += " --fp8";
		break;
	case ConvertPolicy::int4:
		str += " --int4";
		break;
	case ConvertPolicy::best:
		str += " --best";
		break;
	default:
		str += " --defaultPolicy";
		break;
	}
	if (!extraPar.isEmpty())
	{
		str += " " + extraPar.toStdString();
	}
	_processExportToEngine->start("cmd.exe", { "/c",str.c_str() });

	exec();
}

void Converter::handleOutput()
{
	QByteArray output = _processExportToEngine->readAllStandardOutput();
	QString outputStr = QString::fromLocal8Bit(output);
	emit appRunLog(outputStr); // 将输出内容发送到日志或界面
}

void Converter::handleError()
{
	QByteArray output = _processExportToEngine->readAllStandardOutput();
	QString outputStr = QString::fromLocal8Bit(output);
	emit appRunLog(outputStr); // 将输出内容发送到日志或界面
}

void Converter::handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	emit finish();
	quit();
}