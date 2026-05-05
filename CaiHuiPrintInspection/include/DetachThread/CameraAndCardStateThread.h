#pragma once
#include <QThread>
#include <QDebug>
#include <atomic>

class CameraAndCardStateThread : public QThread
{
	Q_OBJECT
private:
	static size_t runtimeCounts;

public:
	explicit CameraAndCardStateThread(QObject* parent = nullptr);

	~CameraAndCardStateThread() override;

	void startThread();

	void stopThread();
protected:
	void run() override;
private:
	void check_cameraState();
	void check_cameraState1();
	void check_cameraState2();
signals:
	void updateCameraLabelState(int cameraIndex, bool state);
signals:
	void buildCamera(int index);

	void destroyCamera(int index);

	void startMonitor(int index);
private:
	std::atomic<bool> running; // 使用原子变量保证线程安全
};