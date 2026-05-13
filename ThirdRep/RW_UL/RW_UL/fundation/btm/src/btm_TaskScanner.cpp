#include "btm_TaskScanner.hpp"
#include <QTimer>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

namespace rw
{
	namespace btm
	{
		TaskScanner::TaskScanner(QObject* parent)
			: QObject(parent)
		{
			m_scanTimer = new QTimer(this);
			connect(m_scanTimer, &QTimer::timeout, this, [this]() {
				scanForTaskConfigs();
				});

			m_watcher = new QFileSystemWatcher(this);
			connect(m_watcher, &QFileSystemWatcher::directoryChanged,
				this, [this](const QString& path) {
					qInfo() << "[TaskScanner] Directory changed:" << path;
					scanForTaskConfigs();
				});

			qDebug() << "[TaskScanner] Initialized";
		}

		void TaskScanner::addMonitorDirectory(const QString& path)
		{
			if (m_monitorDirectories.contains(path))
			{
				qWarning() << "[TaskScanner] Directory already monitored:" << path;
				return;
			}

			QDir dir(path);
			if (!dir.exists())
			{
				qWarning() << "[TaskScanner] Directory does not exist:" << path;
				return;
			}

			m_monitorDirectories.append(path);
			m_watcher->addPath(path);

			qInfo() << "[TaskScanner] Added monitor directory:" << path;

			// 立即扫描
			scanForTaskConfigs();
		}

		void TaskScanner::removeMonitorDirectory(const QString& path)
		{
			if (!m_monitorDirectories.contains(path))
			{
				qWarning() << "[TaskScanner] Directory not monitored:" << path;
				return;
			}

			m_monitorDirectories.removeAll(path);
			m_watcher->removePath(path);

			qInfo() << "[TaskScanner] Removed monitor directory:" << path;
		}

		QList<QString> TaskScanner::scanForTaskConfigs()
		{
			QList<QString> foundConfigs;
			QSet<QString> previousConfigs;

			// 保存之前发现的配置
			static QSet<QString> s_knownConfigs;
			previousConfigs = s_knownConfigs;
			s_knownConfigs.clear();

			for (const QString& dirPath : m_monitorDirectories)
			{
				QDirIterator it(dirPath,
					QStringList() << "task.json" << "task.txt",
					QDir::Files,
					QDirIterator::Subdirectories);

				while (it.hasNext())
				{
					QString configPath = it.next();
					foundConfigs.append(configPath);
					s_knownConfigs.insert(configPath);

					// 如果是新发现的配置
					if (!previousConfigs.contains(configPath))
					{
						qInfo() << "[TaskScanner] Discovered new task config:" << configPath;
						emit taskDiscovered(configPath);
					}
				}
			}

			// 检查被移除的配置
			for (const QString& oldConfig : previousConfigs)
			{
				if (!s_knownConfigs.contains(oldConfig))
				{
					qInfo() << "[TaskScanner] Task config removed:" << oldConfig;
					emit taskRemoved(oldConfig);
				}
			}

			if (foundConfigs.isEmpty())
			{
				qDebug() << "[TaskScanner] No task configs found";
			}
			else
			{
				qInfo() << "[TaskScanner] Found" << foundConfigs.size() << "task configs";
			}

			return foundConfigs;
		}

		void TaskScanner::startAutoScan(int intervalMs)
		{
			if (m_scanTimer->isActive())
			{
				qWarning() << "[TaskScanner] Auto-scan already active";
				return;
			}

			m_scanTimer->start(intervalMs);
			qInfo() << "[TaskScanner] Auto-scan started, interval:" << intervalMs << "ms";
		}

		void TaskScanner::stopAutoScan()
		{
			if (!m_scanTimer->isActive())
			{
				qWarning() << "[TaskScanner] Auto-scan not active";
				return;
			}

			m_scanTimer->stop();
			qInfo() << "[TaskScanner] Auto-scan stopped";
		}

	} // namespace btm
} // namespace rw