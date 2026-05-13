#include "conMon_ConditionMonitor.hpp"

#include <QCoreApplication>
#include <QMetaObject>
#include <QVector>

namespace rw
{
	namespace conMon
	{
		// ========== ConditionMonitorWorker 实现 ==========

		ConditionMonitorWorker::ConditionMonitorWorker(QObject* parent)
			: QObject(parent)
		{
		}

		void ConditionMonitorWorker::setItems(std::unordered_map<int, ConditionItem>* items, QMutex* mutex)
		{
			_items = items;
			_mutex = mutex;
		}

		void ConditionMonitorWorker::setInterval(uint32_t intervalMs)
		{
			_intervalMs = intervalMs;
		}

		void ConditionMonitorWorker::startTimer()
		{
			if (_timer) return;

			_timer = std::make_unique<QTimer>();
			_timer->setInterval(static_cast<int>(_intervalMs));
			connect(_timer.get(), &QTimer::timeout, this, &ConditionMonitorWorker::onTimeout);
			_timer->start();
		}

		void ConditionMonitorWorker::stopTimer()
		{
			if (_timer) {
				_timer->stop();
				_timer.reset();
			}
		}

		void ConditionMonitorWorker::onTimeout()
		{
			if (!_items || !_mutex) return;

			struct WorkItem
			{
				ConditionInfo info;
				bool trigger;
				ConditionCall call;
			};

			struct PeriodicWorkItem
			{
				ConditionInfo info;
				bool isTriggered;
				std::function<void(const ConditionInfo&)> callback;
			};

			QVector<WorkItem> works;
			QVector<PeriodicWorkItem> periodicWorks;
			auto now = QDateTime::currentDateTimeUtc();

			{
				QMutexLocker locker(_mutex);
				for (auto& kv : *_items) {
					auto& item = kv.second;
					if (!item.set.enabled) {
						continue;
					}

					// 新增:检查是否需要在本次检测该条件
					int effectiveIntervalMs = item.set.intervalMs;
					if (effectiveIntervalMs <= 0 || effectiveIntervalMs < static_cast<int>(_intervalMs)) {
						effectiveIntervalMs = static_cast<int>(_intervalMs);
					}

					// 检查是否到了检测时间
					bool shouldCheck = true;
					if (item.status.lastCheckTime.isValid()) {
						qint64 timeSinceLastCheck = item.status.lastCheckTime.msecsTo(now);
						if (timeSinceLastCheck < effectiveIntervalMs) {
							shouldCheck = false;
						}
					}

					// 执行条件检测
					if (shouldCheck) {
						// 更新检测时间
						item.status.lastCheckTime = now;

						bool cond = false;
						if (item.call.condition) {
							try {
								cond = item.call.condition(item.info);
							}
							catch (...) {
								cond = false;
							}
						}

						// 状态转换逻辑
						if (cond && !item.status.triggered) {
							// debounceMs 为 0 时立即触发
							if (item.set.debounceMs <= 0) {
								item.status.triggered = true;
								item.status.lastChangeTime = now;
								item.status.lastTriggeredCallTime = now;
								item.status.lastNotTriggeredCallTime = QDateTime();
								works.push_back({ item.info, true, item.call });
							}
							else {
								if (!item.status.lastChangeTime.isValid()) {
									item.status.lastChangeTime = now;
								}
								else {
									qint64 elapsed = item.status.lastChangeTime.msecsTo(now);
									if (elapsed >= item.set.debounceMs) {
										item.status.triggered = true;
										item.status.lastChangeTime = now;
										item.status.lastTriggeredCallTime = now;
										item.status.lastNotTriggeredCallTime = QDateTime();
										works.push_back({ item.info, true, item.call });
									}
								}
							}
						}
						else if (!cond && !item.status.triggered) {
							// 条件为false且未触发时,重置debounce计时
							item.status.lastChangeTime = QDateTime();
						}
						else if (!cond && item.status.triggered) {
							// clearDebounceMs 为 0 时立即清除
							if (item.set.clearDebounceMs <= 0) {
								item.status.triggered = false;
								item.status.lastChangeTime = QDateTime();
								item.status.lastNotTriggeredCallTime = now;
								item.status.lastTriggeredCallTime = QDateTime();
								works.push_back({ item.info, false, item.call });
							}
							else {
								if (!item.status.lastChangeTime.isValid()) {
									item.status.lastChangeTime = now;
								}
								else {
									qint64 elapsed = item.status.lastChangeTime.msecsTo(now);
									if (elapsed >= item.set.clearDebounceMs) {
										item.status.triggered = false;
										item.status.lastChangeTime = QDateTime();
										item.status.lastNotTriggeredCallTime = now;
										item.status.lastTriggeredCallTime = QDateTime();
										works.push_back({ item.info, false, item.call });
									}
								}
							}
						}
					}

					// 周期性回调逻辑(不受 intervalMs 影响)
					if (item.status.triggered) {
						if (item.set.triggeredPeriodicMs > 0 && item.call.onTriggeredPeriodic) {
							if (!item.status.lastTriggeredCallTime.isValid()) {
								item.status.lastTriggeredCallTime = now;
								periodicWorks.push_back({ item.info, true, item.call.onTriggeredPeriodic });
							}
							else {
								qint64 elapsed = item.status.lastTriggeredCallTime.msecsTo(now);
								if (elapsed >= item.set.triggeredPeriodicMs) {
									item.status.lastTriggeredCallTime = now;
									periodicWorks.push_back({ item.info, true, item.call.onTriggeredPeriodic });
								}
							}
						}
					}
					else {
						if (item.set.notTriggeredPeriodicMs > 0 && item.call.onNotTriggeredPeriodic) {
							if (!item.status.lastNotTriggeredCallTime.isValid()) {
								item.status.lastNotTriggeredCallTime = now;
								periodicWorks.push_back({ item.info, false, item.call.onNotTriggeredPeriodic });
							}
							else {
								qint64 elapsed = item.status.lastNotTriggeredCallTime.msecsTo(now);
								if (elapsed >= item.set.notTriggeredPeriodicMs) {
									item.status.lastNotTriggeredCallTime = now;
									periodicWorks.push_back({ item.info, false, item.call.onNotTriggeredPeriodic });
								}
							}
						}
					}
				}
			}

			// 执行状态转换回调
			for (const auto& w : works) {
				if (w.trigger) {
					if (w.call.onTrigger) {
						try {
							w.call.onTrigger(w.info);
						}
						catch (...) {
						}
					}
					emit conditionTriggered(w.info);
				}
				else {
					if (w.call.onClear) {
						try {
							w.call.onClear(w.info);
						}
						catch (...) {
						}
					}
					emit conditionCleared(w.info);
				}
			}

			// 执行周期性回调并发射信号
			for (const auto& pw : periodicWorks) {
				if (pw.callback) {
					try {
						pw.callback(pw.info);
					}
					catch (...) {
					}
				}

				if (pw.isTriggered) {
					emit conditionTriggeredPeriodic(pw.info);
				}
				else {
					emit conditionNotTriggeredPeriodic(pw.info);
				}
			}
		}

