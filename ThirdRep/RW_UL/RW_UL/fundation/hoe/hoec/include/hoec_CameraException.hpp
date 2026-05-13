#pragma once

#include <exception>
#include <string>
#include <utility>

namespace rw
{
	namespace hoec {
		class CameraException : public std::exception {
		public:
			explicit CameraException(std::string message) : _msg(std::move(message)) {}

			virtual const char* what() const noexcept override {
				return _msg.c_str();
			}

		private:
			std::string _msg;
		};

		class CameraRuntimeError : public CameraException {
		public:
			explicit CameraRuntimeError(std::string message) : CameraException(std::move(message)) {}
		};

		class CameraInitError : public CameraRuntimeError {
		public:
			explicit CameraInitError(std::string message) : CameraRuntimeError(std::move(message)) {}
		};

		class CameraConnectionError : public CameraRuntimeError {
		public:
			explicit CameraConnectionError(std::string message) : CameraRuntimeError(std::move(message)) {}
		};

		class CameraSettingError : public CameraRuntimeError {
		public:
			explicit CameraSettingError(std::string message) : CameraRuntimeError(std::move(message)) {}
		};

		class CameraRetrievalError : public CameraRuntimeError {
		public:
			explicit CameraRetrievalError(std::string message) : CameraRuntimeError(std::move(message)) {}
		};

		class CameraMonitorError : public CameraRuntimeError {
		public:
			explicit CameraMonitorError(std::string message) : CameraRuntimeError(std::move(message)) {}
		};

		class CameraCreateError : public CameraRuntimeError {
		public:
			explicit CameraCreateError(std::string message) : CameraRuntimeError(std::move(message)) {}
		};
	}
}