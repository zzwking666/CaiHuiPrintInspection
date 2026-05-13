#pragma once

#include <QString>
#include <QStringList>
#include <QProcessEnvironment>
#include <QDateTime>
#include <QProcess>
#include <functional>
#include <map>

namespace rw
{
	namespace btm
	{
		/// \brief 任务状态枚举
		enum class TaskStatus
		{
			Idle,           ///< 空闲
			Pending,        ///< 等待执行
			Running,        ///< 运行中
			Completed,      ///< 已完成
			Failed,         ///< 失败
			Timeout,        ///< 超时
			Cancelled       ///< 已取消
		};

		/// \brief 任务优先级
		enum class TaskPriority
		{
			Low = 1,
			Normal = 5,
			High = 10
		};

		/// \brief 触发模式
		enum class TriggerMode
		{
			Manual,         ///< 手动触发
			Idle,           ///< 空闲时触发
			Scheduled,      ///< 定时触发
			Event           ///< 事件触发
		};

		/// \brief 任务配置结构
		struct TaskConfig
		{
			QString name;
			QString description;
			QString version;
			QString executable;
			QString workingDirectory;
			QStringList arguments;
			QProcessEnvironment environment;

			TriggerMode triggerMode{ TriggerMode::Idle };
			TaskPriority priority{ TaskPriority::Normal };
			int maxExecutionTimeSeconds{ 3600 };
			bool retryOnFailure{ false };
			int maxRetryCount{ 3 };
			bool enabled{ true };

			// 资源限制
			int maxCpuPercent{ 80 };
			int maxMemoryMB{ 1024 };

			// 钩子回调
			std::function<void()> onStart;
			std::function<void(int)> onSuccess;
			std::function<void(QString)> onFailure;
			std::function<void()> onTimeout;
		};

		/// \brief 任务运行时信息
		struct TaskRuntime
		{
			QString id;                             ///< 任务唯一ID (通常是任务名称)
			TaskConfig config;
			TaskStatus status{ TaskStatus::Idle };
			QDateTime startTime;
			QDateTime endTime;
			int retryCount{ 0 };
			int exitCode{ 0 };
			QString errorMessage;
			qint64 processPid{ -1 };
			QString configFilePath;                 ///< 配置文件路径

			/// \brief 重置运行时状态(保留配置)
			void reset()
			{
				status = TaskStatus::Idle;
				startTime = QDateTime();
				endTime = QDateTime();
				retryCount = 0;
				exitCode = 0;
				errorMessage.clear();
				processPid = -1;
			}
		};

	} // namespace btm
} // namespace rw