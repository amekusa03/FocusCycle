QT       += core gui widgets dbus
TARGET    = FocusCycle
TEMPLATE  = app
CONFIG   += c++17

# Build output directories
OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
RCC_DIR     = build/rcc
UI_DIR      = build/ui

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/app/Application.cpp \
    src/core/Settings.cpp \
    src/core/SessionLogger.cpp \
    src/core/IdleMonitor.cpp \
    src/timer/CycleController.cpp \
    src/ui/CountdownRing.cpp \
    src/ui/WorkStartWindow.cpp \
    src/ui/BreakDialog.cpp \
    src/ui/CompanyBreakDialog.cpp \
    src/ui/LogWindow.cpp \
    src/ui/SettingsDialog.cpp \
    src/ui/TrayIcon.cpp

HEADERS += \
    src/app/Application.h \
    src/core/AppState.h \
    src/core/Settings.h \
    src/core/SessionLogger.h \
    src/core/IdleMonitor.h \
    src/timer/CycleController.h \
    src/ui/CountdownRing.h \
    src/ui/WorkStartWindow.h \
    src/ui/BreakDialog.h \
    src/ui/CompanyBreakDialog.h \
    src/ui/LogWindow.h \
    src/ui/SettingsDialog.h \
    src/ui/TrayIcon.h

RESOURCES += resources/resources.qrc

# X11 XScreenSaver (optional fallback)
unix:!macx {
    packagesExist(xscrnsaver) {
        DEFINES += HAVE_X11SS
        CONFIG  += link_pkgconfig
        PKGCONFIG += xscrnsaver
        QT += x11extras
    }
}
