#include "UIModule.hpp"
#include "DlgProductSet.h"
#include "CaiHuiPrintInspection.h"
#include "ui_DlgProductSet.h"

void UIModule::build()
{
	_dlgProductSet = new DlgProductSet();
}

void UIModule::destroy()
{
	if (_dlgProductSet)
	{
		delete _dlgProductSet;
		_dlgProductSet = nullptr;
	}
}

void UIModule::start()
{
}

void UIModule::stop()
{
}