		// ========== ConditionMonitor 实现 ==========

		ConditionMonitor::ConditionMonitor(const ConditionCfg& cfg, QObject* parent)
			: QObject(parent), _config(cfg), _running(false)
		{
			if (_config.async) {
				initThread();
			}
		}

		ConditionMonitor::~ConditionMonitor()
		{
			stop();
			cleanupThread();
			QMutexLocker locker(&_mutex);
			_items.clear();
		}

		void ConditionMonitor::initThread()
		{
			_workerThread = std::make_unique<QThread>();
			_worker = new ConditionMonitorWorker();
			_worker->setItems(&_items, &_mutex);
			_worker->setInterval(_config.intervalMs);

			_worker->moveToThread(_workerThread.get());

			connect(_worker, &ConditionMonitorWorker::conditionTriggered,
				this, &ConditionMonitor::conditionTriggered);
			connect(_worker, &ConditionMonitorWorker::conditionCleared,
				this, &ConditionMonitor::conditionCleared);
			connect(_worker, &ConditionMonitorWorker::conditionTriggeredPeriodic,
				this, &ConditionMonitor::conditionTriggeredPeriodic);
			connect(_worker, &ConditionMonitorWorker::conditionNotTriggeredPeriodic,
				this, &ConditionMonitor::conditionNotTriggeredPeriodic);

			_workerThread->start();
		}

		void ConditionMonitor::cleanupThread()
		{
			if (_workerThread) {
				_workerThread->quit();
				_workerThread->wait();
				_workerThread.reset();
			}
			if (_worker) {
				_worker->deleteLater();
				_worker = nullptr;
			}
		}

		bool ConditionMonitor::registerCondition(const ConditionItem& item)
		{
			QMutexLocker locker(&_mutex);
			if (item.info.id < 0) return false;
			if (_items.find(item.info.id) != _items.end()) return false;
			_items.emplace(item.info.id, item);
			return true;
		}

