#include "CycleController.h"
#include "core/IdleMonitor.h"
#include "core/Settings.h"
#include "core/SessionLogger.h"
#include <QTime>
#include <QDebug>

CycleController::CycleController(QObject* parent) : QObject(parent)
{
    m_idleMonitor = new IdleMonitor(this);

    m_workTimer.setInterval(1000);
    connect(&m_workTimer, &QTimer::timeout, this, &CycleController::onWorkTick);

    m_breakTimer.setInterval(1000);
    connect(&m_breakTimer, &QTimer::timeout, this, &CycleController::onBreakTick);

    m_companyCheckTimer.setInterval(1000);
    connect(&m_companyCheckTimer, &QTimer::timeout,
            this, &CycleController::checkCompanyBreaks);
    m_companyCheckTimer.start();
}

CycleController::~CycleController()
{
    stopAll();
}

void CycleController::stopAll()
{
    m_workTimer.stop();
    m_breakTimer.stop();
    m_companyCheckTimer.stop();
    if (m_idleMonitor) {
        m_idleMonitor->stop();
    }
}

void CycleController::startWork(const QString& taskName)
{
    m_currentTask   = taskName;
    m_workStartTime = QDateTime::currentDateTime();
    SessionLogger::instance()->logTaskStart(taskName);

    m_workRemaining = Settings::instance()->workMinutes() * 60;

    m_idleMonitor->stop();
    m_breakTimer.stop();

    setState(AppState::Working);
    emit workTick(m_workRemaining);
    m_workTimer.start();
}

void CycleController::continueWork()
{
    m_workRemaining = Settings::instance()->workMinutes() * 60;
    setState(AppState::Working);
    emit workTick(m_workRemaining);
    m_workTimer.start();
}

void CycleController::completeWork()
{
    m_workTimer.stop();
    m_idleMonitor->stop();
    logSession(true);
    startBreakCountdown();
}

void CycleController::completeBreak()
{
    m_breakTimer.stop();
    setState(AppState::WorkWait);
    emit showWorkStartWindow();
}

void CycleController::companyBreakRest()
{
    m_workTimer.stop();
    m_idleMonitor->stop();

    if (m_stateBeforeCompanyBreak == AppState::Working ||
        m_stateBeforeCompanyBreak == AppState::BreakWait) {
        logSession(false);
    }
    startBreakCountdown();
}

void CycleController::companyBreakContinue()
{
    setState(m_stateBeforeCompanyBreak);

    switch (m_stateBeforeCompanyBreak) {
    case AppState::Working:
        m_workTimer.start();
        break;
    case AppState::BreakWait:
        emit showBreakDialog();
        break;
    case AppState::Break:
        m_breakTimer.start();
        break;
    case AppState::WorkWait:
        emit showWorkStartWindow();
        break;
    default:
        break;
    }
}

void CycleController::onWorkTick()
{
    --m_workRemaining;
    emit workTick(m_workRemaining);

    if (m_workRemaining == 0) {
        emit workTimeFinished();
    }

    if (m_workRemaining <= 0) {
        int threshold = Settings::instance()->idleThresholdSeconds();
        if (threshold == 0) {
            m_workTimer.stop();
            setState(AppState::BreakWait);
            emit showBreakDialog();
        } else {
            qint64 idleMs = m_idleMonitor->currentIdleMs();
            if (idleMs >= threshold * 1000LL) {
                m_workTimer.stop();
                setState(AppState::BreakWait);
                emit showBreakDialog();
            }
        }
    }
}

void CycleController::onBreakTick()
{
    --m_breakRemaining;
    emit breakTick(m_breakRemaining);

    if (m_breakRemaining <= 0) {
        m_breakTimer.stop();
        emit breakTimeFinished();
        setState(AppState::WorkWait);
        emit showWorkStartWindow();
    }
}

void CycleController::checkCompanyBreaks()
{
    QVector<CompanyBreak> breaks = Settings::instance()->companyBreaks();
    if (breaks.isEmpty()) return;

    QTime now = QTime::currentTime();
    int h = now.hour();
    int m = now.minute();

    if (m_state == AppState::CompanyBreak) {
        if (h == m_lastCheckedHour && m == m_lastCheckedMinute) return;
        m_lastCheckedHour   = h;
        m_lastCheckedMinute = m;

        for (const CompanyBreak& cb : breaks) {
            if (cb.hasEndTime() && cb.endTime.hour() == h && cb.endTime.minute() == m) {
                qDebug() << "[CycleController] Company break ended at" << cb.endTime.toString("HH:mm");
                companyBreakContinue();
                return;
            }
        }
        return;
    }

    if (m_state == AppState::WorkWait) return;

    if (h == m_lastCheckedHour && m == m_lastCheckedMinute) return;
    m_lastCheckedHour   = h;
    m_lastCheckedMinute = m;

    for (const CompanyBreak& cb : breaks) {
        if (cb.startTime.hour() == h && cb.startTime.minute() == m) {
            qDebug() << "[CycleController] Company break triggered at" << cb.startTime.toString("HH:mm");
            m_stateBeforeCompanyBreak = m_state;

            if (m_workTimer.isActive()) m_workTimer.stop();

            setState(AppState::CompanyBreak);
            emit showCompanyBreakDialog();
            return;
        }
    }
}

void CycleController::setState(AppState s)
{
    if (m_state == s) return;
    m_state = s;
    qDebug() << "[CycleController] State ->" << static_cast<int>(s);
    emit stateChanged(s);
}

void CycleController::startBreakCountdown()
{
    m_breakRemaining = Settings::instance()->breakMinutes() * 60;
    setState(AppState::Break);
    emit breakTick(m_breakRemaining);
    m_breakTimer.start();
}

void CycleController::logSession(bool completed)
{
    if (!m_workStartTime.isValid()) return;
    SessionRecord r;
    r.startTime = m_workStartTime;
    r.endTime   = QDateTime::currentDateTime();
    r.taskName  = m_currentTask;
    r.completed = completed;
    SessionLogger::instance()->logSession(r);
    m_workStartTime = QDateTime();
}
