#include"hoem_ModbusDeviceScheduler.hpp"

namespace rw
{
	namespace hoem
	{
		ModbusDeviceScheduler::ModbusDeviceScheduler(std::shared_ptr<ModbusDevice> device) noexcept
			: _device(std::move(device))
		{
			_worker = std::thread(&ModbusDeviceScheduler::workerLoop, this);
		}

		ModbusDeviceScheduler::~ModbusDeviceScheduler() noexcept
		{
			{
				std::lock_guard<std::mutex> lock(_queueMutex);
				_running = false;
			}
			_cv.notify_all();
			if (_worker.joinable()) {
				_worker.join();
			}
		}

		void ModbusDeviceScheduler::workerLoop() noexcept
		{
			while (true) {
				TaskItem item{};
				{
					std::unique_lock<std::mutex> lock(_queueMutex);
					_cv.wait(lock, [this] { return !_running || !_tasks.empty(); });
					if (!_running && _tasks.empty()) {
						return;
					}
					item = std::move(_tasks.top());
					_tasks.pop();
				}
				item.work();
				_pendingTasks.fetch_sub(1);
				_cv.notify_all();
			}
		}

		bool ModbusDeviceScheduler::isDeviceConnected() noexcept
		{
			auto fut = isConnectedAsync(0);
			auto res = fut.get();
			return res.second ? res.first : false;
		}

		bool ModbusDeviceScheduler::reconnectDevice() noexcept
		{
			auto fut = reconnectAsync(0);
			return fut.get();
		}

