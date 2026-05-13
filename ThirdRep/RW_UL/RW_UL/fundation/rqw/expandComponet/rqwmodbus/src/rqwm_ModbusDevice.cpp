#include"rqwm_ModbusDevice.hpp"

namespace rw
{
	namespace rqwm
	{
		ModbusDevice::ModbusDevice(const ModbusType& type, const ModbusConfig& config, QObject* parent)
			: QObject(parent)
		{
			_device = hoem::ModbusDeviceFactory::createDevice(type, config);
			if (_device == nullptr)
			{
				throw std::runtime_error("Failed to create Modbus device");
			}
		}

		bool ModbusDevice::connect()
		{
			if (_device)
			{
				return _device->connect();
			}
			return false;
		}

		bool ModbusDevice::disconnect()
		{
			if (_device)
			{
				return _device->disconnect();
			}
			return false;
		}

		bool ModbusDevice::reconnect()
		{
			if (_device)
			{
				return _device->reconnect();
			}
			return false;
		}

		bool ModbusDevice::isConnected() const
		{
			if (_device)
			{
				return _device->isConnected();
			}
			return false;
		}

		bool ModbusDevice::getIState(ModbusI locate) const
		{
			if (_device)
			{
				return _device->getIState(locate);
			}
			return false;
		}

		bool ModbusDevice::setOState(ModbusO locate, bool state)
		{
			if (_device)
			{
				return _device->setOState(locate, state);
			}
			return false;
		}

		bool ModbusDevice::getOState(ModbusO locate) const
		{
			if (_device)
			{
				return _device->getOState(locate);
			}
			return false;
		}
	}
}