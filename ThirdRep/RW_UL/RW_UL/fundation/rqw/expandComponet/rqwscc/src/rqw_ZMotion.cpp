#include"rqw_ZMotion.hpp"

#include"scc_motion.h"

namespace rw
{
	namespace rqw
	{
		ZMotion::ZMotion(const QString& ip)
			:_ip(ip)
		{
			_zMotion = new zwy::scc::Motion;
			_zMotion->OpenBoard(ip.toStdString());
		}

		ZMotion::ZMotion()
		{
		}

		ZMotion::~ZMotion()
		{
			if (_zMotion)
			{
				disConnect();
			}
		}

		void ZMotion::setIp(const QString& ip)
		{
			_ip = ip;
		}

		QString ZMotion::getIp(const QString& ip)
		{
			return _ip;
		}

		bool ZMotion::connect()
		{
			_zMotion = new zwy::scc::Motion;
			return _zMotion->OpenBoard(_ip.toStdString());
		}

		bool ZMotion::connectPcie(int type, const std::string& pconnectstring, unsigned int uims)
		{
			_zMotion = new zwy::scc::Motion;
			return _zMotion->OpenBoardPcie(type, pconnectstring, uims);
		}

		bool ZMotion::getConnectState(bool& isGet)
		{
			if (!_zMotion)
			{
				isGet = false;
				return false;
			}
			isGet = true;
			return _zMotion->getBoardState();
		}

		bool ZMotion::getConnectState()
		{
			bool temp{ true };
			return getConnectState(temp);
		}

		bool ZMotion::disConnect(bool& isGet)
		{
			if (!_zMotion)
			{
				isGet = false;
				return false;
			}
			isGet = true;
			auto result = _zMotion->CloseBoared();
			delete _zMotion;
			_zMotion = nullptr;
			return result;
		}

		bool ZMotion::disConnect()
		{
			bool temp{ true };
			return disConnect(temp);
		}

		bool ZMotion::getIOIn(int portNum)
		{
			bool temp{ true };
			return getIOIn(portNum, temp);
		}

		bool ZMotion::getIOIn(int portNum, bool& isGet)
		{
			if (!_zMotion)
			{
				isGet = false;
				return false;
			}
			isGet = true;
			return _zMotion->GetIOIn(portNum);
		}

		bool ZMotion::getIOOut(int portNum)
		{
			bool temp{ true };
			return getIOOut(portNum, temp);
		}

		bool ZMotion::getIOOut(int portNum, bool& isGet)
		{
			if (!_zMotion)
			{
				isGet = false;
				return false;
			}
			isGet = true;
			return _zMotion->GetIOOut(portNum);
		}

		bool ZMotion::setIOOut(int portNum, bool state)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetIOOut(portNum, state);
		}

		bool ZMotion::SetIOOut(int axis, int ioNUm, bool state, int iotime)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetIOOut(axis, ioNUm, state, iotime);
		}

		bool ZMotion::setAxisPulse(int axis, float value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetAxisPulse(axis, value);
		}

		bool ZMotion::setAxisRunSpeed(int axis, float value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetAxisRunSpeed(axis, value);
		}

		bool ZMotion::setAxisAcc(int axis, float value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetAxisAcc(axis, value);
		}

		bool ZMotion::setAxisDec(int axis, float value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetAxisDec(axis, value);
		}

		bool ZMotion::setAxisRun(int axis, float value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetAxisRun(axis, value);
		}

		bool ZMotion::stopAllAxis()
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->StopAllAxis();
		}

		float ZMotion::getAxisLocation(int axis, bool& isGet)
		{
			float result{ 0 };
			if (!_zMotion)
			{
				isGet = false;
				return result;
			}
			isGet = _zMotion->GetAxisLocation(axis, result);
			return result;
		}

		float ZMotion::getAxisLocation(int axis)
		{
			bool isGet{ false };
			return getAxisLocation(axis, isGet);
		}

		bool ZMotion::singleStop(int axis)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->Single_Stop(axis);
		}

		bool ZMotion::singleMove(int axis, int dir, float speed, float acc, float dec, float units)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->Single_Move(axis, dir, speed, acc, dec, units);
		}

		bool ZMotion::singleMove(int axis, double dir)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->Single_Move(axis, dir);
		}

		bool ZMotion::setLocationZero(int axis)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetLocationZero(axis);
		}

		bool ZMotion::setModbus(int adress, int num, float value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetModbus(adress, num, value);
		}

		float ZMotion::getModbus(int adress, int num, bool& isGet)
		{
			if (!_zMotion)
			{
				isGet = false;
				return false;
			}
			isGet = true;

			float result = 0;
			isGet = _zMotion->GetModbus(adress, num, result);
			return result;
		}

		float ZMotion::getModbus(int adress, int num)
		{
			bool isGet{ false };
			return getModbus(adress, num, isGet);
		}

		bool ZMotion::setAxisType(int axis, int value)
		{
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->SetAxisType(axis, value);
		}

		bool ZMotion::switchOpen(int num, int enable, int axisnum, int outnum, int  outstate, float setpos, float resetpos) {
			if (!_zMotion)
			{
				return false;
			}

			return _zMotion->switchOpen(num, enable, axisnum, outnum, outstate, setpos, resetpos);
		}
	}
}