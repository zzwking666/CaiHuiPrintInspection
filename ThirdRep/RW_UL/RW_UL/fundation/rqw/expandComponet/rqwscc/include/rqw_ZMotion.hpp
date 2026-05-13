#pragma once
#include <memory>
#include <QString>

namespace zwy
{
	namespace scc
	{
		class Motion;
	}
}

namespace rw
{
	namespace rqw
	{
		class ZMotion
		{
		private:
			zwy::scc::Motion* _zMotion{ nullptr };
		public:
			explicit ZMotion(const QString& ip);
			ZMotion();
			~ZMotion();
		private:
			QString _ip{};
		public:
			void setIp(const QString& ip);
			QString getIp(const QString& ip);
		public:
			[[nodiscard]] bool connect();

			[[nodiscard]] bool connectPcie(int type, const std::string& pconnectstring, unsigned int uims);

			[[nodiscard]] bool getConnectState(bool& isGet);
			[[nodiscard]] bool getConnectState();

			[[nodiscard]] bool disConnect(bool& isGet);
			[[nodiscard]] bool disConnect();
		public:
			[[nodiscard]] bool getIOIn(int portNum);
			[[nodiscard]] bool getIOIn(int portNum, bool& isGet);

			[[nodiscard]] bool getIOOut(int portNum);
			[[nodiscard]] bool getIOOut(int portNum, bool& isGet);

			[[nodiscard]] bool setIOOut(int portNum, bool state);
			[[nodiscard]] bool SetIOOut(int axis, int ioNUm, bool state, int iotime);
		public:
			[[nodiscard]] bool setAxisPulse(int axis, float value);
			[[nodiscard]] bool setAxisRunSpeed(int axis, float value);

			[[nodiscard]] bool setAxisAcc(int axis, float value);

			[[nodiscard]] bool setAxisDec(int axis, float value);

			[[nodiscard]] bool setAxisRun(int axis, float value);

			[[nodiscard]] bool stopAllAxis();

			[[nodiscard]] float getAxisLocation(int axis, bool& isGet);
			[[nodiscard]] float getAxisLocation(int axis);

			[[nodiscard]] bool singleStop(int axis);

			[[nodiscard]] bool singleMove(int axis, int dir, float speed = 20, float acc = 1000, float dec = 1000, float units = 0);

			[[nodiscard]] bool singleMove(int axis, double dir);

			[[nodiscard]] bool setLocationZero(int axis);

			[[nodiscard]] bool setModbus(int adress, int num, float value);

			[[nodiscard]] float getModbus(int adress, int num, bool& isGet);
			[[nodiscard]] float getModbus(int adress, int num);

			[[nodiscard]] bool setAxisType(int axis, int value);

			bool switchOpen(int num, int enable, int axisnum, int outnum, int  outstate, float setpos, float resetpos);

		};
	}
}