#pragma once

#include "btm_ProcessManager.hpp"
#include"btm_Types.hpp"


namespace rw
{
	namespace btm
	{
        class TaskScheduler : public QObject {
            Q_OBJECT
        public:
            explicit TaskScheduler(ProcessManager* processManager, QObject* parent = nullptr);

            // 任务管理
            void addTask(const TaskRuntime& task);
            void removeTask(const QString& taskId);

            // 调度控制
            void start();
            void stop();
            void executeTask(const QString& taskId);

            // 空闲检测
            void setIdleDetector(std::function<bool()> detector);

        signals:
            void taskScheduled(const QString& taskId);
            void taskExecuted(const QString& taskId);

        private slots:
            void onIdleCheckTimer();
            void checkTaskTimeout();
        public:
            // 查询接口
            QList<TaskRuntime> getAllTasks() const;
            TaskRuntime getTask(const QString& taskId) const;
        private:
            ProcessManager* m_processManager{ nullptr };
            std::map<QString, TaskRuntime> m_tasks;
            QTimer* m_idleCheckTimer{ nullptr };
            std::function<bool()> m_idleDetector;
        };
	}

}
