#pragma once
#include <QObject>
#include <QColor>
#include <QString>
#include "core/AppState.h"

class QSystemTrayIcon;
class QMenu;
class QAction;
class CycleController;

class TrayIcon : public QObject
{
    Q_OBJECT
public:
    explicit TrayIcon(CycleController* ctrl, QObject* parent = nullptr);
    ~TrayIcon();

    void updateStatus(AppState state, int seconds = 0);
    void setWorkActionEnabled(bool enabled);
    void setBreakActionEnabled(bool enabled);
    void retranslateUi();

signals:
    void openSettingsRequested();
    void openLogRequested();

private:
    void buildMenu();
    QPixmap makeIcon(const QColor& color) const;
    QString formatTime(int seconds) const;

    CycleController*  m_ctrl                = nullptr;
    QSystemTrayIcon*  m_tray                = nullptr;
    QMenu*            m_menu                = nullptr;
    QAction*          m_settingsAction      = nullptr;
    QAction*          m_completeWorkAction  = nullptr;
    QAction*          m_completeBreakAction = nullptr;
    QAction*          m_logAction           = nullptr;
    QAction*          m_quitAction          = nullptr;

    AppState          m_currentState        = AppState::WorkWait;
    int               m_currentSeconds      = 0;
};
