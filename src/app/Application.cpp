#include "Application.h"
#include "timer/CycleController.h"
#include "core/Settings.h"
#include "core/AppState.h"
#include "ui/WorkStartWindow.h"
#include "ui/BreakDialog.h"
#include "ui/CompanyBreakDialog.h"
#include "ui/LogWindow.h"
#include "ui/SettingsDialog.h"
#include "ui/TrayIcon.h"
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

Application::Application(QObject* parent) : QObject(parent) {}

Application::~Application()
{
    if (m_ctrl) {
        m_ctrl->stopAll();
    }
    delete m_workStartWindow;
    delete m_breakDialog;
    delete m_companyBreakDialog;
    delete m_logWindow;
    delete m_settingsDialog;
    delete m_tray;
}

void Application::initialize()
{
    m_ctrl = new CycleController(this);

    m_workStartWindow    = new WorkStartWindow;
    m_breakDialog        = new BreakDialog;
    m_companyBreakDialog = new CompanyBreakDialog;
    m_logWindow          = new LogWindow;
    m_settingsDialog     = new SettingsDialog;

    m_tray = new TrayIcon(m_ctrl, this);

    connectSignals();

    updateAllUiTranslations();

    m_workStartWindow->showStartPage();

    qDebug() << "[Application] Initialized. FocusCycle running.";
}

void Application::updateAllUiTranslations()
{
    m_workStartWindow->retranslateUi();
    m_breakDialog->retranslateUi();
    m_companyBreakDialog->retranslateUi();
    m_logWindow->retranslateUi();
    m_settingsDialog->retranslateUi();
    m_tray->retranslateUi();
    if (m_ctrl) {
        m_tray->updateStatus(m_ctrl->state(),
            m_ctrl->state() == AppState::Working ? m_ctrl->workRemainingSecs() :
            (m_ctrl->state() == AppState::Break ? m_ctrl->breakRemainingSecs() : 0));
    }
}

void Application::connectSignals()
{
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, [this]() {
                qDebug() << "[Application] aboutToQuit received. Stopping timers and closing windows.";
                if (m_ctrl) {
                    m_ctrl->stopAll();
                }
                if (m_workStartWindow) m_workStartWindow->hide();
                if (m_breakDialog) m_breakDialog->hide();
                if (m_companyBreakDialog) m_companyBreakDialog->hide();
                if (m_logWindow) m_logWindow->hide();
                if (m_settingsDialog) m_settingsDialog->hide();
            });

    connect(Settings::instance(), &Settings::settingsChanged,
            this, &Application::updateAllUiTranslations);

    connect(m_ctrl, &CycleController::showWorkStartWindow,
            this, [this]() {
                m_workStartWindow->showStartPage();
            });

    connect(m_ctrl, &CycleController::showBreakDialog,
            this, [this]() {
                m_workStartWindow->hide();
                m_breakDialog->setTaskName(m_ctrl->currentTask());
                m_breakDialog->show();
                m_breakDialog->raise();
            });

    connect(m_ctrl, &CycleController::showCompanyBreakDialog,
            this, [this]() {
                m_workStartWindow->hide();
                bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
                m_companyBreakDialog->show();
                m_companyBreakDialog->raise();
                notify(isJa ? "会社の休憩時間" : "Scheduled Break Time",
                       isJa ? "会社の休憩時間になりました 🏢" : "It's scheduled break time 🏢");
            });

    connect(m_ctrl, &CycleController::workTimeFinished,
            this, [this]() {
                bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
                notify(isJa ? "作業時間終了" : "Work Time Finished",
                       isJa ? "お疲れ様でした！未操作時間を検出後、休憩ダイアログを表示します。"
                            : "Great job! Idle detection will trigger the break dialog.");
            });

    connect(m_ctrl, &CycleController::breakTimeFinished,
            this, [this]() {
                bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
                notify(isJa ? "休憩時間終了" : "Break Time Finished",
                       isJa ? "休憩時間が終了しました。次の作業を開始しましょう！ 🚀"
                            : "Break finished. Ready for the next session! 🚀");
            });

    connect(m_ctrl, &CycleController::workTick,
            this, [this](int remaining) {
                m_tray->updateStatus(m_ctrl->state(), remaining);
            });

    connect(m_ctrl, &CycleController::breakTick,
            this, [this](int remaining) {
                m_tray->updateStatus(m_ctrl->state(), remaining);
            });

    connect(m_ctrl, &CycleController::stateChanged,
            this, [this](AppState state) {
                int secs = (state == AppState::Working) ? m_ctrl->workRemainingSecs() :
                           ((state == AppState::Break) ? m_ctrl->breakRemainingSecs() : 0);
                m_tray->updateStatus(state, secs);

                switch (state) {
                case AppState::WorkWait:
                    m_breakDialog->hide();
                    m_companyBreakDialog->hide();
                    break;

                case AppState::Working:
                    m_workStartWindow->hide();
                    m_breakDialog->hide();
                    m_companyBreakDialog->hide();
                    break;

                case AppState::BreakWait:
                    m_workStartWindow->hide();
                    m_companyBreakDialog->hide();
                    break;

                case AppState::Break:
                    m_workStartWindow->hide();
                    m_breakDialog->hide();
                    m_companyBreakDialog->hide();
                    break;

                case AppState::CompanyBreak:
                    m_workStartWindow->hide();
                    m_breakDialog->hide();
                    break;
                }
            });

    connect(m_workStartWindow, &WorkStartWindow::workStartRequested,
            this, [this](const QString& task) {
                m_workStartWindow->hide();
                m_ctrl->startWork(task);
            });

    connect(m_breakDialog, &BreakDialog::continueRequested,
            this, [this]() {
                m_breakDialog->hide();
                m_ctrl->continueWork();
            });
    connect(m_breakDialog, &BreakDialog::completeRequested,
            this, [this]() {
                m_breakDialog->hide();
                m_ctrl->completeWork();
            });

    connect(m_companyBreakDialog, &CompanyBreakDialog::restRequested,
            this, [this]() {
                m_companyBreakDialog->hide();
                m_ctrl->companyBreakRest();
            });
    connect(m_companyBreakDialog, &CompanyBreakDialog::continueWorkRequested,
            this, [this]() {
                m_companyBreakDialog->hide();
                m_ctrl->companyBreakContinue();
            });

    connect(m_tray, &TrayIcon::openSettingsRequested,
            this, [this]() {
                m_settingsDialog->show();
                m_settingsDialog->raise();
            });

    connect(m_tray, &TrayIcon::openLogRequested,
            this, [this]() {
                m_logWindow->refresh();
                m_logWindow->show();
                m_logWindow->raise();
            });
}

void Application::notify(const QString& title, const QString& body)
{
    QProcess::startDetached("notify-send", {
        "--app-name=FocusCycle",
        "--expire-time=5000",
        title,
        body
    });
}
