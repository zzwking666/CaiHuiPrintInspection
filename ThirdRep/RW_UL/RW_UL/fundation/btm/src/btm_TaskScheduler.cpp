#include "btm_TaskScheduler.hpp"
#include <QTimer>
#include <QDebug>
#include <algorithm>

namespace rw
{
	namespace btm
	{
		TaskScheduler::TaskScheduler(ProcessManager* processManager, QObject* parent)
			: QObject(parent)
			, m_processManager(processManager)
		{
			m_idleCheckTimer = new QTimer(this);
			connect(m_idleCheckTimer, &QTimer::timeout, this, &TaskScheduler::onIdleCheckTimer);

			// 连接ProcessManager信号
			if (m_processManager)
			{
				connect(m_processManager, &ProcessManager::processFinished,
					this, [this](qint64 pid, int exitCode) {
						// 查找对应的任务
						for (auto& [taskId, runtime] : m_tasks)
						{
							if (runtime.processPid == pid)
							{
								runtime.endTime = QDateTime::currentDateTime();
								runtime.exitCode = exitCode;

								if (exitCode == 0)
								{
									runtime.status = TaskStatus::Completed;
									qInfo() << "[TaskScheduler] Task completed:" << taskId;

									// 调用成功回调
									if (runtime.config.onSuccess)
									{
										runtime.config.onSuccess(exitCode);
									}
								}
								else
								{
									runtime.status = TaskStatus::Failed;
									qWarning() << "[TaskScheduler] Task failed:" << taskId
										<< "exitCode:" << exitCode;

									// 调用失败回调
									if (runtime.config.onFailure)
									{
										runtime.config.onFailure(QString("Process exited with code %1").arg(exitCode));
									}

									// 重试逻辑
									if (runtime.config.retryOnFailure &&
										runtime.retryCount < runtime.config.maxRetryCount)
									{
										runtime.retryCount++;
										qInfo() << "[TaskScheduler] Retrying task:" << taskId
											<< "attempt:" << runtime.retryCount;

										// 延迟重试
										QTimer::singleShot(5000, this, [this, taskId]() {
											executeTask(taskId);
											});
									}
								}

								break;
							}
						}
					});

				connect(m_processManager, &ProcessManager::processError,
					this, [this](qint64 pid, QProcess::ProcessError error) {
						for (auto& [taskId, runtime] : m_tasks)
						{
							if (runtime.processPid == pid)
							{
								runtime.status = TaskStatus::Failed;
								runtime.errorMessage = QString("Process error: %1").arg(error);
								qWarning() << "[TaskScheduler] Process error for task:" << taskId
									<< "error:" << error;

								if (runtime.config.onFailure)
								{
									runtime.config.onFailure(runtime.errorMessage);
								}
								break;
							}
						}
					});
			}

			qDebug() << "[TaskScheduler] Initialized";
		}

		void TaskScheduler::addTask(const TaskRuntime& task)
		{
			if (task.id.isEmpty())
			{
				qWarning() << "[TaskScheduler] Cannot add task with empty ID";
				return;
			}

			if (m_tasks.find(task.id) != m_tasks.end())
			{
				qWarning() << "[TaskScheduler] Task already exists:" << task.id;
				return;
			}

			m_tasks[task.id] = task;
			qInfo() << "[TaskScheduler] Task added:" << task.id
				<< "priority:" << static_cast<int>(task.config.priority);

			emit taskScheduled(task.id);
		}

		void TaskScheduler::removeTask(const QString& taskId)
		{
			auto it = m_tasks.find(taskId);
			if (it == m_tasks.end())
			{
				qWarning() << "[TaskScheduler] Task not found:" << taskId;
				return;
			}

			// 如果任务正在运行,先终止
			if (it->second.status == TaskStatus::Running && it->second.processPid > 0)
			{
				m_processManager->terminateProcess(it->second.processPid, false);
			}

			m_tasks.erase(it);
			qInfo() << "[TaskScheduler] Task removed:" << taskId;
		}

		void TaskScheduler::start()
		{
			if (m_idleCheckTimer->isActive())
			{
				qWarning() << "[TaskScheduler] Already started";
				return;
			}

			// 每5秒检查一次空闲状态
			m_idleCheckTimer->start(5000);
			qInfo() << "[TaskScheduler] Scheduler started";
		}

		void TaskScheduler::stop()
		{
			if (!m_idleCheckTimer->isActive())
			{
				qWarning() << "[TaskScheduler] Not started";
				return;
			}

			m_idleCheckTimer->stop();
			qInfo() << "[TaskScheduler] Scheduler stopped";

			// 终止所有运行中的任务
			for (auto& [taskId, runtime] : m_tasks)
			{
				if (runtime.status == TaskStatus::Running && runtime.processPid > 0)
				{
					qInfo() << "[TaskScheduler] Terminating running task:" << taskId;
					m_processManager->terminateProcess(runtime.processPid, false);
				}
			}
		}

