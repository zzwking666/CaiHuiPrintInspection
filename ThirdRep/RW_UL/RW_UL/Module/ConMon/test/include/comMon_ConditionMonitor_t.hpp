#pragma once

#include"gtest/gtest.h"

#include"conMon_ConditionMonitor.hpp"

#include"QApplication"
#include"QThread"

using namespace rw::conMon;

class ConditionMonitorTest : public ::testing::Test {
protected:
	void SetUp() override {
		int argc = 0;
		char* argv[] = { nullptr };
		app = new QApplication(argc, argv);
		qDebug() << "主线程 ID:" << QThread::currentThreadId();

		monitor.registerCondition(
			1, "温度报警",
			[](const ConditionInfo& info) {
				return 90 > 80.0;
			},
			[](const ConditionInfo& info) {
				qWarning() << "温度过高! ID:" << info.id << "名称:" << info.name
					<< "线程:" << QThread::currentThreadId();
			},
			[](const ConditionInfo& info) {
				qInfo() << "温度恢复正常 ID:" << info.id << "名称:" << info.name
					<< "线程:" << QThread::currentThreadId();
			},
			1000, 500, true,
			[](const ConditionInfo& info) {
				qDebug() << "触发态周期回调 ID:" << info.id << "名称:" << info.name;
			},
			[](const ConditionInfo& info) {
				qDebug() << "非触发态周期回调 ID:" << info.id << "名称:" << info.name;
			},
			1000, 1500,
			1000
		);


		monitor.registerCondition(
			2, "报警2",
			[](const ConditionInfo& info) {
				return 90 > 80.0;
			},
			[](const ConditionInfo& info) {
				qWarning() << "温度过高! ID:" << info.id << "名称:" << info.name
					<< "线程:" << QThread::currentThreadId();
			},
			[](const ConditionInfo& info) {
				qInfo() << "温度恢复正常 ID:" << info.id << "名称:" << info.name
					<< "线程:" << QThread::currentThreadId();
			},
			1000, 500, true,
			[](const ConditionInfo& info) {
				qDebug() << "触发态周期回调 ID:" << info.id << "名称:" << info.name;
			},
			[](const ConditionInfo& info) {
				qDebug() << "非触发态周期回调 ID:" << info.id << "名称:" << info.name;
			},
			2000, 3000,
			2000
		);

	}

public:
	ConditionMonitor monitor;
	QApplication *app;
};