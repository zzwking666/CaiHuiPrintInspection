#include "btm_BackgroundTaskManager.hpp"
#include <QDebug>

namespace rw
{
	namespace btm
	{
		BackgroundTaskManager::BackgroundTaskManager(QObject* parent)
			: QObject(parent)
		{
			// 创建子模块
			m_processManager = std::make_unique<ProcessManager>(this);
			m_taskScanner = std::make_unique<TaskScanner>(this);
			m_taskScheduler = std::make_unique<TaskScheduler>(m_processManager.get(), this);

			// 连接TaskScanner信号
			connect(m_taskScanner.get(), &TaskScanner::taskDiscovered,
				this, [this](const QString& configPath) {
					addTask(configPath);
				});

			connect(m_taskScanner.get(), &TaskScanner::taskRemoved,
				this, [this](const QString& configPath) {
					// 根据配置路径查找任务ID并移除
					// 这里简化处理,实际应该维护路径到ID的映射
					qInfo() << "[BackgroundTaskManager] Task config removed:" << configPath;
				});

			// 连接TaskScheduler信号
			connect(m_taskScheduler.get(), &TaskScheduler::taskExecuted,
				this, &BackgroundTaskManager::taskStarted);

			// 连接ProcessManager信号
			connect(m_processManager.get(), &ProcessManager::processFinished,
				this, [this](qint64 pid, int exitCode) {
					// 查找对应任务并发出信号
					auto tasks = getAllTasks();
					for (const auto& task : tasks)
					{
						if (task.processPid == pid)
						{
							if (exitCode == 0)
							{
								emit taskCompleted(task.id, exitCode);
							}
							else
							{
								emit taskFailed(task.id, QString("Exit code: %1").arg(exitCode));
							}
							break;
						}
					}
				});

			qInfo() << "[BackgroundTaskManager] Initialized";
		}

		BackgroundTaskManager::~BackgroundTaskManager()
		{
			stop();
			qInfo() << "[BackgroundTaskManager] Destroyed";
		}

		bool BackgroundTaskManager::addMonitorDirectory(const QString& path)
		{
			try
			{
				m_taskScanner->addMonitorDirectory(path);
				qInfo() << "[BackgroundTaskManager] Monitor directory added:" << path;
				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to add monitor directory:"
					<< path << "error:" << e.what();
				return false;
			}
		}

		bool BackgroundTaskManager::removeMonitorDirectory(const QString& path)
		{
			try
			{
				m_taskScanner->removeMonitorDirectory(path);
				qInfo() << "[BackgroundTaskManager] Monitor directory removed:" << path;
				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to remove monitor directory:"
					<< path << "error:" << e.what();
				return false;
			}
		}

		size_t BackgroundTaskManager::scanAndLoadTasks()
		{
			QList<QString> configs = m_taskScanner->scanForTaskConfigs();
			size_t loadedCount = 0;

			for (const QString& configPath : configs)
			{
				if (addTask(configPath))
				{
					loadedCount++;
				}
			}

			qInfo() << "[BackgroundTaskManager] Scanned and loaded" << loadedCount << "tasks";
			return loadedCount;
		}

		bool BackgroundTaskManager::addTask(const QString& configPath)
		{
			try
			{
				// 加载配置
				TaskConfig config = ConfigLoader::loadFromFile(configPath);

				// 验证配置
				QString errorMsg;
				if (!ConfigLoader::validateConfig(config, errorMsg))
				{
					qWarning() << "[BackgroundTaskManager] Invalid config:" << configPath
						<< "error:" << errorMsg;
					return false;
				}

				// 创建运行时信息
				TaskRuntime runtime;
				runtime.id = config.name;  // 使用任务名称作为ID
				runtime.config = config;
				runtime.configFilePath = configPath;
				runtime.status = TaskStatus::Idle;

				// 添加到调度器
				m_taskScheduler->addTask(runtime);

				emit taskAdded(runtime.id);
				qInfo() << "[BackgroundTaskManager] Task added:" << runtime.id
					<< "from:" << configPath;

				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to add task:"
					<< configPath << "error:" << e.what();
				return false;
			}
		}

		bool BackgroundTaskManager::removeTask(const QString& taskId)
		{
			try
			{
				m_taskScheduler->removeTask(taskId);
				emit taskRemoved(taskId);
				qInfo() << "[BackgroundTaskManager] Task removed:" << taskId;
				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to remove task:"
					<< taskId << "error:" << e.what();
				return false;
			}
		}

		bool BackgroundTaskManager::executeTask(const QString& taskId)
		{
			try
			{
				m_taskScheduler->executeTask(taskId);
				qInfo() << "[BackgroundTaskManager] Task execution requested:" << taskId;
				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to execute task:"
					<< taskId << "error:" << e.what();
				return false;
			}
		}

		bool BackgroundTaskManager::terminateTask(const QString& taskId)
		{
			try
			{
				auto tasks = getAllTasks();
				for (const auto& task : tasks)
				{
					if (task.id == taskId && task.processPid > 0)
					{
						m_processManager->terminateProcess(task.processPid, false);
						qInfo() << "[BackgroundTaskManager] Task terminated:" << taskId;
						return true;
					}
				}
				qWarning() << "[BackgroundTaskManager] Task not running:" << taskId;
				return false;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to terminate task:"
					<< taskId << "error:" << e.what();
				return false;
			}
		}

		bool BackgroundTaskManager::start()
		{
			try
			{
				m_taskScheduler->start();
				qInfo() << "[BackgroundTaskManager] Started";
				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to start:" << e.what();
				return false;
			}
		}

		bool BackgroundTaskManager::stop()
		{
			try
			{
				m_taskScheduler->stop();
				qInfo() << "[BackgroundTaskManager] Stopped";
				return true;
			}
			catch (const std::exception& e)
			{
				qWarning() << "[BackgroundTaskManager] Failed to stop:" << e.what();
				return false;
			}
		}

		void BackgroundTaskManager::setIdleDetector(std::function<bool()> detector)
		{
			m_taskScheduler->setIdleDetector(detector);
			qDebug() << "[BackgroundTaskManager] Idle detector set";
		}

		void BackgroundTaskManager::setOnTaskStarted(std::function<void(const TaskRuntime&)> callback)
		{
			connect(this, &BackgroundTaskManager::taskStarted,
				this, [callback, this](const QString& taskId) {
					auto task = getTask(taskId);
					if (!task.id.isEmpty())
					{
						callback(task);
					}
				});
		}

		void BackgroundTaskManager::setOnTaskCompleted(std::function<void(const TaskRuntime&)> callback)
		{
			connect(this, &BackgroundTaskManager::taskCompleted,
				this, [callback, this](const QString& taskId, int exitCode) {
					Q_UNUSED(exitCode);
					auto task = getTask(taskId);
					if (!task.id.isEmpty())
					{
						callback(task);
					}
				});
		}

		void BackgroundTaskManager::setOnTaskFailed(std::function<void(const TaskRuntime&)> callback)
		{
			connect(this, &BackgroundTaskManager::taskFailed,
				this, [callback, this](const QString& taskId, const QString& error) {
					Q_UNUSED(error);
					auto task = getTask(taskId);
					if (!task.id.isEmpty())
					{
						callback(task);
					}
				});
		}

		QList<TaskRuntime> BackgroundTaskManager::getAllTasks() const
		{
			return m_taskScheduler->getAllTasks();
		}

		TaskRuntime BackgroundTaskManager::getTask(const QString& taskId) const
		{
			return m_taskScheduler->getTask(taskId);
		}

	} // namespace btm
} // namespace rw