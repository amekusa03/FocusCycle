#include "IdleMonitor.h"
#include <QProcessEnvironment>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

#ifdef HAVE_X11SS
#  include <X11/extensions/scrnsaver.h>
#  include <QX11Info>
#endif

IdleMonitor::IdleMonitor(QObject* parent) : QObject(parent)
{
    // Detect whether the session is running under Wayland.
    QString waylandDisplay =
        QProcessEnvironment::systemEnvironment().value("WAYLAND_DISPLAY");
    m_waylandMode = !waylandDisplay.isEmpty();

    // Pre-create the D-Bus interface once so that the 1-second poll()
    // does not repeatedly construct/validate it on every call.
    if (m_waylandMode) {
        m_dbusIface = new QDBusInterface(
            "org.gnome.Mutter.IdleMonitor",
            "/org/gnome/Mutter/IdleMonitor/Core",
            "org.gnome.Mutter.IdleMonitor",
            QDBusConnection::sessionBus(),
            this);
        if (!m_dbusIface->isValid()) {
            qWarning() << "[IdleMonitor] Mutter.IdleMonitor D-Bus unavailable;"
                       << "idle detection will fall back to X11 (if available).";
        }
    }

    m_timer.setInterval(1000); // 1-second polling
    connect(&m_timer, &QTimer::timeout, this, &IdleMonitor::poll);
}

void IdleMonitor::start(int thresholdSeconds)
{
    if (thresholdSeconds <= 0) {
        // Threshold of 0 means "detect immediately".
        emit idleDetected();
        return;
    }
    m_thresholdMs = static_cast<qint64>(thresholdSeconds) * 1000LL;
    m_timer.start();
}

void IdleMonitor::stop()
{
    m_timer.stop();
}

qint64 IdleMonitor::currentIdleMs() const
{
    return getIdleTimeMs();
}

void IdleMonitor::poll()
{
    qint64 idle = getIdleTimeMs();
    if (idle >= m_thresholdMs && m_thresholdMs > 0) {
        m_timer.stop();
        emit idleDetected();
    }
}

qint64 IdleMonitor::getIdleTimeMs() const
{
    if (m_waylandMode) {
        qint64 t = getIdleTimeWayland();
        if (t >= 0) return t;
    }
#ifdef HAVE_X11SS
    return getIdleTimeX11();
#endif
    // Cannot determine idle time; return 0 so idle is never falsely triggered.
    return 0;
}

qint64 IdleMonitor::getIdleTimeWayland() const
{
    if (!m_dbusIface || !m_dbusIface->isValid())
        return -1;

    QDBusReply<qulonglong> reply = m_dbusIface->call("GetIdletime");
    if (!reply.isValid()) {
        qWarning() << "[IdleMonitor] GetIdletime failed:" << reply.error().message();
        return -1;
    }
    return static_cast<qint64>(reply.value()); // milliseconds
}

#ifdef HAVE_X11SS
qint64 IdleMonitor::getIdleTimeX11() const
{
    Display* dpy = QX11Info::display();
    if (!dpy) return 0;
    XScreenSaverInfo* info = XScreenSaverAllocInfo();
    if (!info) return 0;
    XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info);
    qint64 idle = static_cast<qint64>(info->idle);
    XFree(info);
    return idle;
}
#endif
