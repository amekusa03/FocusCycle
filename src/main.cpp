#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QFont>
#include "app/Application.h"

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("FocusCycle");
    app.setApplicationDisplayName("FocusCycle");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("FocusCycle");

    app.setQuitOnLastWindowClosed(false);

    QPalette dark;
    dark.setColor(QPalette::Window,          QColor(0x1a, 0x1b, 0x2e));
    dark.setColor(QPalette::WindowText,      QColor(0xea, 0xea, 0xea));
    dark.setColor(QPalette::Base,            QColor(0x16, 0x21, 0x3e));
    dark.setColor(QPalette::AlternateBase,   QColor(0x1a, 0x1b, 0x2e));
    dark.setColor(QPalette::Text,            QColor(0xea, 0xea, 0xea));
    dark.setColor(QPalette::Button,          QColor(0x2a, 0x2b, 0x4e));
    dark.setColor(QPalette::ButtonText,      QColor(0xea, 0xea, 0xea));
    dark.setColor(QPalette::Highlight,       QColor(0x4a, 0x90, 0xe2));
    dark.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
    dark.setColor(QPalette::ToolTipBase,     QColor(0x1a, 0x1b, 0x2e));
    dark.setColor(QPalette::ToolTipText,     QColor(0xea, 0xea, 0xea));
    app.setPalette(dark);

    QFont defaultFont("Sans-Serif", 10);
    defaultFont.setStyleHint(QFont::SansSerif);
    app.setFont(defaultFont);

    Application focusApp;
    focusApp.initialize();

    return app.exec();
}
