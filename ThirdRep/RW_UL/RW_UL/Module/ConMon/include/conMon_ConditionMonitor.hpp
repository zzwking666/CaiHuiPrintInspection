#pragma once


#include"conMon_ConditionItem.hpp"

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <functional>
#include <unordered_map>
#include <memory>

namespace rw
{
	namespace conMon
	{
		

		struct ConditionCfg
		{
			bool async{ true };              // 是否异步执行条件函数
			uint32_t intervalMs{ 500 };      // 检测周期，单位 ms
		};

		// Worker 类在工作线程中运行
		class ConditionMonitorWorker : public QObject
		{
			Q_OBJECT
		public:
			explicit ConditionMonitorWorker(QObject* parent = nullptr);
			~ConditionMonitorWorker() override = default;

			void setItems(std::unordered_map<int, ConditionItem>* items, QMutex* mutex);
			void setInterval(uint32_t intervalMs);

		public slots:
			void startTimer();
			void stopTimer();

		signals:
			void conditionTriggered(ConditionInfo info);
			void conditionCleared(ConditionInfo info);
			void conditionTriggeredPeriodic(ConditionInfo info);
			void conditionNotTriggeredPeriodic(ConditionInfo info);
		private slots:
			void onTimeout();

		private:
			std::unordered_map<int, ConditionItem>* _items{ nullptr };
			QMutex* _mutex{ nullptr };
			std::unique_ptr<QTimer> _timer;
			uint32_t _intervalMs{ 500 };
		};

		class ConditionMonitor : public QObject
		{
			Q_OBJECT
		public:
			explicit ConditionMonitor(const ConditionCfg& cfg = ConditionCfg(), QObject* parent = nullptr);
			~ConditionMonitor() override;

			// 注册 / 注销
			// 如果 id 已存在返回 false
			bool registerCondition(const ConditionItem& item);
			bool registerCondition(int id,
				const QString& name,
				std::function<bool(const ConditionInfo&)> condition,
				std::function<void(const ConditionInfo&)> onTrigger = nullptr,
				std::function<void(const ConditionInfo&)> onClear = nullptr,
				int debounceMs = 500,
				int clearDebounceMs = 500,
				bool autoClear = true,
				std::function<void(const ConditionInfo&)> onTriggeredPeriodic = nullptr,
				std::function<void(const ConditionInfo&)> onNotTriggeredPeriodic = nullptr,
				int triggeredPeriodicMs = 0,
				int notTriggeredPeriodicMs = 0,
				int intervalMs = 0);

			// 注销条件
			bool unregisterCondition(int id);

			// 启用 / 禁用
			bool setEnabled(int id, bool enabled);

			// 查询当前触发状态（线程安全）
			bool isTriggered(int id);

			void start();
			void stop();

			// 获取已注册 id 列表（线程安全）
			QVector<int> getRegisteredIds();

		signals:
			// 在主线程发出，UI 或其他模块可 connect
			void conditionTriggered(ConditionInfo info);
			void conditionCleared(ConditionInfo info);
			void conditionTriggeredPeriodic(ConditionInfo info);
			void conditionNotTriggeredPeriodic(ConditionInfo info);

		private slots:
			void onTimeout();

		private:
			void initThread();
			void cleanupThread();

		private:
			QMutex _mutex;
			std::unordered_map<int, ConditionItem> _items;

			std::unique_ptr<QTimer> _timer;
			std::unique_ptr<QThread> _workerThread;
			ConditionMonitorWorker* _worker{ nullptr };
			ConditionCfg _config;
			bool _running{ false };
		};
	}
}

