#pragma once

#include"hoem_ModbusDevice.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace rw
{
	namespace hoem
	{
		class ModbusDeviceScheduler
		{
		public:
			explicit ModbusDeviceScheduler(std::shared_ptr<ModbusDevice> device) noexcept;
			~ModbusDeviceScheduler() noexcept;

			ModbusDeviceScheduler(const ModbusDeviceScheduler&) = delete;
			ModbusDeviceScheduler& operator=(const ModbusDeviceScheduler&) = delete;

		public:
			// 同步便捷接口
			bool isDeviceConnected() noexcept;
			bool reconnectDevice() noexcept;
			void waitAll() noexcept;

		public:
			// 连接相关
			std::future<bool> connectAsync(int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> disconnectAsync(int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> reconnectAsync(int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<std::pair<bool, bool>> isConnectedAsync(int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			// 基址
			std::future<bool> setBasedAddressAsync(Address16 basedAddress, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<std::pair<Address16, bool>> getBasedAddressAsync(int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			// 16 位寄存器
			std::future<std::pair<std::vector<UInt16>, bool>> readUInt16RegistersAbsoluteAsync(Address16 address, Quantity quantity, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeUInt16RegistersAbsoluteAsync(Address16 address, const std::vector<UInt16>& data, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			std::future<std::pair<std::vector<UInt16>, bool>> readUInt16RegistersAsync(Address16 startAddress, Quantity quantity, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeUInt16RegistersAsync(Address16 startAddress, const std::vector<UInt16>& data, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			std::future<std::pair<UInt16, bool>> readUInt16RegisterAsync(Address16 startAddress, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeUInt16RegisterAsync(Address16 startAddress, UInt16 data, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			// 32 位寄存器
			std::future<std::pair<UInt32, bool>> readUInt32RegisterAsync(Address16 startAddress, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<std::pair<std::vector<UInt32>, bool>> readUInt32RegistersAsync(Address16 startAddress, size_t count, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeUInt32RegisterAsync(Address16 startAddress, UInt32 data, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeUInt32RegistersAsync(Address16 startAddress, const std::vector<UInt32>& data, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			// 浮点寄存器
			std::future<std::pair<float, bool>> readFloatRegisterAsync(Address16 startAddress, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<std::pair<std::vector<float>, bool>> readFloatRegistersAsync(Address16 startAddress, size_t count, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeFloatRegisterAsync(Address16 startAddress, float value, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeFloatRegistersAsync(Address16 startAddress, const std::vector<float>& data, Endianness byteOrder, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

			// 线圈
			std::future<std::pair<std::vector<bool>, bool>> readCoilsAsync(Address16 startAddress, Quantity quantity, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<std::pair<bool, bool>> readCoilAsync(Address16 startAddress, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeCoilAsync(Address16 address, bool state, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;
			std::future<bool> writeCoilsAsync(Address16 startAddress, const std::vector<bool>& states, int prio = 8, std::optional<std::chrono::milliseconds> timeout = std::nullopt) noexcept;

		private:
			struct TaskItem
			{
				int priority;
				uint64_t sequence;
				std::function<void()> work;
			};

			struct TaskCompare
			{
				bool operator()(const TaskItem& lhs, const TaskItem& rhs) const
				{
					if (lhs.priority == rhs.priority) {
						return lhs.sequence > rhs.sequence;
					}
					return lhs.priority > rhs.priority; // 优先级数值越小，优先级越高
				}
			};

		private:
			template<typename Callable, typename Result>
			std::future<Result> enqueueTask(int prio, std::optional<std::chrono::milliseconds> timeout, Callable&& callable, Result fallback) noexcept;

			void workerLoop() noexcept;

		private:
			std::shared_ptr<ModbusDevice> _device;
			std::priority_queue<TaskItem, std::vector<TaskItem>, TaskCompare> _tasks;
			std::mutex _queueMutex;
			std::condition_variable _cv;
			std::thread _worker;
			std::atomic<bool> _running{ true };
			std::atomic<uint64_t> _sequence{ 0 };
			std::atomic<uint64_t> _pendingTasks{ 0 };
		};

		template<typename Callable, typename Result>
		std::future<Result> ModbusDeviceScheduler::enqueueTask(int prio, std::optional<std::chrono::milliseconds> timeout, Callable&& callable, Result fallback) noexcept
		{
			auto promisePtr = std::make_shared<std::promise<Result>>();
			auto fut = promisePtr->get_future();
			auto done = std::make_shared<std::atomic<bool>>(false);

			auto taskWrapper = [this, callable = std::forward<Callable>(callable), promisePtr, done, fallback]() mutable noexcept
				{
					if (!this->_device) {
						bool expected = false;
						if (done->compare_exchange_strong(expected, true)) {
							promisePtr->set_value(fallback);
						}
						return;
					}
					try {
						Result r = callable();
						bool expected = false;
						if (done->compare_exchange_strong(expected, true)) {
							promisePtr->set_value(std::move(r));
						}
					}
					catch (...) {
						bool expected = false;
						if (done->compare_exchange_strong(expected, true)) {
							promisePtr->set_value(fallback);
						}
					}
				};

			{
				std::lock_guard<std::mutex> lock(_queueMutex);
				if (!_running) {
					promisePtr->set_value(fallback);
					return fut;
				}
				_pendingTasks.fetch_add(1);
				_tasks.push(TaskItem{ prio, _sequence.fetch_add(1), std::move(taskWrapper) });
			}
			_cv.notify_one();

			if (timeout.has_value()) {
				std::thread([timeout, promisePtr, done, fallback]()
					{
						std::this_thread::sleep_for(*timeout);
						bool expected = false;
						if (done->compare_exchange_strong(expected, true)) {
							promisePtr->set_value(fallback);
						}
					}).detach();
			}

			return fut;
		}
	}
}