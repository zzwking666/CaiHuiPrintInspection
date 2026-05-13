#include "scc_Motion.h"

#include<iostream>

namespace zwy {
	namespace scc {


		Motion::Motion()
		{
		}
		bool Motion::OpenBoard(char* ipAdress)
		{
			// qDebug()<<"运行";
			char* ip_addr = (char*)ipAdress;
			int ret = ZAux_OpenEth(ip_addr, &g_handle);
			//qDebug()<<ipAdress;
			if (ERR_SUCCESS != ret)
			{
				g_handle = nullptr;
				//qDebug()<<"fale";
				isOPen = false;
				return false;
			}
			else
			{
				// qDebug()<<"success";
				isOPen = true;
				ZMC_SetTimeOut(g_handle, 100);

				return true;
			}
		}

		bool Motion::OpenBoardPcie(int type, char* pconnectstring, uint32 uims)
		{
			// qDebug()<<"运行";
			int ret = ZAux_FastOpen(type, pconnectstring, uims, &g_handle);
			//qDebug()<<ipAdress;
			if (ERR_SUCCESS != ret)
			{
				g_handle = nullptr;
				//qDebug()<<"fale";
				isOPen = false;
				return false;
			}
			else
			{
				// qDebug()<<"success";
				isOPen = true;
				ZMC_SetTimeOut(g_handle, 100);

				return true;
			}
		}

		bool Motion::OpenBoard(const std::string& ipAdress)
		{
			return OpenBoard(const_cast<char*>(ipAdress.c_str()));
		}

		bool Motion::OpenBoardPcie(int type, const std::string& pconnectstring, uint32 uims)
		{
			return OpenBoardPcie(type, const_cast<char*>(pconnectstring.c_str()), uims);
		}

		bool Motion::CloseBoared()
		{
			if (g_handle != nullptr)
			{
				auto reuslt = ZAux_Close(g_handle);
				//关闭连接
				if (reuslt == ERROR_OK)
				{
					g_handle = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		//bool Motion::GetBoardStatue()
		//{
		//}
		bool Motion::SetAxisRun(int axis, float value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			ZAux_Direct_Single_Vmove(g_handle, axis, -value);
			return true;
		}

		//设置轴的类型
		//1为脉冲方向方式的步进或伺服
		//2为模拟型号控制伺服
		bool Motion::SetAxisType(int axis, int value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				int ret = ZAux_Direct_SetAtype(g_handle, axis, value);
				return true;
			}
		}
		//设置脉冲当量
		bool Motion::SetAxisPulse(int axis, float value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				int  ret = ZAux_Direct_SetUnits(g_handle, axis, value);
				return true;
			}
		}
		//设置轴的运动速度
		bool Motion::SetAxisRunSpeed(int axis, float value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				ZAux_Direct_SetSpeed(g_handle, axis, value);
				return true;
			}
		}

		//获取当前轴的位置
		bool Motion::GetAxisLocation(int axis, float& value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				ZAux_Direct_GetDpos(g_handle, axis, &value);
				return true;
			}
		}

		bool Motion::SetModbus(uint16 adress, uint16 num, uint8 value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			uint8 v;
			v = value;
			int ret = ZAux_Modbus_Set0x(g_handle, adress, num, &v);
			return true;
		}

		bool Motion::SetIOOut(int axis, int ioNUm, bool state, int iotime)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				int s = -1;
				if (state == true)
				{
					s = 1;
					ZAux_Direct_MoveOp2(g_handle, axis, ioNUm, s, iotime);
				}
				else
				{
					s = 0;
					ZAux_Direct_MoveOp2(g_handle, axis, ioNUm, s, iotime);
				}