		std::future<bool> ModbusDeviceScheduler::connectAsync(int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this]() noexcept
				{
					return _device ? _device->connect() : false;
				}, false);
		}

		std::future<bool> ModbusDeviceScheduler::disconnectAsync(int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this]() noexcept
				{
					return _device ? _device->disconnect() : false;
				}, false);
		}

		std::future<bool> ModbusDeviceScheduler::reconnectAsync(int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this]() noexcept
				{
					return _device ? _device->reconnect() : false;
				}, false);
		}

		std::future<std::pair<bool, bool>> ModbusDeviceScheduler::isConnectedAsync(int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this]() noexcept
				{
					bool ok = _device && _device->isConnected();
					return std::make_pair(ok, true);
				}, std::make_pair(false, false));
		}

		std::future<bool> ModbusDeviceScheduler::setBasedAddressAsync(Address16 basedAddress, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, basedAddress]() noexcept
				{
					return _device && _device->setBasedAddress(basedAddress);
				}, false);
		}

		std::future<std::pair<Address16, bool>> ModbusDeviceScheduler::getBasedAddressAsync(int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this]() noexcept
				{
					if (_device) {
						return std::make_pair(_device->getBasedAddress(), true);
					}
					return std::make_pair<Address16, bool>(0, false);
				}, std::make_pair<Address16, bool>(0, false));
		}

		std::future<std::pair<std::vector<UInt16>, bool>> ModbusDeviceScheduler::readUInt16RegistersAbsoluteAsync(Address16 address, Quantity quantity, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			std::vector<UInt16> fallbackValue(quantity, 0);
			return enqueueTask(prio, timeout, [this, address, quantity]() noexcept
				{
					std::vector<UInt16> data;
					bool ok = _device && _device->readUInt16RegistersAbsolute(address, quantity, data);
					return std::make_pair(std::move(data), ok);
				}, std::make_pair(std::move(fallbackValue), false));
		}

		std::future<bool> ModbusDeviceScheduler::writeUInt16RegistersAbsoluteAsync(Address16 address, const std::vector<UInt16>& data, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, address, data]() noexcept
				{
					return _device && _device->writeUInt16RegistersAbsolute(address, data);
				}, false);
		}

		std::future<std::pair<std::vector<UInt16>, bool>> ModbusDeviceScheduler::readUInt16RegistersAsync(Address16 startAddress, Quantity quantity, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			std::vector<UInt16> fallbackValue(quantity, 0);
			return enqueueTask(prio, timeout, [this, startAddress, quantity]() noexcept
				{
					std::vector<UInt16> data;
					bool ok = _device && _device->readUInt16Registers(startAddress, quantity, data);
					return std::make_pair(std::move(data), ok);
				}, std::make_pair(std::move(fallbackValue), false));
		}

		std::future<bool> ModbusDeviceScheduler::writeUInt16RegistersAsync(Address16 startAddress, const std::vector<UInt16>& data, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, data]() noexcept
				{
					return _device && _device->writeUInt16Registers(startAddress, data);
				}, false);
		}

		std::future<std::pair<UInt16, bool>> ModbusDeviceScheduler::readUInt16RegisterAsync(Address16 startAddress, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress]() noexcept
				{
					UInt16 value = 0;
					bool ok = _device && _device->readUInt16Register(startAddress, value);
					return std::make_pair(value, ok);
				}, std::make_pair<UInt16, bool>(0, false));
		}

		std::future<bool> ModbusDeviceScheduler::writeUInt16RegisterAsync(Address16 startAddress, UInt16 data, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, data]() noexcept
				{
					return _device && _device->writeUInt16Register(startAddress, data);
				}, false);
		}

		std::future<std::pair<UInt32, bool>> ModbusDeviceScheduler::readUInt32RegisterAsync(Address16 startAddress, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, byteOrder]() noexcept
				{
					UInt32 value = 0;
					bool ok = _device && _device->readUInt32Register(startAddress, value, byteOrder);
					return std::make_pair(value, ok);
				}, std::make_pair<UInt32, bool>(0, false));
		}

		std::future<std::pair<std::vector<UInt32>, bool>> ModbusDeviceScheduler::readUInt32RegistersAsync(Address16 startAddress, size_t count, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			std::vector<UInt32> fallbackValue(count, 0);
			return enqueueTask(prio, timeout, [this, startAddress, count, byteOrder]() noexcept
				{
					std::vector<UInt32> data(count, 0);
					bool ok = _device && _device->readUInt32Registers(startAddress, data, byteOrder);
					return std::make_pair(std::move(data), ok);
				}, std::make_pair(std::move(fallbackValue), false));
		}

		std::future<bool> ModbusDeviceScheduler::writeUInt32RegisterAsync(Address16 startAddress, UInt32 data, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, data, byteOrder]() noexcept
				{
					return _device && _device->writeUInt32Register(startAddress, data, byteOrder);
				}, false);
		}

		std::future<bool> ModbusDeviceScheduler::writeUInt32RegistersAsync(Address16 startAddress, const std::vector<UInt32>& data, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, data, byteOrder]() noexcept
				{
					return _device && _device->writeUInt32Registers(startAddress, data, byteOrder);
				}, false);
		}

		std::future<std::pair<float, bool>> ModbusDeviceScheduler::readFloatRegisterAsync(Address16 startAddress, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, byteOrder]() noexcept
				{
					float value = 0.0f;
					bool ok = _device && _device->readFloatRegister(startAddress, value, byteOrder);
					return std::make_pair(value, ok);
				}, std::make_pair(0.0f, false));
		}

		std::future<std::pair<std::vector<float>, bool>> ModbusDeviceScheduler::readFloatRegistersAsync(Address16 startAddress, size_t count, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			std::vector<float> fallbackValue(count, 0.0f);
			return enqueueTask(prio, timeout, [this, startAddress, count, byteOrder]() noexcept
				{
					std::vector<float> data(count, 0.0f);
					bool ok = _device && _device->readFloatRegisters(startAddress, data, byteOrder);
					return std::make_pair(std::move(data), ok);
				}, std::make_pair(std::move(fallbackValue), false));
		}

		std::future<bool> ModbusDeviceScheduler::writeFloatRegisterAsync(Address16 startAddress, float value, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, value, byteOrder]() noexcept
				{
					return _device && _device->writeFloatRegister(startAddress, value, byteOrder);
				}, false);
		}

		std::future<bool> ModbusDeviceScheduler::writeFloatRegistersAsync(Address16 startAddress, const std::vector<float>& data, Endianness byteOrder, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, data, byteOrder]() noexcept
				{
					return _device && _device->writeFloatRegisters(startAddress, data, byteOrder);
				}, false);
		}

		std::future<std::pair<std::vector<bool>, bool>> ModbusDeviceScheduler::readCoilsAsync(Address16 startAddress, Quantity quantity, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			std::vector<bool> fallbackValue(quantity, false);
			return enqueueTask(prio, timeout, [this, startAddress, quantity]() noexcept
				{
					std::vector<bool> data;
					bool ok = _device && _device->readCoils(startAddress, quantity, data);
					return std::make_pair(std::move(data), ok);
				}, std::make_pair(std::move(fallbackValue), false));
		}

		std::future<std::pair<bool, bool>> ModbusDeviceScheduler::readCoilAsync(Address16 startAddress, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress]() noexcept
				{
					bool state = false;
					bool ok = _device && _device->readCoil(startAddress, state);
					return std::make_pair(state, ok);
				}, std::make_pair(false, false));
		}

		std::future<bool> ModbusDeviceScheduler::writeCoilAsync(Address16 address, bool state, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, address, state]() noexcept
				{
					return _device && _device->writeCoil(address, state);
				}, false);
		}

		std::future<bool> ModbusDeviceScheduler::writeCoilsAsync(Address16 startAddress, const std::vector<bool>& states, int prio, std::optional<std::chrono::milliseconds> timeout) noexcept
		{
			return enqueueTask(prio, timeout, [this, startAddress, states]() noexcept
				{
					return _device && _device->writeCoils(startAddress, states);
				}, false);
		}

		void ModbusDeviceScheduler::waitAll() noexcept
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_cv.wait(lock, [this]()
				{
					return !_running || _pendingTasks.load() == 0;
				});
		}
	}
}