#pragma once
#include <QObject>
#include <QString>

class CycleController;
class WorkStartWindow;
class BreakDialog;
class CompanyBreakDialog;
class LogWindow;
class SettingsDialog;
class TrayIcon;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr);
    ~Application();

    void initialize();

public slots:
    void updateAllUiTranslations();

private:
    void connectSignals();
    void notify(const QString& title, const QString& body);

    CycleController*     m_ctrl                = nullptr;
    WorkStartWindow*     m_workStartWindow    = nullptr;
    BreakDialog*         m_breakDialog        = nullptr;
    CompanyBreakDialog*  m_companyBreakDialog = nullptr;
    LogWindow*           m_logWindow          = nullptr;
    SettingsDialog*      m_settingsDialog     = nullptr;
    TrayIcon*            m_tray               = nullptr;
};
