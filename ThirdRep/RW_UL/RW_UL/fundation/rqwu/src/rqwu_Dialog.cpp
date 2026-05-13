#include"rqwu/rqwu_Dialog.h"

namespace rw
{
	namespace rqwu
	{
		Dialog::Dialog(QWidget* parent)
		{
			// Set frameless window
			Qt::WindowFlags flags = windowFlags();
			flags |= Qt::FramelessWindowHint;
			setWindowFlags(flags);
		}

		Dialog::~Dialog()
		{
		}
	}

}