		void TaskScheduler::executeTask(const QString& taskId)
		{
			auto it = m_tasks.find(taskId);
			if (it == m_tasks.end())
			{
				qWarning() << "[TaskScheduler] Task not found:" << taskId;
				return;
			}

			TaskRuntime& runtime = it->second;

			// 检查任务是否已在运行
			if (runtime.status == TaskStatus::Running)
			{
				qWarning() << "[TaskScheduler] Task already running:" << taskId;
				return;
			}

			// 检查任务是否启用
			if (!runtime.config.enabled)
			{
				qInfo() << "[TaskScheduler] Task is disabled:" << taskId;
				return;
			}

			// 重置运行时状态(保留配置和重试计数)
			runtime.status = TaskStatus::Pending;
			runtime.startTime = QDateTime::currentDateTime();
			runtime.endTime = QDateTime();
			runtime.exitCode = 0;
			runtime.errorMessage.clear();

			qInfo() << "[TaskScheduler] Executing task:" << taskId;

			// 调用启动回调
			if (runtime.config.onStart)
			{
				runtime.config.onStart();
			}

			// 启动进程
			qint64 pid = m_processManager->startProcess(runtime.config);
			if (pid > 0)
			{
				runtime.processPid = pid;
				runtime.status = TaskStatus::Running;
				emit taskExecuted(taskId);

				// 设置超时检查
				if (runtime.config.maxExecutionTimeSeconds > 0)
				{
					QTimer::singleShot(runtime.config.maxExecutionTimeSeconds * 1000,
						this, [this, taskId, pid]() {
							checkTaskTimeout();
						});
				}
			}
			else
			{
				runtime.status = TaskStatus::Failed;
				runtime.errorMessage = "Failed to start process";
				qWarning() << "[TaskScheduler] Failed to start process for task:" << taskId;

				if (runtime.config.onFailure)
				{
					runtime.config.onFailure(runtime.errorMessage);
				}
			}
		}

		void TaskScheduler::setIdleDetector(std::function<bool()> detector)
		{
			m_idleDetector = detector;
			qDebug() << "[TaskScheduler] Idle detector set";
		}
		QList<TaskRuntime> TaskScheduler::getAllTasks() const
		{
			QList<TaskRuntime> result;
			for (const auto& [taskId, runtime] : m_tasks)
			{
				result.append(runtime);
			}
			return result;
		}

		TaskRuntime TaskScheduler::getTask(const QString& taskId) const
		{
			auto it = m_tasks.find(taskId);
			if (it != m_tasks.end())
			{
				return it->second;
			}
			return TaskRuntime();  // 返回空运行时
		}
		void TaskScheduler::onIdleCheckTimer()
		{
			// 检查是否有空闲检测器
			if (!m_idleDetector)
			{
				return;
			}

			// 检查系统是否空闲
			bool isIdle = m_idleDetector();
			if (!isIdle)
			{
				return;
			}

			// 查找需要在空闲时执行的任务
			std::vector<std::pair<QString, int>> idleTasks;
			for (const auto& [taskId, runtime] : m_tasks)
			{
				if (runtime.config.triggerMode == TriggerMode::Idle &&
					runtime.status == TaskStatus::Idle &&
					runtime.config.enabled)
				{
					idleTasks.push_back({ taskId, static_cast<int>(runtime.config.priority) });
				}
			}

			if (idleTasks.empty())
			{
				return;
			}

			// 按优先级排序(降序)
			std::sort(idleTasks.begin(), idleTasks.end(),
				[](const auto& a, const auto& b) {
					return a.second > b.second;
				});

			// 执行优先级最高的任务
			const QString& taskId = idleTasks.front().first;
			qInfo() << "[TaskScheduler] Idle detected, executing task:" << taskId
				<< "priority:" << idleTasks.front().second;
			executeTask(taskId);
		}

		void TaskScheduler::checkTaskTimeout()
		{
			QDateTime now = QDateTime::currentDateTime();

			for (auto& [taskId, runtime] : m_tasks)
			{
				if (runtime.status == TaskStatus::Running)
				{
					qint64 elapsedSeconds = runtime.startTime.secsTo(now);
					if (elapsedSeconds >= runtime.config.maxExecutionTimeSeconds)
					{
						qWarning() << "[TaskScheduler] Task timeout:" << taskId
							<< "elapsed:" << elapsedSeconds << "s";

						// 终止进程
						if (runtime.processPid > 0)
						{
							m_processManager->terminateProcess(runtime.processPid, true);
						}

						runtime.status = TaskStatus::Timeout;
						runtime.endTime = now;
						runtime.errorMessage = QString("Task timeout after %1 seconds").arg(elapsedSeconds);

						// 调用超时回调
						if (runtime.config.onTimeout)
						{
							runtime.config.onTimeout();
						}
					}
				}
			}
		}

	} // namespace btm
} // namespace rw