		bool ConditionMonitor::registerCondition(int id, const QString& name,
			std::function<bool(const ConditionInfo&)> condition,
			std::function<void(const ConditionInfo&)> onTrigger,
			std::function<void(const ConditionInfo&)> onClear,
			int debounceMs, int clearDebounceMs, bool autoClear,
			std::function<void(const ConditionInfo&)> onTriggeredPeriodic,
			std::function<void(const ConditionInfo&)> onNotTriggeredPeriodic,
			int triggeredPeriodicMs, int notTriggeredPeriodicMs, int intervalMs)
		{
			if (id < 0) return false;
			ConditionItem item;
			item.info.id = id;
			item.info.name = name;
			item.set.enabled = true;
			item.set.debounceMs = debounceMs;
			item.set.clearDebounceMs = clearDebounceMs;
			item.set.autoClear = autoClear;
			item.set.triggeredPeriodicMs = triggeredPeriodicMs;
			item.set.notTriggeredPeriodicMs = notTriggeredPeriodicMs;
			item.set.intervalMs = intervalMs;
			item.status.triggered = false;
			item.status.lastChangeTime = QDateTime();
			item.status.lastTriggeredCallTime = QDateTime();
			item.status.lastNotTriggeredCallTime = QDateTime();
			item.status.lastCheckTime = QDateTime();
			item.call.condition = std::move(condition);
			item.call.onTrigger = std::move(onTrigger);
			item.call.onClear = std::move(onClear);
			item.call.onTriggeredPeriodic = std::move(onTriggeredPeriodic);
			item.call.onNotTriggeredPeriodic = std::move(onNotTriggeredPeriodic);

			return registerCondition(item);
		}

		bool ConditionMonitor::unregisterCondition(int id)
		{
			QMutexLocker locker(&_mutex);
			auto it = _items.find(id);
			if (it == _items.end()) return false;
			_items.erase(it);
			return true;
		}

		bool ConditionMonitor::setEnabled(int id, bool enabled)
		{
			QMutexLocker locker(&_mutex);
			auto it = _items.find(id);
			if (it == _items.end()) return false;
			it->second.set.enabled = enabled;
			return true;
		}

		bool ConditionMonitor::isTriggered(int id)
		{
			QMutexLocker locker(&_mutex);
			auto it = _items.find(id);
			if (it == _items.end()) return false;
			return it->second.status.triggered;
		}

		void ConditionMonitor::start()
		{
			QMutexLocker locker(&_mutex);
			if (_running) return;

			if (_config.async) {
				QMetaObject::invokeMethod(_worker, "startTimer", Qt::QueuedConnection);
			}
			else {
				_timer = std::make_unique<QTimer>(this);
				_timer->setInterval(static_cast<int>(_config.intervalMs));
				connect(_timer.get(), &QTimer::timeout, this, &ConditionMonitor::onTimeout);
				_timer->start();
			}

			_running = true;
		}

		void ConditionMonitor::stop()
		{
			QMutexLocker locker(&_mutex);
			if (!_running) return;

			if (_config.async) {
				QMetaObject::invokeMethod(_worker, "stopTimer", Qt::BlockingQueuedConnection);
			}
			else {
				if (_timer) {
					_timer->stop();
					_timer.reset();
				}
			}

			_running = false;
		}

		QVector<int> ConditionMonitor::getRegisteredIds()
		{
			QVector<int> ids;
			QMutexLocker locker(&_mutex);
			ids.reserve(static_cast<int>(_items.size()));
			for (const auto& p : _items) {
				ids.push_back(p.first);
			}
			return ids;
		}

