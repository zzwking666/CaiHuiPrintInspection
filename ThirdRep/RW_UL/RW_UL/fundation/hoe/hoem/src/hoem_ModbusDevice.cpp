#include"hoem_ModbusDevice.hpp"

#include <iostream>
#include <modbus.h>
#include <ostream>

#include "lgm_PreDef.hpp"

namespace rw
{
	namespace hoem
	{

		ModbusDevice::ModbusDevice(const ModbusDeviceTcpCfg& cfg)
		{
			LOG_INFO("Build modbus by tcp, ip \"{}\",port \"{}\" ...", cfg.ip, cfg.port);

			_modbusContext = modbus_new_tcp(cfg.ip.c_str(), cfg.port);
			if (_modbusContext == nullptr) {
				LOG_ERROR("Failed to create Modbus context");

				throw std::runtime_error("Failed to create Modbus context");
			}
			modbus_set_slave(_modbusContext, 1); // 设置从站ID为1
			_checkConnectStatusCoil = cfg.checkConnectStatusCoil;
		}

		ModbusDevice::ModbusDevice(const ModbusDeviceRtuCfg& cfg)
		{
			LOG_INFO("Build modbus by rtu, device \"{}\",baud \"{}\",dataBit \"{}\",stopBit \"{}\" ,parity \"{}\"...", cfg.device,cfg.baud, cfg.dataBit,cfg.stopBit,cfg.parity);

			_modbusContext = modbus_new_rtu(cfg.device.c_str(), cfg.baud, cfg.parity, cfg.dataBit, cfg.stopBit);
			if (_modbusContext == nullptr) {
				LOG_ERROR("Failed to create Modbus RTU context");

				throw std::runtime_error("Failed to create Modbus RTU context");
			}
			modbus_set_slave(_modbusContext, 1); // 设置从站ID为1
			_baseAddress = cfg.baseAddress;
			_checkConnectStatusCoil = cfg.checkConnectStatusCoil;

		}

		ModbusDevice::~ModbusDevice()
		{
			ModbusDevice::disconnect();
			if (_modbusContext != nullptr) {
				modbus_free(_modbusContext);
				_modbusContext = nullptr;
			}
		}

		bool ModbusDevice::connect()
		{
			if (modbus_connect(_modbusContext) == -1)
			{
				LOG_WARN("Connect modbus failed");
				return false;
			}
			LOG_INFO("Connect modbus success");
			return true;
		}

		bool ModbusDevice::disconnect()
		{
			if (_modbusContext != nullptr) {
				modbus_close(_modbusContext);
				LOG_INFO("Disconnect modbus success");
				return true;
			}
			LOG_INFO("Disconnect modbus failed");
			return false;
		}

		bool ModbusDevice::isConnected() const
		{
			if (_modbusContext == nullptr) {
				return false;
			}

			uint8_t coilData = 0;
			int result = modbus_read_bits(_modbusContext, _checkConnectStatusCoil, 1, &coilData);

			if (result == 1)
			{
				return true;
			}

			LOG_WARN("Modbus disconnected!");
		
			return false;
		}

		bool ModbusDevice::reconnect()
		{
			if (modbus_connect(_modbusContext) == -1)
			{
				LOG_WARN("Reconnect modbus failed");
				return false;
			}
			LOG_INFO("Reconnect modbus success");
			return true;
		}

