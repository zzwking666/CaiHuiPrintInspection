#pragma once

#include"btm_Types.hpp"


namespace rw
{
	namespace btm
	{
        class TaskScanner : public QObject {
            Q_OBJECT
        public:
            explicit TaskScanner(QObject* parent = nullptr);

            // 目录管理
            void addMonitorDirectory(const QString& path);
            void removeMonitorDirectory(const QString& path);

            // 扫描操作
            QList<QString> scanForTaskConfigs();
            void startAutoScan(int intervalMs = 60000);
            void stopAutoScan();

        signals:
            void taskDiscovered(const QString& configPath);
            void taskRemoved(const QString& configPath);

        private:
            QStringList m_monitorDirectories;
            QTimer* m_scanTimer{ nullptr };
            QFileSystemWatcher* m_watcher{ nullptr };
        };
	}

}
