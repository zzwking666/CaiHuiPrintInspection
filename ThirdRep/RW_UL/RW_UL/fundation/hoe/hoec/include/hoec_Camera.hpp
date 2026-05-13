#ifndef HOEC_ICAMERA_H_
#define HOEC_ICAMERA_H_

#include <string>
#include<functional>

namespace cv {
	class Mat;
}
namespace rw
{
	namespace hoec
	{
		class CameraFactory;

		struct OutTriggerConfig
		{
		public:
			size_t lineSelector{ 0 };
			size_t lineMode{ 0 };
			size_t lineSource{ 0 };
			long durationValue{ 0 };
			size_t delayValue{ 0 };
			size_t preDelayValue{ 0 };
			bool strobeEnable = false;
		};

		struct CameraInfo
		{
			std::string ip;
			std::string name;
			std::string mac;
		};

		/**
		 * @brief This enum class is used to specify the mode of taking a picture.
		 *
		 * This enum class is used to specify the mode of taking a picture. The mode is mutually exclusive
		 */
		enum class CameraTakePictureMode
		{
			// Take a picture in the active mode.
			// When use this mode, If you want to get a picture, you should call the getImage() function.
			Active,

			// Take a picture in the passive mode.
			// When use this mode, If you want to get a picture, you should register a callback function.
			// When the camera get a picture, the callback function will be called.
			// The callback function has default realization, you can also register your own callback function
			//when you construct the camera object.
			Passive
		};

		/**
		 * @brief This enum class is used to specify the mode of monitoring the camera.
		 *
		 * This enum class is used to specify the mode of monitoring the camera. The mode is mutually exclusive
		 */
		enum class CameraTriggerMode
		{
			// Monitor the camera in the software triggered mode.
			// When use this mode, the camera will be monitored by the software trigger.
			// You can set frame rate or get a picture by calling the getImage() function.
			SoftwareTriggered,

			// Monitor the camera in the hardware triggered mode.
			// When use this mode, the camera will be monitored by the hardware trigger.
			// You can set the trigger line and trigger mode by calling the setTriggerLine() function.
			HardwareTriggered,
		};

		/**
		 * @brief This enum class is used to specify the provider of the camera.
		 *
		 * This enum class is used to specify the provider of the camera. The provider is mutually exclusive
		 */
		enum class CameraProvider
		{
			// The camera is provided by the HIKVISION.
			MVS
		};
		inline CameraProvider  from_string(const std::string& string)
		{
			if (string == "MVS")
			{
				return CameraProvider::MVS;
			}
			return CameraProvider::MVS;
		}

		inline const char* to_string(CameraProvider e)
		{
			switch (e)
			{
			case CameraProvider::MVS: return "MVS";
			default: return "unknown";
			}
		}

		/**
		 * @brief This class is used to provide the interface of the camera.
		 *
		 * For different camera providers, you should implement the ICamera interface.
		 */
		class ICamera
		{
		protected:
			CameraInfo _cameraInfo;
			std::string _ip;
		public:

			/**
			 * @brief Destructor
			 *
			 * For destructor,you should guarantee that when object is destructed,
			 *all hardware resource of the camera should be released.
			 */
			virtual ~ICamera() = default;

			/**
			 * @brief Constructor
			 *
			 * For constructor,you should guarantee that when object is constructed,
			 *all hardware resource of the camera has been ready such as initialize SDK resource.
			 */
			ICamera() = default;

			/**
			 * @brief Delete the copy constructor and copy assignment operator.
			 *
			 * The camera object is entity object,they are global unique,for each camera object,
			 *it take over every hardware resource of the camera.
			 */
			ICamera(ICamera&&) = delete;
			ICamera& operator=(ICamera&&) = delete;
			ICamera(const ICamera&) = delete;
			ICamera& operator=(const ICamera&) = delete;

		public:
			/**
			 *@Parameters:
			 *  - ip: the ip of the camera
			 *@Methods:
			 *  Set the ip of the camera.
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			void setIP(const std::string& ip);

			/**
			 *@Parameters:
			 *
			 *@Methods:
			 *  Get the ip of the camera.
			 *@Returns: the ip of the camera
			 *
			 *@Throws:
			 *
			 */
			[[nodiscard]] std::string getIP() const;

		public:

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Connect the camera,in this call, it will exclusive the camera resource,and open
			 * device. It also initializes the camera resource and get the camera handle.
			 *@Returns: bool
			 *  If connect the camera successfully, return true,otherwise return false.
			 *@Throws:
			 *   CameraConnectionError: If failed to connect the camera, throw the CameraConnectionError.
			 *   Below is the reason:
			 *      - Failed to enumerate devices
			 *      - Failed to find target device
			 *      - Failed to access device
			 *      - Failed to create handle
			 *      - Failed to open device
			 *
			 */
			virtual void connectCamera() = 0;

			virtual bool getConnectState() = 0;

