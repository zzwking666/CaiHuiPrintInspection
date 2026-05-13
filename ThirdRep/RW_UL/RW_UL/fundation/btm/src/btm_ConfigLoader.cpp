#include "btm_ConfigLoader.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QFileInfo>

namespace rw
{
	namespace btm
	{
		TaskConfig ConfigLoader::loadFromFile(const QString& filePath)
		{
			QFile file(filePath);
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				qWarning() << "[ConfigLoader] Failed to open file:" << filePath;
				throw std::runtime_error("Failed to open config file: " + filePath.toStdString());
			}

			QByteArray data = file.readAll();
			file.close();

			QJsonParseError parseError;
			QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

			if (parseError.error != QJsonParseError::NoError)
			{
				qWarning() << "[ConfigLoader] JSON parse error:" << parseError.errorString();
				throw std::runtime_error("JSON parse error: " + parseError.errorString().toStdString());
			}

			if (!doc.isObject())
			{
				throw std::runtime_error("Invalid JSON format: root must be an object");
			}

			return loadFromJson(doc.object());
		}

		TaskConfig ConfigLoader::loadFromJson(const QJsonObject& json)
		{
			TaskConfig config;

			// 解析 task_info
			if (json.contains("task_info") && json["task_info"].isObject())
			{
				QJsonObject taskInfo = json["task_info"].toObject();
				config.name = taskInfo["name"].toString();
				config.description = taskInfo["description"].toString();
				config.version = taskInfo["version"].toString();
				config.priority = static_cast<TaskPriority>(taskInfo.value("priority").toInt(5));
				config.enabled = taskInfo.value("enabled").toBool(true);
			}

			// 解析 execution
			if (json.contains("execution") && json["execution"].isObject())
			{
				QJsonObject execution = json["execution"].toObject();
				config.executable = execution["executable"].toString();
				config.workingDirectory = execution.value("working_directory").toString();

				// 解析参数
				if (execution.contains("arguments") && execution["arguments"].isArray())
				{
					QJsonArray argsArray = execution["arguments"].toArray();
					for (const QJsonValue& arg : argsArray)
					{
						config.arguments.append(arg.toString());
					}
				}

				// 解析环境变量
				if (execution.contains("environment") && execution["environment"].isObject())
				{
					config.environment = parseEnvironment(execution["environment"].toObject());
				}
			}

			// 解析 scheduling
			if (json.contains("scheduling") && json["scheduling"].isObject())
			{
				QJsonObject scheduling = json["scheduling"].toObject();

				QString triggerMode = scheduling.value("trigger_mode").toString("idle");
				if (triggerMode == "manual")
					config.triggerMode = TriggerMode::Manual;
				else if (triggerMode == "idle")
					config.triggerMode = TriggerMode::Idle;
				else if (triggerMode == "scheduled")
					config.triggerMode = TriggerMode::Scheduled;
				else if (triggerMode == "event")
					config.triggerMode = TriggerMode::Event;

				config.maxExecutionTimeSeconds = scheduling.value("max_execution_time_seconds").toInt(3600);
				config.retryOnFailure = scheduling.value("retry_on_failure").toBool(false);
				config.maxRetryCount = scheduling.value("max_retry_count").toInt(3);
			}

			// 解析 resources
			if (json.contains("resources") && json["resources"].isObject())
			{
				QJsonObject resources = json["resources"].toObject();
				config.maxCpuPercent = resources.value("max_cpu_percent").toInt(80);
				config.maxMemoryMB = resources.value("max_memory_mb").toInt(1024);
			}

			return config;
		}

		bool ConfigLoader::saveToFile(const TaskConfig& config, const QString& filePath)
		{
			QJsonObject root;

			// task_info
			QJsonObject taskInfo;
			taskInfo["name"] = config.name;
			taskInfo["description"] = config.description;
			taskInfo["version"] = config.version;
			taskInfo["priority"] = static_cast<int>(config.priority);
			taskInfo["enabled"] = config.enabled;
			root["task_info"] = taskInfo;

			// execution
			QJsonObject execution;
			execution["executable"] = config.executable;
			execution["working_directory"] = config.workingDirectory;

			QJsonArray argsArray;
			for (const QString& arg : config.arguments)
			{
				argsArray.append(arg);
			}
			execution["arguments"] = argsArray;

			QJsonObject envObj;
			QStringList keys = config.environment.keys();
			for (const QString& key : keys)
			{
				envObj[key] = config.environment.value(key);
			}
			execution["environment"] = envObj;

			root["execution"] = execution;

			// scheduling
			QJsonObject scheduling;
			QString triggerModeStr;
			switch (config.triggerMode)
			{
			case TriggerMode::Manual: triggerModeStr = "manual"; break;
			case TriggerMode::Idle: triggerModeStr = "idle"; break;
			case TriggerMode::Scheduled: triggerModeStr = "scheduled"; break;
			case TriggerMode::Event: triggerModeStr = "event"; break;
			}
			scheduling["trigger_mode"] = triggerModeStr;
			scheduling["max_execution_time_seconds"] = config.maxExecutionTimeSeconds;
			scheduling["retry_on_failure"] = config.retryOnFailure;
			scheduling["max_retry_count"] = config.maxRetryCount;
			root["scheduling"] = scheduling;

			// resources
			QJsonObject resources;
			resources["max_cpu_percent"] = config.maxCpuPercent;
			resources["max_memory_mb"] = config.maxMemoryMB;
			root["resources"] = resources;

			// 写入文件
			QJsonDocument doc(root);
			QFile file(filePath);
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				qWarning() << "[ConfigLoader] Failed to open file for writing:" << filePath;
				return false;
			}

			file.write(doc.toJson(QJsonDocument::Indented));
			file.close();

			qInfo() << "[ConfigLoader] Config saved to:" << filePath;
			return true;
		}

		bool ConfigLoader::validateConfig(const TaskConfig& config, QString& errorMsg)
		{
			// 验证必填字段
			if (config.name.isEmpty())
			{
				errorMsg = "Task name is required";
				return false;
			}

			if (config.executable.isEmpty())
			{
				errorMsg = "Executable is required";
				return false;
			}

			// 验证可执行文件存在性
			QFileInfo fileInfo(config.executable);
			if (!fileInfo.exists())
			{
				errorMsg = "Executable file does not exist: " + config.executable;
				return false;
			}

			// 验证数值范围
			if (config.maxExecutionTimeSeconds <= 0)
			{
				errorMsg = "Max execution time must be positive";
				return false;
			}

			if (config.maxRetryCount < 0)
			{
				errorMsg = "Max retry count must be non-negative";
				return false;
			}

			return true;
		}

		QProcessEnvironment ConfigLoader::parseEnvironment(const QJsonObject& envObj)
		{
			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

			for (auto it = envObj.begin(); it != envObj.end(); ++it)
			{
				env.insert(it.key(), it.value().toString());
			}

			return env;
		}

	} // namespace btm
} // namespace rw