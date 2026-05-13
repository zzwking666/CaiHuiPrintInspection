#pragma once

#include <QDialog>

namespace rw
{
	namespace rqwu
	{
		class Dialog : public QDialog
		{
		public:
			Dialog(QWidget* parent = nullptr);
			~Dialog() override;
		};

	}

}
