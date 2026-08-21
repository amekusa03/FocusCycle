#include "TrayIcon.h"
#include "timer/CycleController.h"
#include "core/Settings.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QPalette>
#include <QApplication>

TrayIcon::TrayIcon(CycleController* ctrl, QObject* parent)
    : QObject(parent), m_ctrl(ctrl)
{
    m_tray = new QSystemTrayIcon(this);
    m_tray->setToolTip("FocusCycle");
    buildMenu();
    retranslateUi();
    m_tray->show();
}

TrayIcon::~TrayIcon()
{
    if (m_tray) {
        m_tray->hide();
    }
}

void TrayIcon::buildMenu()
{
    m_menu = new QMenu;
    m_menu->setStyleSheet(R"QSS(
QMenu {
    background-color: #1e1f38;
    color: #eaeaea;
    border: 1px solid #2a2b4e;
    border-radius: 10px;
    padding: 6px 0;
    font-family: sans-serif;
    font-size: 13px;
}
QMenu::item {
    padding: 8px 22px;
    border-radius: 6px;
    margin: 1px 6px;
}
QMenu::item:selected {
    background-color: rgba(74,144,226,0.2);
    color: white;
}
QMenu::separator {
    height: 1px;
    background: rgba(255,255,255,0.07);
    margin: 4px 12px;
}
)QSS");

    m_settingsAction = m_menu->addAction("");
    connect(m_settingsAction, &QAction::triggered,
            this, &TrayIcon::openSettingsRequested);

    m_menu->addSeparator();

    m_completeWorkAction = m_menu->addAction("");
    connect(m_completeWorkAction, &QAction::triggered,
            m_ctrl, &CycleController::completeWork);

    m_completeBreakAction = m_menu->addAction("");
    connect(m_completeBreakAction, &QAction::triggered,
            m_ctrl, &CycleController::completeBreak);

    m_menu->addSeparator();

    m_logAction = m_menu->addAction("");
    connect(m_logAction, &QAction::triggered,
            this, &TrayIcon::openLogRequested);

    m_menu->addSeparator();

    m_quitAction = m_menu->addAction("");
    connect(m_quitAction, &QAction::triggered, this, [this]() {
        if (m_ctrl) {
            m_ctrl->stopAll();
        }
        if (m_tray) {
            m_tray->hide();
        }
        qApp->quit();
    });

    m_tray->setContextMenu(m_menu);
}

void TrayIcon::retranslateUi()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (isJa) {
        m_settingsAction->setText("⚙  設定を開く");
        m_completeWorkAction->setText("✅  作業完了");
        m_completeBreakAction->setText("⏭  休憩完了");
        m_logAction->setText("📋  ログを開く");
        m_quitAction->setText("✕  終了");
    } else {
        m_settingsAction->setText("⚙  Open Settings");
        m_completeWorkAction->setText("✅  Complete Work");
        m_completeBreakAction->setText("⏭  Complete Break");
        m_logAction->setText("📋  Open Log");
        m_quitAction->setText("✕  Quit");
    }
    updateStatus(m_currentState, m_currentSeconds);
}

QPixmap TrayIcon::makeIcon(const QColor& color) const
{
    const int sz = 22;
    QPixmap pm(sz, sz);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(color, 1.8));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QRectF(1, 1, sz - 2, sz - 2));

    p.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(sz / 2.0, sz / 2.0), QPointF(sz / 2.0, 3.5));

    p.setPen(QPen(color.lighter(140), 1.6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(sz / 2.0, sz / 2.0), QPointF(sz - 4.0, sz / 2.0));

    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawEllipse(QPointF(sz / 2.0, sz / 2.0), 1.8, 1.8);

    return pm;
}

QString TrayIcon::formatTime(int seconds) const
{
    int absSecs = qAbs(seconds);
    int mins = absSecs / 60;
    int secs = absSecs % 60;
    if (seconds < 0) {
        return QString("-%1:%2")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    } else {
        return QString("%1:%2")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    }
}

void TrayIcon::updateStatus(AppState state, int seconds)
{
    m_currentState = state;
    m_currentSeconds = seconds;
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);

    QColor systemColor = qApp->palette().color(QPalette::WindowText);
    if (systemColor.lightness() < 100) {
        systemColor = QColor(240, 240, 240);
    }

    QColor iconColor = systemColor;
    QString tooltipStr;

    switch (state) {
    case AppState::WorkWait:
        iconColor = systemColor;
        tooltipStr = isJa ? "作業待ち" : "Waiting for Work";
        setWorkActionEnabled(false);
        setBreakActionEnabled(false);
        break;

    case AppState::Working:
        iconColor = systemColor;
        tooltipStr = isJa ? QString("作業中 %1").arg(formatTime(seconds))
                           : QString("Working %1").arg(formatTime(seconds));
        setWorkActionEnabled(true);
        setBreakActionEnabled(false);
        break;

    case AppState::BreakWait:
        iconColor = systemColor;
        tooltipStr = isJa ? "休憩待ち" : "Waiting for Break";
        setWorkActionEnabled(true);
        setBreakActionEnabled(false);
        break;

    case AppState::Break:
        iconColor = QColor(255, 179, 0); // Yellow for break notification
        tooltipStr = isJa ? QString("休憩中 %1").arg(formatTime(seconds))
                           : QString("On Break %1").arg(formatTime(seconds));
        setWorkActionEnabled(false);
        setBreakActionEnabled(true);
        break;

    case AppState::CompanyBreak:
        iconColor = systemColor;
        tooltipStr = isJa ? "会社の休憩時間" : "Company Break";
        setWorkActionEnabled(false);
        setBreakActionEnabled(false);
        break;
    }

    m_tray->setIcon(QIcon(makeIcon(iconColor)));
    m_tray->setToolTip("FocusCycle | " + tooltipStr);
}

void TrayIcon::setWorkActionEnabled(bool enabled)
{
    if (m_completeWorkAction)
        m_completeWorkAction->setEnabled(enabled);
}

void TrayIcon::setBreakActionEnabled(bool enabled)
{
    if (m_completeBreakAction)
        m_completeBreakAction->setEnabled(enabled);
}
