#pragma once
#include <QObject>
#include <QTimer>
#include <QDBusInterface>

/**
 * PC Idle Time Monitor
 *
 * Wayland (Primary): org.gnome.Mutter.IdleMonitor D-Bus API
 * X11 (Fallback): XScreenSaver Extension
 */
class IdleMonitor : public QObject
{
    Q_OBJECT
public:
    explicit IdleMonitor(QObject* parent = nullptr);

    void start(int thresholdSeconds);
    void stop();
    qint64 currentIdleMs() const;

signals:
    void idleDetected();

private slots:
    void poll();

private:
    QTimer  m_timer;
    qint64  m_thresholdMs  = 0;
    bool             m_waylandMode  = false;
    QDBusInterface*  m_dbusIface    = nullptr; // cached Mutter D-Bus handle

    qint64 getIdleTimeMs()      const;
    qint64 getIdleTimeWayland() const;
#ifdef HAVE_X11SS
    qint64 getIdleTimeX11()     const;
#endif
};
