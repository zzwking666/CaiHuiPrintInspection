#pragma once

#include "rqwm_ModbusUtilty.hpp"
#include <QThread>
#include <memory>
#include <shared_mutex>
#include "rqwm_ModbusDevice.hpp"

namespace rw::rqwm {
	class ModbusDeviceThreadSafe : public QObject {
		Q_OBJECT
	public:
		explicit ModbusDeviceThreadSafe(const ModbusType& type, const ModbusConfig& config, QObject* parent = nullptr);
		~ModbusDeviceThreadSafe() override;

		bool connect();
		bool disconnect();
		bool reconnect();
		[[nodiscard]] bool isConnected() const;
		[[nodiscard]] bool getIState(ModbusI locate) const;
		bool setOState(ModbusO locate, bool state);
		[[nodiscard]] bool getOState(ModbusO locate) const;

	private:
		std::unique_ptr<ModbusDevice> _device;
		mutable std::mutex _mutex;
	};
} // namespace rw::rqwm