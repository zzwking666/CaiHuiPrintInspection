#include"comMon_ConditionMonitor_t.hpp"



TEST_F(ConditionMonitorTest, ImageProcess)
{


	monitor.start();  // 每200ms检测一次



	app->exec();
}