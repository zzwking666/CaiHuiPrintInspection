#include"rqw_MonitorMotionIO.hpp"

#include<QSet>

namespace rw
{
	namespace rqw
	{
		MonitorZMotionIOStateThread::MonitorZMotionIOStateThread(QObject* parent)
			: QThread(parent) {
		}

		void MonitorZMotionIOStateThread::setRunning(bool running)
		{
			QMutexLocker locker(&m_mutex);
			m_running = running;
			if (m_running)
				m_waitCond.wakeAll();
		}

		void MonitorZMotionIOStateThread::destroyThread()
		{
			{
				QMutexLocker locker(&m_mutex);
				m_exit = true;
				m_waitCond.wakeAll();
			}
			wait();
		}

		void MonitorZMotionIOStateThread::setMonitorFrequency(unsigned long ms)
		{
			_monitorFrequency = ms;
		}

		void MonitorZMotionIOStateThread::setMonitorObject(ZMotion& zMotion)
		{
			_monitorObject = &zMotion;
		}

		void MonitorZMotionIOStateThread::setMonitorIList(const QVector<size_t>& monitorIList)
		{
			QMutexLocker locker(&m_mutex); // 加锁
			QSet<size_t> uniqueSet(monitorIList.begin(), monitorIList.end());
			_monitorIList = QVector<size_t>(uniqueSet.begin(), uniqueSet.end());
		}

		void MonitorZMotionIOStateThread::setMonitorOList(const QVector<size_t>& monitorOList)
		{
			QMutexLocker locker(&m_mutex); // 加锁
			QSet<size_t> uniqueSet(monitorOList.begin(), monitorOList.end());
			_monitorOList = QVector<size_t>(uniqueSet.begin(), uniqueSet.end());
		}

		void MonitorZMotionIOStateThread::setMonitorIOList(const QVector<size_t>& monitorIList,
			const QVector<size_t>& monitorOList)
		{
			QMutexLocker locker(&m_mutex); // 加锁
			_monitorIList = monitorIList;
			_monitorOList = monitorOList;
		}

		void MonitorZMotionIOStateThread::run()
		{
			while (true) {
				m_mutex.lock();
				if (m_exit) {
					m_mutex.unlock();
					break;
				}
				if (!m_running) {
					m_waitCond.wait(&m_mutex);
					if (m_exit) {
						m_mutex.unlock();
						break;
					}
				}
				m_mutex.unlock();

				monitorDIState();
				monitorDOState();

				msleep(_monitorFrequency);
			}
		}

		void MonitorZMotionIOStateThread::monitorDIState()
		{
			QVector<size_t> monitorIListCopy;
			{
				QMutexLocker locker(&m_mutex); // 加锁
				monitorIListCopy = _monitorIList; // 复制 _monitorIList
			}

			if (!_monitorObject)
			{
				return;
			}

			for (const auto& item : monitorIListCopy)
			{
				emit DIState(item, _monitorObject->getIOIn(item));
			}
		}

		void MonitorZMotionIOStateThread::monitorDOState()
		{
			QVector<size_t> monitorOListCopy;
			{
				QMutexLocker locker(&m_mutex); // 加锁
				monitorOListCopy = _monitorOList; // 复制 _monitorOList
			}

			if (!_monitorObject)
			{
				return;
			}

			for (const auto& item : monitorOListCopy)
			{
				emit DOState(item, _monitorObject->getIOOut(item));
			}
		}
	}
}