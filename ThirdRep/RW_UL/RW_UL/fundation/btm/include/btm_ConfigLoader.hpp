#pragma once
#include "btm_Types.hpp"

#include <QJsonObject>



namespace rw
{
	namespace btm
	{
        class ConfigLoader {
        public:
            // 加载配置
            static TaskConfig loadFromFile(const QString& filePath);
            static TaskConfig loadFromJson(const QJsonObject& json);

            // 保存配置
            static bool saveToFile(const TaskConfig& config, const QString& filePath);

            // 验证配置
            static bool validateConfig(const TaskConfig& config, QString& errorMsg);

        private:
            static QProcessEnvironment parseEnvironment(const QJsonObject& envObj);
        };
	}

}
