#include"rqwu/rqwu_DialogDraggable.h"

#include <QMouseEvent>

namespace rw
{
	namespace rqwu
	{
		DialogDraggable::DialogDraggable(QWidget* parent)
			:rqwu::Dialog(parent)
		{
			setAttribute(Qt::WA_TranslucentBackground, true);
		}

		DialogDraggable::~DialogDraggable()
		{

		}

		void DialogDraggable::mousePressEvent(QMouseEvent* event)
		{
			if (event->button() == Qt::LeftButton) {
				m_dragging = true;
				m_dragPos = event->globalPosition() - frameGeometry().topLeft();
				event->accept();
			}
			else {
				QDialog::mousePressEvent(event);
			}
		}

		void DialogDraggable::mouseMoveEvent(QMouseEvent* event)
		{
			if (m_dragging && (event->buttons() & Qt::LeftButton)) {
				move((event->globalPosition() - m_dragPos).toPoint());
				event->accept();
			}
			else {
				QDialog::mouseMoveEvent(event);
			}
		}

		void DialogDraggable::mouseReleaseEvent(QMouseEvent* event)
		{
			Q_UNUSED(event);
			m_dragging = false;
		}
	}
}