				return true;
			}
		}

		//获取输入IO状态
		bool  Motion::GetIOIn(int portNum)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				unsigned int in_value;
				int ret = ZAux_Direct_GetIn(g_handle, portNum, &in_value);
				if (in_value == 0)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		//获取输出IO状态
		bool  Motion::GetIOOut(int portNum)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				unsigned int out_value;
				int  ret = ZAux_Direct_GetOp(g_handle, portNum, &out_value);
				if (out_value == 0)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		//设置输出IO状态
		bool Motion::SetIOOut(int portNum, bool state)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				int s = -1;
				if (state == true)
				{
					s = 1;
					int ret = ZAux_Direct_SetOp(g_handle, portNum, s);
					return true;
				}
				else
				{
					s = 0;
					int ret = ZAux_Direct_SetOp(g_handle, portNum, s);
					return true;
				}
			}
		}

		//单轴停止运动
		bool Motion::Single_Stop(int axis)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				return true;
			}
		}
		//所有轴停止运动
		bool Motion::StopAllAxis()
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle != nullptr)
			{
				for (int i = 0; i < 8; i++)
				{
					ZAux_Direct_Single_Cancel(g_handle, i, 2);
				}

				return true;
			}
		}
		//单轴运动
		bool Motion::Single_Move(int axis, double dir)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle)
			{
				int ret = ZAux_Direct_Single_Move(g_handle, axis, dir);
				return true;
			}
		}
		//单轴运动
		bool Motion::Single_Move(int axis, int dir, float speed, float acc, float dec, float Units)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle)
			{
				if (dir > 0)
				{
					dir = 1;
				}
				else
				{
					dir = -1;
				}
				int ret = ZAux_Direct_SetAtype(g_handle, axis, 2);//设置为脉冲式伺服
				ret = ZAux_Direct_SetSpeed(g_handle, axis, speed); //设置轴 0 速度为 200units/s
				ret = ZAux_Direct_SetUnits(g_handle, axis, Units);//设置轴 0 脉冲当量为 200units/s
				ret = ZAux_Direct_SetAccel(g_handle, axis, acc); //设置轴 0 加速度为 2000units/s/s
				ret = ZAux_Direct_SetDecel(g_handle, axis, dec); //设置轴 0 减速度为 2000units/s/s
				ret = ZAux_Direct_SetSramp(g_handle, axis, 100); //设置轴 0 S曲线时间 100(梯形加减速)
				ret = ZAux_Direct_Single_Vmove(g_handle, axis, dir);
				return true;
			}
		}

		//轴位置清零
		bool Motion::SetLocationZero(int axis)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			if (g_handle)
			{
				int ret = ZAux_Direct_SetDpos(g_handle, axis, 0);
				return true;
			}
		}

		bool Motion::getBoardState()
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			uint8 state = -1;
			if (g_handle)
			{
				ZMC_GetState(g_handle, &state);
			}
			if (state == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool Motion::getAxisSpeed(int axis, float& speed)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			ZAux_Direct_GetVpSpeed(g_handle, axis, &speed);
			return true;
		}

		//  设置轴运动加速度
		bool Motion::SetAxisAcc(int axis, float value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			ZAux_Direct_SetAccel(g_handle, axis, value);
			return true;
		}
		//  设置轴运动减速度
		bool Motion::SetAxisDec(int axis, float value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			ZAux_Direct_SetDecel(g_handle, axis, value);
			return true;
		}

		bool Motion::GetAllIOIN(int portNum)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			ZAux_Direct_GetInMulti(g_handle, 8, 11, &portNum);
			return true;
		}

		bool Motion::SetModbus(uint16 adress, uint16 num, float value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			float v;
			v = value;
			// int ret = ZAux_Modbus_Set0x(g_handle, adress, num,  &v);
			int ret = ZAux_Modbus_Set4x_Float(g_handle, adress, num, &v);
			return true;
		}

		bool Motion::GetModbus(uint16 adress, uint16 num, float& value)
		{
			if (g_handle == nullptr)
			{
				return false;
			}
			float v;
			v = value;
			// int ret = ZAux_Modbus_Set0x(g_handle, adress, num,  &v);
			int ret = ZAux_Modbus_Get4x_Float(g_handle, adress, num, &v);
			value = v;
			return true;
		}

		bool Motion::switchOpen(int num, int enable, int axisnum, int outnum, int  outstate, float setpos, float resetpos) {
			auto result = ZAux_Direct_Pswitch(g_handle, num, enable, axisnum, outnum, outstate, setpos, resetpos);

			return true;
		}
	}
}