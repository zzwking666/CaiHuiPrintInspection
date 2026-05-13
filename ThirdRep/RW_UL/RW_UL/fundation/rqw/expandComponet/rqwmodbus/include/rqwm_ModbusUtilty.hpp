#pragma once
#include"hoem_ModbusDeviceFactory.hpp"
namespace rw::hoem
{
	struct ModbusConfig;
	enum class ModbusO;
	enum class ModbusI;
	enum class ModbusDeviceName;
}

namespace rw
{
	namespace rqwm
	{
		using ModbusType = rw::hoem::ModbusDeviceName;
		using ModbusI = rw::hoem::ModbusI;
		using ModbusO = rw::hoem::ModbusO;
		using ModbusConfig = rw::hoem::ModbusConfig;
	}
}