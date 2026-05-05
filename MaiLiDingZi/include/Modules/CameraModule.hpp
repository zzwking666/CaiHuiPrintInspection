#pragma once
#include <memory>
#include <QObject>

#include "rqw_CameraObjectThread.hpp"
#include"IModule.hpp"

enum BuildError
{
	Camera1Error = 1,
	Camera2Error = 2
};

class CameraModule
	: public QObject, public IModule<std::vector<BuildError>>
{
	Q_OBJECT
public:
	CameraModule();
	~CameraModule() override;
public:
	std::vector<BuildError> build() override;
	void destroy() override;
public:
	void start() override;
	void stop() override;
private:
	bool build_camera1();
	void destroy_camera1();

	bool build_camera2();
	void destroy_camera2();
private:
	std::vector<BuildError> _buildResults;
public:
	std::vector<BuildError> getBuildResults() { return _buildResults; };

private:
	bool isTargetCamera(const QString& cameraIndex, const QString& targetName);
	rw::rqw::CameraMetaData cameraMetaDataCheck(const QString& cameraIndex,
		const QVector<rw::rqw::CameraMetaData>& cameraInfo);
public slots:
	bool onBuildCamera(int index);
	void onDestroyCamera(int index);
	void onStartCamera(int index);

	void onFrameCaptured(rw::rqw::MatInfo frame, size_t index);
signals:
	void frameCaptured1(rw::rqw::MatInfo matInfo, size_t index);
	void frameCaptured2(rw::rqw::MatInfo matInfo, size_t index);
public:
	std::unique_ptr<rw::rqw::CameraPassiveThread> camera1{ nullptr };
	std::unique_ptr<rw::rqw::CameraPassiveThread> camera2{ nullptr };
};