		public:

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Start the camera monitor, the camera will start to monitor the image.
			 *@Returns: bool
			 *  If start the camera monitor successfully, return true,otherwise return false.
			 *@Throws:
			 *  CameraMonitorError: If failed to start the camera monitor, throw the CameraMonitorError.
			 *  Below is the reason:
			 *      - Monitor has already started
			 *      - Failed to start grabbing
			 *@Warning:
			 * If you want stop the camera monitor,you can call the stopMonitor() function
			 *or destruct the camera object which will stop the camera monitor automatically.
			 */
			virtual void startMonitor() = 0;

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Stop the camera monitor, the camera will stop to monitor the image.
			 *@Returns: bool
			 *  If stop the camera monitor successfully, return true,otherwise return false.
			 *@Throws:
			 *  CameraMonitorError: If failed to stop the camera monitor, throw the CameraMonitorError.
			 *  Below is the reason:
			 *      - Monitor has already stopped
			 *      - Failed to stop grabbing
			 *
			 */
			virtual void stopMonitor() = 0;

		public:
			CameraInfo getCameraInfo();

		protected:
			/**
			 *@Parameters:
			 *  - cameraInfo: The information of the camera you want to set.
			 *@Methods:
			 *  When connect the camera, you should set the camera information in connectCamera() function.
			 *@Returns: types
			 *  returns
			 *@Throws:
			 *  throws
			 */
			void setCameraInfo(const CameraInfo& cameraInfo);
		public:
			virtual void setHeartbeatTime(size_t heartBeatTime) = 0;

			virtual float getHeartbeatTime() = 0;

			virtual void setFrameRate(float cameraFrameRate) = 0;
			virtual float getFrameRate() = 0;

			/**
			 *@Parameters:
			 *  - value: The value of the exposure time you want to set.
			 *@Methods:
			 *  Set the exposure time of the camera.
			 *@Returns: bool
			 *  If set the exposure time successfully, return true,otherwise return false.
			 *@Throws:
			 *  CameraSettingError: If failed to set the exposure time, throw the CameraSettingError.
			 *  Below is the reason:
			 *      - Failed to set exposure time
			 *@Warning:
			 * For some camera, the exposure time has a range, you should set the value in the range.
			 *For this function,we guarantee that when you set the exposure,whatever the value is,if
			 *the value will be agreed,it will return true,otherwise return false.
			 */
			virtual void setExposureTime(size_t value) = 0;

			/**
			 *@Parameters:
			 *  - value: The value of the gain you want to set.
			 *@Methods:
			 *  set the gain of the camera.
			 *@Returns: bool
			 *  If set the gain successfully, return true,otherwise return false.
			 *@Throws:
			 *  CameraSettingError: If failed to set the gain, throw the CameraSettingError.
			 *  Below is the reason:
			 *      - Failed to set gain
			 *
			 *@Warning:
			 * For some camera, the gain has a range, you should set the value in the range.
			 *For this function,we guarantee that when you set the gain,whatever the value is,if
			 *the value will be agreed,it will return true,otherwise return false.
			 */
			virtual void setGain(size_t value) = 0;

			/**
			 *@Parameters:
			 *  - mode: The mode of the camera monitor you want to set.
			 *      type: CameraMonitorMode
			 *@Methods:
			 *  Set the monitor mode of the camera.
			 *@Returns: bool
			 *  If set the monitor mode successfully, return true,otherwise return false.
			 *@Throws:
			 *  CameraSettingError: If failed to set the monitor mode, throw the CameraSettingError.
			 *  Below is the reason:
			 *      - Failed to set monitor mode
			 *
			 */
			virtual void setTriggerMode(CameraTriggerMode mode) = 0;

			/**
			 *@Parameters:
			 *  - lineIndex: The index of the trigger line you want to set.
			 *@Methods:
			 *  Set the trigger line of the camera which will be used in the hardware trigger mode.
			 *@Returns: bool
			 *  If set the trigger line successfully, return true,otherwise return false.
			 *@Throws:
			 *  CameraSettingError: If failed to set the trigger line, throw the CameraSettingError.
			 *  Below is the reason:
			 *      - Failed to set trigger line
			 *
			 */
			virtual void setInTriggerLine(size_t lineIndex) = 0;

		public:

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the exposure time of the camera.
			 *@Returns: void
			 *
			 *@Throws:
			 *  CameraRetrievalError: If failed to get the exposure time, throw the CameraRetrievalError.
			 *  Below is the reason:
			 *      - Failed to get exposure time
			 *
			 */
			[[nodiscard]] virtual size_t getExposureTime() = 0;

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the gain of the camera.
			 *@Returns: void
			 *
			 *@Throws:
			 *  CameraRetrievalError: If failed to get the gain, throw the CameraRetrievalError.
			 *  Below is the reason:
			 *      - Failed to get gain
			 */
			[[nodiscard]] virtual size_t getGain() = 0;

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the monitor mode of the camera.
			 *@Returns: void
			 *
			 *@Throws:
			 *  CameraRetrievalError: If failed to get the monitor mode, throw the CameraRetrievalError.
			 *  Below is the reason:
			 *      - Failed to get monitor mode
			 */
			[[nodiscard]] virtual CameraTriggerMode getMonitorMode() = 0;

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the trigger line of the camera.
			 *@Returns: void
			 *
			 *@Throws:
			 *  CameraRetrievalError: If failed to get the trigger line, throw the CameraRetrievalError.
			 *  - Failed to get trigger line
			 *
			 */
			[[nodiscard]] virtual size_t getTriggerLine() = 0;
		public:
			virtual void setOutTriggerConfig(const OutTriggerConfig& config) = 0;
			virtual void outTrigger() = 0;
			virtual void outTrigger(bool isOpen) = 0;
		};

