#ifndef MOTION_H
#define MOTION_H

#include <string>

#include"zauxdll2.h"
#include"zmotion.h"

namespace zwy {
	namespace scc {
		class Motion
		{
		public:
			Motion();

			//打开控制器
			bool OpenBoard(char* ipAdress);
			bool OpenBoardPcie(int type, char* pconnectstring, uint32 uims);
			bool OpenBoard(const std::string& ipAdress);
			bool OpenBoardPcie(int type, const std::string& pconnectstring, uint32 uims);
			//关闭板卡
			bool CloseBoared();
			//    //获取板卡连接状态
			//    bool GetBoardStatue( );
				//设置轴的类型
			bool SetAxisType(int axis, int value);
			//设置ModBus的类型
			bool SetModbus(uint16 adress, uint16 num, uint8 value);

			//设置轴的脉冲当量
			bool SetAxisPulse(int axis, float value);
			//  设置轴运动速度
			bool SetAxisRunSpeed(int axis, float value);
			//  设置轴运动加速度
			bool SetAxisAcc(int axis, float value);
			//  设置轴运动减速度
			bool SetAxisDec(int axis, float value);
			//获取当前轴的位置
			 //  设置轴运动
			bool SetAxisRun(int axis, float value);
			//返回轴的参数
			bool GetAxisLocation(int axis, float& value);
			//获取输入IO状态
			bool GetIOIn(int portNum);
			//获取输出IO状态
			bool GetIOOut(int portNum);
			//获取输入IO状态
			bool GetAllIOIN(int portNum);
			//设置输出IO状态
			bool SetIOOut(int portNum, bool state);
			//单轴停止运动
			bool Single_Stop(int axis);
			//停止所有轴
			bool StopAllAxis();
			//单轴运动
			bool Single_Move(int axis, int dir, float speed = 20, float acc = 1000, float dec = 1000, float units = 0);
			//单轴运动
			bool Single_Move(int axis, double dir);
			//轴位置清零
			bool SetLocationZero(int axis);
			//获取板卡状态
			bool getBoardState();
			//获取板卡状态
			bool getAxisSpeed(int axis, float& speed);
			//设置io
			bool SetIOOut(int axis, int ioNUm, bool state, int iotime);

			bool SetModbus(uint16 adress, uint16 num, float value);
			bool GetModbus(uint16 adress, uint16 num, float& value);
			bool switchOpen(int num, int enable, int axisnum, int outnum, int  outstate, float setpos, float resetpos);
			bool isOPen{ false };
		private:
			ZMC_HANDLE g_handle = nullptr;
		};
	}
}

#endif // MOTION_H
