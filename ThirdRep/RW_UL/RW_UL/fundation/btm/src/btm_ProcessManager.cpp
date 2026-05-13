#include "btm_ProcessManager.hpp"
#include <QDebug>
#include <QTimer>

namespace rw
{
	namespace btm
	{
		ProcessManager::ProcessManager(QObject* parent)
			: QObject(parent)
		{
			qDebug() << "[ProcessManager] Initialized";
		}

		ProcessManager::~ProcessManager()
		{
			// 清理所有进程
			for (auto& [pid, entry] : m_processes)
			{
				if (entry.process && entry.process->state() != QProcess::NotRunning)
				{
					qWarning() << "[ProcessManager] Terminating process" << pid << "on destruction";
					entry.process->terminate();
					if (!entry.process->waitForFinished(3000))
					{
						entry.process->kill();
					}
				}
			}
			m_processes.clear();
		}

		qint64 ProcessManager::startProcess(const TaskConfig& config)
		{
			qint64 pid = m_nextPid++;

			ProcessEntry entry;
			entry.process = std::make_unique<QProcess>();
			entry.config = config;
			entry.startTime = QDateTime::currentDateTime();

			// 设置工作目录
			if (!config.workingDirectory.isEmpty())
			{
				entry.process->setWorkingDirectory(config.workingDirectory);
			}

			// 设置环境变量
			if (!config.environment.isEmpty())
			{
				entry.process->setProcessEnvironment(config.environment);
			}

			// 连接信号槽 - 启动
			connect(entry.process.get(), &QProcess::started,
				this, [this, pid]() {
					qInfo() << "[ProcessManager] Process started:" << pid;
					emit processStarted(pid);
				});

			// 连接信号槽 - 完成
			connect(entry.process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
				this, [this, pid](int exitCode, QProcess::ExitStatus exitStatus) {
					qInfo() << "[ProcessManager] Process finished:" << pid
						<< "exitCode:" << exitCode
						<< "status:" << (exitStatus == QProcess::NormalExit ? "Normal" : "Crashed");
					emit processFinished(pid, exitCode);
				});

			// 连接信号槽 - 错误
			connect(entry.process.get(), &QProcess::errorOccurred,
				this, [this, pid](QProcess::ProcessError error) {
					qWarning() << "[ProcessManager] Process error:" << pid << "error:" << error;
					emit processError(pid, error);
				});

			// 连接信号槽 - 标准输出
			connect(entry.process.get(), &QProcess::readyReadStandardOutput,
				this, [this, pid]() {
					auto it = m_processes.find(pid);
					if (it != m_processes.end() && it->second.process)
					{
						QString output = QString::fromLocal8Bit(it->second.process->readAllStandardOutput());
						if (!output.isEmpty())
						{
							emit standardOutput(pid, output);
						}
					}
				});

			// 连接信号槽 - 标准错误
			connect(entry.process.get(), &QProcess::readyReadStandardError,
				this, [this, pid]() {
					auto it = m_processes.find(pid);
					if (it != m_processes.end() && it->second.process)
					{
						QString error = QString::fromLocal8Bit(it->second.process->readAllStandardError());
						if (!error.isEmpty())
						{
							emit standardError(pid, error);
						}
					}
				});

			// 启动进程
			qInfo() << "[ProcessManager] Starting process:" << pid
				<< "executable:" << config.executable
				<< "args:" << config.arguments;

			entry.process->start(config.executable, config.arguments);

			// 保存进程信息
			m_processes[pid] = std::move(entry);

			// 等待启动
			if (!m_processes[pid].process->waitForStarted(5000))
			{
				qWarning() << "[ProcessManager] Failed to start process:" << pid;
				m_processes.erase(pid);
				return -1;
			}

			return pid;
		}

		bool ProcessManager::terminateProcess(qint64 pid, bool force)
		{
			auto it = m_processes.find(pid);
			if (it == m_processes.end())
			{
				qWarning() << "[ProcessManager] Process not found:" << pid;
				return false;
			}

			auto& entry = it->second;
			if (!entry.process || entry.process->state() == QProcess::NotRunning)
			{
				qWarning() << "[ProcessManager] Process not running:" << pid;
				return false;
			}

			qInfo() << "[ProcessManager] Terminating process:" << pid << "force:" << force;

			if (force)
			{
				entry.process->kill();
			}
			else
			{
				entry.process->terminate();
				if (!entry.process->waitForFinished(3000))
				{
					qWarning() << "[ProcessManager] Terminate timeout, killing process:" << pid;
					entry.process->kill();
				}
			}

			return true;
		}

		bool ProcessManager::isProcessRunning(qint64 pid) const
		{
			auto it = m_processes.find(pid);
			if (it == m_processes.end())
			{
				return false;
			}
			return it->second.process &&
				it->second.process->state() == QProcess::Running;
		}

		int ProcessManager::getProcessCpuUsage(qint64 pid) const
		{
			// TODO: 实现CPU使用率监控
			// 需要平台相关代码(Windows: PDH, Linux: /proc)
			Q_UNUSED(pid);
			return 0;
		}

		qint64 ProcessManager::getProcessMemoryUsage(qint64 pid) const
		{
			// TODO: 实现内存使用监控
			// 需要平台相关代码
			Q_UNUSED(pid);
			return 0;
		}

	} // namespace btm
} // namespace rw