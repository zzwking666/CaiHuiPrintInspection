#pragma once

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "rqw_ZMotion.hpp"

namespace rw
{
	namespace rqw
	{
		class MonitorZMotionIOStateThread : public QThread
		{
			Q_OBJECT

		public:
			explicit MonitorZMotionIOStateThread(QObject* parent = nullptr);

			void setRunning(bool running);

			void destroyThread();
		public:
			void setMonitorFrequency(unsigned long ms);
		private:
			unsigned long _monitorFrequency{ 100 };
		public:
			void setMonitorObject(ZMotion& zMotion);
		private:
			ZMotion* _monitorObject = nullptr;
		public:
			void setMonitorIList(const QVector<size_t>& monitorIList);
			void setMonitorOList(const QVector<size_t>& monitorOList);
			void setMonitorIOList(const QVector<size_t>& monitorIList, const QVector<size_t>& monitorOList);
		private:
			QVector<size_t> _monitorIList;
			QVector<size_t> _monitorOList;
		protected:
			void run() override;
		private:
			void monitorDIState();
			void monitorDOState();
		signals:
			void DIState(size_t index, bool state);
			void DOState(size_t index, bool state);
		private:
			QMutex m_mutex;
			QWaitCondition m_waitCond;
			bool m_running{ false };
			bool m_exit{ false };
		};
	}
}