		bool ModbusDevice::readUInt16Registers(Address16 startAddress, Quantity quantity, std::vector<UInt16>& data)
		{
			data.resize(quantity);
			int result = modbus_read_registers(_modbusContext, startAddress + _baseAddress, quantity, reinterpret_cast<uint16_t*>(data.data()));
			uint16_t number = static_cast<uint16_t>(data[0]);

			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::writeUInt16Registers(Address16 startAddress, const std::vector<UInt16>& data)
		{
			int result = modbus_write_registers(_modbusContext, startAddress + _baseAddress, data.size(), reinterpret_cast<const uint16_t*>(data.data()));
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::readUInt16Register(Address16 startAddress, UInt16& data)
		{
			uint16_t reg = 0;
			int result = modbus_read_registers(_modbusContext, startAddress + _baseAddress, 1, &reg);
			if (result < 0) {
				return false;
			}
			data = static_cast<UInt16>(reg);
			return true;
		}

		bool ModbusDevice::writeUInt16Register(Address16 startAddress, UInt16 data)
		{
			int result = modbus_write_register(_modbusContext, startAddress + _baseAddress, static_cast<uint16_t>(data));
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::writeUInt32Register(Address16 startAddress, UInt32 data, Endianness byteOrder)
		{

			// 将 32 位值拆分为两个 16 位寄存器（高 16 位和低 16 位）
			uint16_t high = static_cast<uint16_t>((static_cast<uint32_t>(data) >> 16) & 0xFFFFu);
			uint16_t low = static_cast<uint16_t>(static_cast<uint32_t>(data) & 0xFFFFu);

			uint16_t regs[2];

			// 根据字节序在两个寄存器之间安排顺序
			// 常见约定：BigEndian - 高字在前；LittleEndian - 低字在前
			switch (byteOrder) {
			case Endianness::BigEndian:
				regs[0] = high;
				regs[1] = low;
				break;
			case Endianness::LittleEndian:
				regs[0] = low;
				regs[1] = high;
				break;
			default:
				// 若有其他自定义字节序，回退为 BigEndian 行为
				regs[0] = high;
				regs[1] = low;
				break;
			}

			int result = modbus_write_registers(_modbusContext, startAddress + _baseAddress, 2, regs);
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::writeUInt32Registers(Address16 startAddress, const std::vector<UInt32>& data,
			Endianness byteOrder)
		{

			if (data.empty()) {
				return false;
			}

			const size_t count32 = data.size();
			const size_t regCount = count32 * 2; // 每个 32 位值占用两个 16 位寄存器

			std::vector<uint16_t> regs(regCount);

			for (size_t i = 0; i < count32; ++i) {
				uint32_t v = static_cast<uint32_t>(data[i]);
				uint16_t high = static_cast<uint16_t>((v >> 16) & 0xFFFFu);
				uint16_t low = static_cast<uint16_t>(v & 0xFFFFu);

				if (byteOrder == Endianness::BigEndian) {
					regs[i * 2] = high;
					regs[i * 2 + 1] = low;
				}
				else if (byteOrder == Endianness::LittleEndian) {
					regs[i * 2] = low;
					regs[i * 2 + 1] = high;
				}
				else {
					// 其它情况回退为 BigEndian 行为
					regs[i * 2] = high;
					regs[i * 2 + 1] = low;
				}
			}

			int result = modbus_write_registers(_modbusContext, startAddress + _baseAddress, static_cast<int>(regCount), regs.data());
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::readUInt32Register(Address16 startAddress, UInt32& data, Endianness byteOrder)
		{

			uint16_t regs[2] = { 0, 0 };
			int result = modbus_read_registers(_modbusContext, startAddress + _baseAddress, 2, regs);
			if (result < 0) {
				return false;
			}

			uint32_t value = 0;
			switch (byteOrder) {
			case Endianness::BigEndian:
				// regs[0] = high, regs[1] = low
				value = (static_cast<uint32_t>(regs[0]) << 16) | static_cast<uint32_t>(regs[1]);
				break;
			case Endianness::LittleEndian:
				// regs[0] = low, regs[1] = high
				value = (static_cast<uint32_t>(regs[1]) << 16) | static_cast<uint32_t>(regs[0]);
				break;
			default:
				// 回退为 BigEndian 行为
				value = (static_cast<uint32_t>(regs[0]) << 16) | static_cast<uint32_t>(regs[1]);
				break;
			}

			data = static_cast<UInt32>(value);
			return true;
		}

		bool ModbusDevice::readUInt32Registers(Address16 startAddress, std::vector<UInt32>& data, Endianness byteOrder)
		{

			// 需要读取多少个 32 位值
			if (data.empty()) {
				return false;
			}

			const size_t count32 = data.size();
			const size_t regCount = count32 * 2; // 每个 32 位值占用两个 16 位寄存器

			std::vector<uint16_t> regs(regCount, 0);
			int result = modbus_read_registers(_modbusContext, startAddress + _baseAddress, static_cast<int>(regCount), regs.data());
			if (result < 0 || result != static_cast<int>(regCount)) {
				return false;
			}

			for (size_t i = 0; i < count32; ++i) {
				uint16_t r0 = regs[i * 2];
				uint16_t r1 = regs[i * 2 + 1];
				uint32_t value = 0;

				switch (byteOrder) {
				case Endianness::BigEndian:
					// regs[0] = high, regs[1] = low
					value = (static_cast<uint32_t>(r0) << 16) | static_cast<uint32_t>(r1);
					break;
				case Endianness::LittleEndian:
					// regs[0] = low, regs[1] = high
					value = (static_cast<uint32_t>(r1) << 16) | static_cast<uint32_t>(r0);
					break;
				default:
					// 回退为 BigEndian 行为
					value = (static_cast<uint32_t>(r0) << 16) | static_cast<uint32_t>(r1);
					break;
				}

				data[i] = static_cast<UInt32>(value);
			}

			return true;
		}

		bool ModbusDevice::readCoils(Address16 startAddress, Quantity quantity, std::vector<bool>& data)
		{
			/*if (!isConnected()) {
				return false;
			}*/
			data.resize(quantity);
			std::vector<uint8_t> coilData(quantity, 0); // 每个线圈 1 字节
			int result = modbus_read_bits(_modbusContext, startAddress + _baseAddress, quantity, coilData.data());
			if (result < 0) {
				return false;
			}
			for (size_t i = 0; i < quantity; ++i) {
				data[i] = coilData[i] != 0;
			}
			return true;
		}

		bool ModbusDevice::readCoil(Address16 startAddress, bool& state)
		{
			uint8_t coilData = 0;
			int result = modbus_read_bits(_modbusContext, startAddress + _baseAddress, 1, &coilData);
			if (result < 0) {
				return false;
			}
			state = (coilData & 0x01) != 0;
			return true;
		}

		bool ModbusDevice::writeCoil(Address16 address, bool state)
		{
			int result = modbus_write_bit(_modbusContext, address + _baseAddress, state ? 1 : 0);
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::writeCoils(Address16 startAddress, const std::vector<bool>& states)
		{
			if (states.empty()) {
				return true;
			}
			std::vector<uint8_t> coilData(states.size(), 0); // 每个线圈 1 字节
			for (size_t i = 0; i < states.size(); ++i) {
				coilData[i] = states[i] ? 1 : 0;
			}
			int result = modbus_write_bits(_modbusContext, startAddress + _baseAddress, states.size(), coilData.data());
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::setBasedAddress(Address16 basedAddress)
		{
			_baseAddress = basedAddress;
			return true;
		}

		Address16 ModbusDevice::getBasedAddress() const
		{
			return _baseAddress;
		}

		bool ModbusDevice::readUInt16RegistersAbsolute(Address16 address, Quantity quantity, std::vector<UInt16>& data)
		{
			data.resize(quantity);
			int result = modbus_read_registers(_modbusContext, address, quantity, reinterpret_cast<uint16_t*>(data.data()));
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::writeUInt16RegistersAbsolute(Address16 address, const std::vector<UInt16>& data)
		{
			int result = modbus_write_registers(_modbusContext, address, data.size(), reinterpret_cast<const uint16_t*>(data.data()));
			if (result < 0) {
				return false;
			}
			return true;
		}

		bool ModbusDevice::writeFloatRegister(Address16 startAddress, float value, Endianness byteOrder)
		{
			UInt32 bits = floatToUInt32(value);
			return writeUInt32Register(startAddress, bits, byteOrder);
		}

		bool ModbusDevice::writeFloatRegisters(Address16 startAddress, const std::vector<float>& data, Endianness byteOrder)
		{
			if (data.empty())
				return true;
			std::vector<UInt32> packed;
			packed.resize(data.size());
			for (float f : data)
				packed.push_back(floatToUInt32(f));
			return writeUInt32Registers(startAddress, packed, byteOrder);
		}

		bool ModbusDevice::readFloatRegister(Address16 startAddress, float& value, Endianness byteOrder)
		{
			UInt32 bits{};
			if (!readUInt32Register(startAddress, bits, byteOrder))
				return false;
			value = uint32ToFloat(bits);
			return true;
		}

		bool ModbusDevice::readFloatRegisters(Address16 startAddress, std::vector<float>& values, Endianness byteOrder)
		{
			std::vector<UInt32> raw;
			raw.clear();
			raw.resize(values.size());
			if (!readUInt32Registers(startAddress, raw, byteOrder))
				return false;
			values.clear();
			for (UInt32 b : raw)
				values.push_back(uint32ToFloat(b));
			return true;
		}
	}
}