		class ICameraActive
		{
		public:
			virtual ~ICameraActive() = default;
		public:

			/**
			 *@Parameters:
			 *  - isget: the flag of the image is get or not
			 *@Methods:
			 *  Get the image from the camera.
			 *@Returns: cv::mat
			 *  For all camera, the image is a cv::mat object.
			 *@Throws:
			 *
			 */
			[[nodiscard]] virtual cv::Mat getImage(bool& isGet) = 0;

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the image from the camera.
			 *@Returns: cv::mat
			 *  For all camera, the image is a cv::mat object.
			 *@Throws:
			 *
			 */
			[[nodiscard]] virtual cv::Mat getImage() = 0;
		};

		class ICameraPassive
		{
		public:
			using UserToCallBack = std::function<void(cv::Mat)>;
		public:
			virtual ~ICameraPassive() = default;

		public:
			/**
			 * @brief User defined callback function
			 *
			 * When the camera get a picture, the callback function will be called.
			 */
			using UserToCallBack = std::function<void(cv::Mat)>;
		public:

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Register the callback function.
			 *@Returns: bool
			 *  If register the callback function successfully, return true,otherwise return false.
			 *@Throws:
			 *
			 */
			virtual void RegisterCallBackFunc() = 0;
		};

		class CameraActive
			:public ICameraActive, public ICamera
		{
			friend class CameraFactory;
		protected:
			void connectCamera() override;
		public:
			bool getConnectState()override;
			void setFrameRate(float cameraFrameRate) override;
		public:
			void setHeartbeatTime(size_t heartBeatTime)override;
			float getHeartbeatTime() override;
			float getFrameRate() override;
			void startMonitor() override;
			void stopMonitor() override;
			void setExposureTime(size_t value) override;
			void setGain(size_t value) override;
			void setTriggerMode(CameraTriggerMode mode) override;
			void setInTriggerLine(size_t lineIndex) override;
			[[nodiscard]] size_t getExposureTime() override;
			[[nodiscard]] size_t getGain() override;
			[[nodiscard]] CameraTriggerMode getMonitorMode() override;
			[[nodiscard]] size_t getTriggerLine() override;
		public:
			[[nodiscard]] cv::Mat getImage(bool& isGet) override;
			[[nodiscard]] cv::Mat getImage() override;
		private:
			CameraProvider _provider;
		public:
			void setCameraProvider(CameraProvider provider);
			CameraProvider getCameraProvider() const;
		private:
			ICamera* _camera{ nullptr };
			ICameraActive* _cameraActive{ nullptr };
		public:
			CameraActive(ICamera* camera, ICameraActive* cameraActive);
		public:
			~CameraActive() override;
		public:
			virtual void setOutTriggerConfig(const OutTriggerConfig& config) override;
			virtual void outTrigger() override;
			virtual void outTrigger(bool isOpen)override;
		};

		class CameraPassive
			:public ICameraPassive, public ICamera
		{
			friend class CameraFactory;
		public:
			UserToCallBack _userToCallBack;
		protected:
			void connectCamera() override;
		public:
			bool getConnectState()override;
			void setFrameRate(float cameraFrameRate) override;
		public:
			void setHeartbeatTime(size_t heartBeatTime)override;
			float getHeartbeatTime() override;
			float getFrameRate() override;
			void startMonitor() override;
			void stopMonitor() override;
			void setExposureTime(size_t value) override;
			void setGain(size_t value) override;
			void setTriggerMode(CameraTriggerMode mode) override;
			void setInTriggerLine(size_t lineIndex) override;
			size_t getExposureTime() override;
			size_t getGain() override;
			CameraTriggerMode getMonitorMode() override;
		public:
			size_t getTriggerLine() override;
			void RegisterCallBackFunc() override;
		private:
			CameraProvider _provider;
		public:
			void setCameraProvider(CameraProvider provider);
			CameraProvider getCameraProvider() const;
		private:
			ICamera* _camera{ nullptr };
			ICameraPassive* _cameraPassive{ nullptr };
		public:
			CameraPassive(ICamera* camera, ICameraPassive* cameraPassive, UserToCallBack userToCallBack);
		public:
			~CameraPassive() override;
		public:
			virtual void setOutTriggerConfig(const OutTriggerConfig& config) override;
			virtual void outTrigger() override;
			virtual void outTrigger(bool isOpen)override;
		};
	}
}

#endif