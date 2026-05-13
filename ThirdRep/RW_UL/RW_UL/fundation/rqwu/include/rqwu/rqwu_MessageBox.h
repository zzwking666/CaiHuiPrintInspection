#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

namespace rw
{
	namespace rqwu
	{
		class MessageBox : public QDialog
		{
			Q_OBJECT
		public:
			enum class Icon {
				NoIcon,
				Information,
				Warning,
				Critical,
				Question
			};

			enum class StandardButton {
				NoButton = 0x00000000,
				Ok = 0x00000400,
				Cancel = 0x00400000,
				Yes = 0x00004000,
				No = 0x00010000
			};
		public:

			explicit MessageBox(QWidget* parent = nullptr);
			~MessageBox() = default;

			void setText(const QString& text);
			void setIcon(Icon icon);
			void setStandardButtons(int buttons);

			static StandardButton information(QWidget* parent, const QString& title, const QString& text,
				int buttons = static_cast<int>(StandardButton::Ok));
			static StandardButton warning(QWidget* parent, const QString& title, const QString& text,
				int buttons = static_cast<int>(StandardButton::Ok));
			static StandardButton critical(QWidget* parent, const QString& title, const QString& text,
				int buttons = static_cast<int>(StandardButton::Ok));
			static StandardButton question(QWidget* parent, const QString& title, const QString& text,
				int buttons = (static_cast<int>(StandardButton::Yes) | static_cast<int>(StandardButton::No)));

		private:
			void setupUI();
			void updateIcon();

			QLabel* iconLabel;
			QLabel* textLabel;
			QHBoxLayout* buttonLayout;
			QVBoxLayout* mainLayout;
			Icon currentIcon;
			int result;

		private slots:
			void onButtonClicked();
		};

		// 位运算符重载(可选,放在类外)
		inline int operator|(MessageBox::StandardButton a, MessageBox::StandardButton b)
		{
			return static_cast<int>(a) | static_cast<int>(b);
		}
	}
}

