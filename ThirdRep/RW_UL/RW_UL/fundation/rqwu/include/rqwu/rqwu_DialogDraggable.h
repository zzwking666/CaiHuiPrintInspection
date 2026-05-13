#pragma once

#include "rqwu/rqwu_Dialog.h"

namespace rw
{
	namespace rqwu
	{
		class DialogDraggable : public rqwu::Dialog
		{
			Q_OBJECT
		public:
			DialogDraggable(QWidget* parent = nullptr);
			~DialogDraggable() override;
		private:
			QPointF m_dragPos;
			bool m_dragging{ false };
		protected:
			void mousePressEvent(QMouseEvent* event) final;
			void mouseMoveEvent(QMouseEvent* event) final;
			void mouseReleaseEvent(QMouseEvent* event) final;
		};
	}
}
