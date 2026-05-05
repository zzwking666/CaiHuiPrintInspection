#pragma once

#include <QCheckBox>
#include <QObject>
#include "IModule.hpp"

class MaiLiDingZi;
class DlgProductSet;

class UIModule
	: public QObject, public IModule<void>
{
	Q_OBJECT
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:

public:
	DlgProductSet* _dlgProductSet = nullptr;
	MaiLiDingZi* _maiLiDingZi = nullptr;
};