		void ConditionMonitor::onTimeout()
		{
			// 同步模式的实现(与 Worker 的 onTimeout 逻辑相同)
			struct WorkItem
			{
				ConditionInfo info;
				bool trigger;
				ConditionCall call;
			};

			struct PeriodicWorkItem
			{
				ConditionInfo info;
				bool isTriggered;
				std::function<void(const ConditionInfo&)> callback;
			};

			QVector<WorkItem> works;
			QVector<PeriodicWorkItem> periodicWorks;
			auto now = QDateTime::currentDateTimeUtc();

			{
				QMutexLocker locker(&_mutex);
				for (auto& kv : _items) {
					auto& item = kv.second;
					if (!item.set.enabled) {
						continue;
					}

					// 新增:检查是否需要在本次检测该条件
					int effectiveIntervalMs = item.set.intervalMs;
					if (effectiveIntervalMs <= 0 || effectiveIntervalMs < static_cast<int>(_config.intervalMs)) {
						effectiveIntervalMs = static_cast<int>(_config.intervalMs);
					}

					// 检查是否到了检测时间
					bool shouldCheck = true;
					if (item.status.lastCheckTime.isValid()) {
						qint64 timeSinceLastCheck = item.status.lastCheckTime.msecsTo(now);
						if (timeSinceLastCheck < effectiveIntervalMs) {
							shouldCheck = false;
						}
					}

					// 执行条件检测
					if (shouldCheck) {
						// 更新检测时间
						item.status.lastCheckTime = now;

						bool cond = false;
						if (item.call.condition) {
							try {
								cond = item.call.condition(item.info);
							}
							catch (...) {
								cond = false;
							}
						}

						// 状态转换逻辑
						if (cond && !item.status.triggered) {
							// debounceMs 为 0 时立即触发
							if (item.set.debounceMs <= 0) {
								item.status.triggered = true;
								item.status.lastChangeTime = now;
								item.status.lastTriggeredCallTime = now;
								item.status.lastNotTriggeredCallTime = QDateTime();
								works.push_back({ item.info, true, item.call });
							}
							else {
								if (!item.status.lastChangeTime.isValid()) {
									item.status.lastChangeTime = now;
								}
								else {
									qint64 elapsed = item.status.lastChangeTime.msecsTo(now);
									if (elapsed >= item.set.debounceMs) {
										item.status.triggered = true;
										item.status.lastChangeTime = now;
										item.status.lastTriggeredCallTime = now;
										item.status.lastNotTriggeredCallTime = QDateTime();
										works.push_back({ item.info, true, item.call });
									}
								}
							}
						}
						else if (!cond && !item.status.triggered) {
							// 条件为false且未触发时,重置debounce计时
							item.status.lastChangeTime = QDateTime();
						}
						else if (!cond && item.status.triggered) {
							// clearDebounceMs 为 0 时立即清除
							if (item.set.clearDebounceMs <= 0) {
								item.status.triggered = false;
								item.status.lastChangeTime = QDateTime();
								item.status.lastNotTriggeredCallTime = now;
								item.status.lastTriggeredCallTime = QDateTime();
								works.push_back({ item.info, false, item.call });
							}
							else {
								if (!item.status.lastChangeTime.isValid()) {
									item.status.lastChangeTime = now;
								}
								else {
									qint64 elapsed = item.status.lastChangeTime.msecsTo(now);
									if (elapsed >= item.set.clearDebounceMs) {
										item.status.triggered = false;
										item.status.lastChangeTime = QDateTime();
										item.status.lastNotTriggeredCallTime = now;
										item.status.lastTriggeredCallTime = QDateTime();
										works.push_back({ item.info, false, item.call });
									}
								}
							}
						}
					}

					// 周期性回调逻辑(不受 intervalMs 影响)
					if (item.status.triggered) {
						if (item.set.triggeredPeriodicMs > 0 && item.call.onTriggeredPeriodic) {
							if (!item.status.lastTriggeredCallTime.isValid()) {
								item.status.lastTriggeredCallTime = now;
								periodicWorks.push_back({ item.info, true, item.call.onTriggeredPeriodic });
							}
							else {
								qint64 elapsed = item.status.lastTriggeredCallTime.msecsTo(now);
								if (elapsed >= item.set.triggeredPeriodicMs) {
									item.status.lastTriggeredCallTime = now;
									periodicWorks.push_back({ item.info, true, item.call.onTriggeredPeriodic });
								}
							}
						}
					}
					else {
						if (item.set.notTriggeredPeriodicMs > 0 && item.call.onNotTriggeredPeriodic) {
							if (!item.status.lastNotTriggeredCallTime.isValid()) {
								item.status.lastNotTriggeredCallTime = now;
								periodicWorks.push_back({ item.info, false, item.call.onNotTriggeredPeriodic });
							}
							else {
								qint64 elapsed = item.status.lastNotTriggeredCallTime.msecsTo(now);
								if (elapsed >= item.set.notTriggeredPeriodicMs) {
									item.status.lastNotTriggeredCallTime = now;
									periodicWorks.push_back({ item.info, false, item.call.onNotTriggeredPeriodic });
								}
							}
						}
					}
				}
			}

			// 执行状态转换回调
			for (const auto& w : works) {
				if (w.trigger) {
					if (w.call.onTrigger) {
						try {
							w.call.onTrigger(w.info);
						}
						catch (...) {
						}
					}
					emit conditionTriggered(w.info);
				}
				else {
					if (w.call.onClear) {
						try {
							w.call.onClear(w.info);
						}
						catch (...) {
						}
					}
					emit conditionCleared(w.info);
				}
			}

			// 执行周期性回调并发射信号
			for (const auto& pw : periodicWorks) {
				if (pw.callback) {
					try {
						pw.callback(pw.info);
					}
					catch (...) {
					}
				}

				if (pw.isTriggered) {
					emit conditionTriggeredPeriodic(pw.info);
				}
				else {
					emit conditionNotTriggeredPeriodic(pw.info);
				}
			}
		}
	}
}