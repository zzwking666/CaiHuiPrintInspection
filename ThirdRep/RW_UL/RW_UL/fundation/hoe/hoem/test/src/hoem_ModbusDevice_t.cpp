#include "hoem_ModbusDevice.hpp"
#include"hoem_pch_t.hpp"
#include"hoem_ModbusDeviceScheduler.hpp"

#include <thread>
#include <vector>
#include <atomic>
#include <future>
#include <chrono>
#include <iostream>

namespace hoem_ModbusDevice
{
	TEST(ModbusDeviceTest, HighConcurrencyStress)
	{
		rw::hoem::ModbusDeviceTcpCfg cfg;
		cfg.ip = "192.168.1.199";
		cfg.port = 502;

		auto sharedPtr = std::make_shared<rw::hoem::ModbusDevice>(cfg);

		auto connectResult = sharedPtr->connect();
		ASSERT_TRUE(connectResult);

		rw::hoem::ModbusDeviceScheduler scheduler(sharedPtr);

		// 高并发参数
		const int readerThreads = 12;
		const int writerThreads = 6;
		const int readIterations = 20000;
		const int writeIterations = 5000;
		const rw::hoem::Address16 readAddress = 6000;
		const rw::hoem::Address16 coilAddress = 35023;
		const rw::hoem::Quantity coilReadQuantity = static_cast<rw::hoem::Quantity>(4);
		const auto readTimeout = std::optional<std::chrono::milliseconds>{ std::chrono::milliseconds(300) };
		const auto writeTimeout = std::optional<std::chrono::milliseconds>{ std::chrono::milliseconds(300) };

		std::atomic<bool> stop{ false };
		std::atomic<int> readErrors{ 0 };
		std::atomic<int> writeErrors{ 0 };
		std::atomic<bool> connectionLost{ false };
		std::atomic<int> reconnectAttempts{ 0 };
		std::atomic<int> readDone{ 0 };
		std::atomic<int> writeDone{ 0 };

		// 同步起跑栅栏（C++17 使用 promise/future 实现）
		std::promise<void> startPromise;
		auto startFuture = startPromise.get_future();

		// 读线程：混合读取寄存器与线圈
		std::vector<std::thread> readers;
		for (int i = 0; i < readerThreads; ++i)
		{
			readers.emplace_back([&, i]()
				{
					startFuture.wait();
					std::cout << "[Reader#" << i << "] start" << std::endl;
					for (int it = 0; it < readIterations && !stop.load(); ++it)
					{
						try
						{
							auto futReg = scheduler.readUInt32RegisterAsync(readAddress, rw::hoem::Endianness::LittleEndian, 2, readTimeout);
							auto regRes = futReg.get();
							if (!regRes.second)
							{
								++readErrors;
							}

							auto futCoil = scheduler.readCoilsAsync(coilAddress, coilReadQuantity, 5, readTimeout);
							auto coilRes = futCoil.get();
							if (!coilRes.second)
							{
								++readErrors;
							}
						}
						catch (...)
						{
							++readErrors;
						}
						int done = ++readDone;
						if ((done % 1000) == 0)
						{
							std::cout << "[Reader#" << i << "] progress=" << done
								<< " readErrors=" << readErrors.load()
								<< " writeErrors=" << writeErrors.load()
								<< " reconnectAttempts=" << reconnectAttempts.load()
								<< std::endl;
						}
						if ((it % 256) == 0)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
					}
				});
		}

		// 写线程：交替写线圈与 32 位寄存器
		std::vector<std::thread> writers;
		for (int i = 0; i < writerThreads; ++i)
		{
			writers.emplace_back([&, i]()
				{
					startFuture.wait();
					std::cout << "[Writer#" << i << "] start" << std::endl;
					bool state = (i % 2 == 0);
					rw::hoem::UInt32 value = 0x5AA55AA5u + static_cast<rw::hoem::UInt32>(i);
					for (int it = 0; it < writeIterations && !stop.load(); ++it)
					{
						try
						{
							auto futCoil = scheduler.writeCoilAsync(static_cast<rw::hoem::Address16>(coilAddress + (i % 2)), state, 3, writeTimeout);
							auto futReg = scheduler.writeUInt32RegisterAsync(readAddress, value, rw::hoem::Endianness::LittleEndian, 4, writeTimeout);
							bool okCoil = futCoil.get();
							bool okReg = futReg.get();
							if (!okCoil || !okReg)
							{
								++writeErrors;
							}
						}
						catch (...)
						{
							++writeErrors;
						}
						int done = ++writeDone;
						if ((done % 500) == 0)
						{
							std::cout << "[Writer#" << i << "] progress=" << done
								<< " readErrors=" << readErrors.load()
								<< " writeErrors=" << writeErrors.load()
								<< " reconnectAttempts=" << reconnectAttempts.load()
								<< std::endl;
						}
						state = !state;
						value ^= 0xAAAAAAAAu;
						if ((it % 128) == 0)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
					}
				});
		}

		// 监视线程：高频检测并尝试重连
		std::thread monitor([&]()
			{
				startFuture.wait();
				std::cout << "[Monitor] start" << std::endl;
				const int maxReconnectAttempts = 6;
				for (int i = 0; i < 400000 && !stop.load(); ++i)
				{
					bool isConnected = scheduler.isDeviceConnected();
					if (!isConnected)
					{
						bool reconnected = false;
						for (int attempt = 1; attempt <= maxReconnectAttempts && !stop.load(); ++attempt)
						{
							++reconnectAttempts;
							try
							{
								if (scheduler.reconnectDevice())
								{
									reconnected = true;
									break;
								}
							}
							catch (...)
							{
							}
							auto backoff = std::chrono::milliseconds(50 * (1 << (attempt - 1)));
							std::this_thread::sleep_for(backoff);
						}
						if (!reconnected)
						{
							connectionLost.store(true);
							stop.store(true);
							break;
						}
					}
					if ((i % 1000) == 0)
					{
						std::cout << "[Monitor] tick=" << i
							<< " readDone=" << readDone.load()
							<< " writeDone=" << writeDone.load()
							<< " readErrors=" << readErrors.load()
							<< " writeErrors=" << writeErrors.load()
							<< " reconnectAttempts=" << reconnectAttempts.load()
							<< " connected=" << isConnected
							<< std::endl;
					}
					if ((i % 64) == 0)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}
			});

		// 同步启动
		startPromise.set_value();

		for (auto& t : readers) if (t.joinable()) t.join();
		for (auto& t : writers) if (t.joinable()) t.join();

		stop.store(true);
		if (monitor.joinable()) monitor.join();

		// 等待调度器内部队列完成
		scheduler.waitAll();

		std::cout << "[HighConcurrencyStress] readErrors=" << readErrors.load()
			<< " writeErrors=" << writeErrors.load()
			<< " connectionLost=" << connectionLost.load()
			<< " reconnectAttempts=" << reconnectAttempts.load()
			<< " readDone=" << readDone.load()
			<< " writeDone=" << writeDone.load()
			<< std::endl;

		ASSERT_FALSE(connectionLost.load());
		ASSERT_EQ(readErrors.load(), 0);
		ASSERT_EQ(writeErrors.load(), 0);
	}
}