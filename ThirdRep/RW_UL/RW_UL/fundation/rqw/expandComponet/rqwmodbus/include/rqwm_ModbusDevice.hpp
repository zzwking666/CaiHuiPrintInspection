#pragma once

#include"rqwm_ModbusUtilty.hpp"
#include<QObject>

namespace rw::hoem
{
	class IModbusDevice;
}

namespace rw
{
	namespace rqwm
	{
		class ModbusDevice : public QObject
		{
			Q_OBJECT
		public:
			explicit ModbusDevice(const ModbusType& type, const ModbusConfig& config, QObject* parent = nullptr);
			~ModbusDevice()override = default;
		private:
			std::unique_ptr<hoem::IModbusDevice> _device;
		public:
			bool connect();

			bool disconnect();

			bool reconnect();

			[[nodiscard]] bool isConnected() const;

			[[nodiscard]] bool getIState(ModbusI locate) const;

			bool setOState(ModbusO locate, bool state);

			[[nodiscard]] bool getOState(ModbusO locate) const;
		};
	}
}