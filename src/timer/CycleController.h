#pragma once
#include <QObject>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QDateTime>
#include "core/AppState.h"

class IdleMonitor;

/**
 * FocusCycle Main State Machine
 */
class CycleController : public QObject
{
    Q_OBJECT
public:
    explicit CycleController(QObject* parent = nullptr);
    ~CycleController();

    AppState state()          const { return m_state;         }
    int   workRemainingSecs() const { return m_workRemaining; }
    int   breakRemainingSecs()const { return m_breakRemaining;}
    QString currentTask()     const { return m_currentTask;   }

    void stopAll();

public slots:
    void startWork(const QString& taskName);
    void continueWork();
    void completeWork();
    void completeBreak();
    void companyBreakRest();
    void companyBreakContinue();

signals:
    void stateChanged(AppState state);
    void workTick(int remainingSeconds);
    void breakTick(int remainingSeconds);
    void workTimeFinished();
    void breakTimeFinished();
    void showBreakDialog();
    void showCompanyBreakDialog();
    void showWorkStartWindow();

private slots:
    void onWorkTick();
    void onBreakTick();
    void checkCompanyBreaks();

private:
    void setState(AppState s);
    void startBreakCountdown();
    void logSession(bool completed);

    AppState  m_state                     = AppState::WorkWait;
    AppState  m_stateBeforeCompanyBreak   = AppState::WorkWait;
    QString   m_currentTask;
    QDateTime m_workStartTime;
    int       m_workRemaining             = 0;
    int       m_breakRemaining            = 0;

    QTimer       m_workTimer;
    QTimer       m_breakTimer;
    QTimer       m_companyCheckTimer;
    IdleMonitor* m_idleMonitor            = nullptr;

    int m_lastCheckedMinute               = -1;
    int m_lastCheckedHour                 = -1;
};
