#pragma once
#include <QDateTime>
#include <QString>

#include <any>

namespace rw
{
	namespace conMon
	{
		// Condition 基本数据结构(用户已定义样式)
		struct ConditionInfo
		{
			int id{ -1 };
			QString name;
			std::map<std::string,std::any> customData;
		};

		struct ConditionStatus
		{
			bool triggered{ false };
			QDateTime lastChangeTime;
			QDateTime lastTriggeredCallTime;    // 上次触发态周期回调的时间
			QDateTime lastNotTriggeredCallTime; // 上次非触发态周期回调的时间
			QDateTime lastCheckTime;	// 上次检测时间
		};

		struct ConditionSet
		{
			bool enabled{ true };
			int debounceMs{ 500 };                  // 条件连续为true需要持续时间(ms),若为0则立即触发(中间不允许false)
			int clearDebounceMs{ 500 };             // 从 true 变为 false 后需要持续时间才清除(ms)
			bool autoClear{ true };                 // 条件消失时是否自动清除报警
			int triggeredPeriodicMs{ 0 };           // 触发态周期回调间隔(ms),0 表示不启用
			int notTriggeredPeriodicMs{ 0 };        // 非触发态周期回调间隔(ms),0 表示不启用
			int intervalMs{ 500 };
		};

		struct ConditionCall
		{
			// 用户提供的条件:返回 true 表示应当触发
			std::function<bool(const ConditionInfo&)> condition;
			// 钩子:触发时调用
			std::function<void(const ConditionInfo&)> onTrigger;
			// 钩子:清除时调用
			std::function<void(const ConditionInfo&)> onClear;
			// 钩子:触发态时周期性调用
			std::function<void(const ConditionInfo&)> onTriggeredPeriodic;
			// 钩子:非触发态时周期性调用
			std::function<void(const ConditionInfo&)> onNotTriggeredPeriodic;
		};

		struct ConditionItem
		{
			ConditionInfo info;
			ConditionStatus status;
			ConditionSet set;
			ConditionCall call;
		};
	}
}
