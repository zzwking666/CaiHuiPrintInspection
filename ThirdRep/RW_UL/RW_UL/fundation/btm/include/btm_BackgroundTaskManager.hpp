#pragma once

#include "btm_ConfigLoader.hpp"
#include "btm_IdleDetector.hpp"
#include "btm_TaskScanner.hpp"
#include "btm_TaskScheduler.hpp"
#include"btm_Types.hpp"

namespace rw
{
	namespace btm
	{
        class BackgroundTaskManager : public QObject {
            Q_OBJECT
        public:
            explicit BackgroundTaskManager(QObject* parent = nullptr);
            ~BackgroundTaskManager();

            // 目录管理
            bool addMonitorDirectory(const QString& path);
            bool removeMonitorDirectory(const QString& path);

            // 任务管理
            size_t scanAndLoadTasks();
            bool addTask(const QString& configPath);
            bool removeTask(const QString& taskId);
            bool executeTask(const QString& taskId);
            bool terminateTask(const QString& taskId);

            // 系统控制
            bool start();
            bool stop();

            // 空闲检测
            void setIdleDetector(std::function<bool()> detector);

            // 回调设置
            void setOnTaskStarted(std::function<void(const TaskRuntime&)> callback);
            void setOnTaskCompleted(std::function<void(const TaskRuntime&)> callback);
            void setOnTaskFailed(std::function<void(const TaskRuntime&)> callback);

            // 查询接口
            QList<TaskRuntime> getAllTasks() const;
            TaskRuntime getTask(const QString& taskId) const;

        signals:
            void taskAdded(const QString& taskId);
            void taskRemoved(const QString& taskId);
            void taskStarted(const QString& taskId);
            void taskCompleted(const QString& taskId, int exitCode);
            void taskFailed(const QString& taskId, const QString& error);

        private:
            std::unique_ptr<ProcessManager> m_processManager;
            std::unique_ptr<TaskScanner> m_taskScanner;
            std::unique_ptr<ConfigLoader> m_configLoader;
            std::unique_ptr<TaskScheduler> m_taskScheduler;
            std::unique_ptr<IdleDetector> m_idleDetector;
            std::unique_ptr<ResourceMonitor> m_resourceMonitor;
        };
	}

}
