#pragma once

#include "btm_Types.hpp"


namespace rw
{
	namespace btm
	{
        class ProcessManager : public QObject {
            Q_OBJECT
        public:
            explicit ProcessManager(QObject* parent = nullptr);
            ~ProcessManager();

            // 进程控制
            qint64 startProcess(const TaskConfig& config);
            bool terminateProcess(qint64 pid, bool force = false);
            bool isProcessRunning(qint64 pid) const;

            // 资源管理
            int getProcessCpuUsage(qint64 pid) const;
            qint64 getProcessMemoryUsage(qint64 pid) const;

        signals:
            void processStarted(qint64 pid);
            void processFinished(qint64 pid, int exitCode);
            void processError(qint64 pid, QProcess::ProcessError error);
            void standardOutput(qint64 pid, const QString& output);
            void standardError(qint64 pid, const QString& error);

        private:
            struct ProcessEntry {
                std::unique_ptr<QProcess> process;
                TaskConfig config;
                QDateTime startTime;
            };

            std::map<qint64, ProcessEntry> m_processes;
            qint64 m_nextPid{ 1 };
        };
	}

}
