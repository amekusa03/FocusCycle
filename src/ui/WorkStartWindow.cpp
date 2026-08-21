#include "WorkStartWindow.h"
#include "CountdownRing.h"
#include "core/Settings.h"
#include "core/SessionLogger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

namespace {
const char* kStyleSheet = R"QSS(
WorkStartWindow { background: transparent; }
QWidget {
    font-family: sans-serif;
    background: transparent;
}
#mainCard {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #1a1b32, stop:1 #121324);
    border-radius: 16px;
    border: 1px solid rgba(255,255,255,0.08);
}
#logoLabel {
    font-size: 20px;
    font-weight: bold;
    color: #ffffff;
}
#logoSub {
    font-size: 9px;
    letter-spacing: 2px;
    color: #6060a0;
}
#divider {
    background: rgba(255,255,255,0.06);
    height: 1px;
}
#taskInput {
    background-color: rgba(255,255,255,0.05);
    border: 1px solid rgba(255,255,255,0.1);
    border-radius: 10px;
    padding: 7px 12px;
    color: #eaeaea;
    selection-background-color: #4a90e2;
    font-size: 13px;
}
#taskInput:focus {
    border: 1.5px solid #4a90e2;
    background-color: rgba(74,144,226,0.07);
}
#startButton {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #4a90e2, stop:1 #2c68b8);
    color: white;
    border: none;
    border-radius: 10px;
    padding: 11px;
    font-size: 13px;
    font-weight: bold;
}
#startButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #5fa3f5, stop:1 #3d7fd1);
}
#startButton:pressed {
    background: #205090;
}
#workTaskLabel {
    font-size: 13px;
    color: #ccccdd;
    background: rgba(255,255,255,0.04);
    border-radius: 8px;
    padding: 6px 12px;
}
#workStatusLabel {
    font-size: 12px;
    color: #5555aa;
}
)QSS";
}

WorkStartWindow::WorkStartWindow(QWidget* parent)
    : QWidget(parent,
              Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("FocusCycle");
    setupUi();
    applyStyle();
    retranslateUi();
    centerOnScreen();
}

void WorkStartWindow::setupUi()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(12, 12, 12, 12);

    auto* card = new QWidget;
    card->setObjectName("mainCard");
    card->setFixedWidth(270);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(25);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 150));
    card->setGraphicsEffect(shadow);

    m_stack = new QStackedWidget(card);
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->addWidget(m_stack);

    // Page 1: Start
    m_startPage = new QWidget;
    auto* sl = new QVBoxLayout(m_startPage);
    sl->setContentsMargins(20, 20, 20, 20);
    sl->setSpacing(0);

    m_logoLabel = new QLabel("FocusCycle");
    m_logoLabel->setObjectName("logoLabel");
    m_logoLabel->setAlignment(Qt::AlignCenter);

    m_logoSubLabel = new QLabel("FOCUS · REST · REPEAT");
    m_logoSubLabel->setObjectName("logoSub");
    m_logoSubLabel->setAlignment(Qt::AlignCenter);

    auto* div = new QWidget;
    div->setObjectName("divider");

    m_taskInput = new QComboBox;
    m_taskInput->setObjectName("taskInput");
    m_taskInput->setEditable(true);
    m_taskInput->setMaxCount(30);

    m_startButton = new QPushButton;
    m_startButton->setObjectName("startButton");
    m_startButton->setCursor(Qt::PointingHandCursor);

    sl->addWidget(m_logoLabel);
    sl->addSpacing(2);
    sl->addWidget(m_logoSubLabel);
    sl->addSpacing(14);
    sl->addWidget(div);
    sl->addSpacing(14);
    sl->addWidget(m_taskInput);
    sl->addSpacing(10);
    sl->addWidget(m_startButton);

    // Page 2: Working
    m_workPage = new QWidget;
    auto* wl = new QVBoxLayout(m_workPage);
    wl->setContentsMargins(20, 18, 20, 18);
    wl->setSpacing(10);

    m_taskLabel = new QLabel;
    m_taskLabel->setObjectName("workTaskLabel");
    m_taskLabel->setAlignment(Qt::AlignCenter);
    m_taskLabel->setWordWrap(true);

    m_ring = new CountdownRing;
    m_ring->setFixedSize(150, 150);

    m_statusLabel = new QLabel;
    m_statusLabel->setObjectName("workStatusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);

    wl->addWidget(m_taskLabel);
    wl->addSpacing(4);
    wl->addWidget(m_ring, 0, Qt::AlignCenter);
    wl->addWidget(m_statusLabel);

    m_stack->addWidget(m_startPage);
    m_stack->addWidget(m_workPage);

    outerLayout->addWidget(card);

    installDragFilter(card);
    installDragFilter(m_startPage);
    installDragFilter(m_workPage);
    installDragFilter(m_logoLabel);
    installDragFilter(m_logoSubLabel);
    installDragFilter(m_taskLabel);
    installDragFilter(m_ring);
    installDragFilter(m_statusLabel);

    connect(m_startButton, &QPushButton::clicked, this, [this]() {
        emit workStartRequested(m_taskInput->currentText().trimmed());
    });
    if (m_taskInput->lineEdit()) {
        connect(m_taskInput->lineEdit(), &QLineEdit::returnPressed,
                m_startButton, &QPushButton::click);
    }
}

void WorkStartWindow::installDragFilter(QWidget* widget)
{
    if (widget) {
        widget->installEventFilter(this);
    }
}

void WorkStartWindow::applyStyle()
{
    setStyleSheet(kStyleSheet);
}

void WorkStartWindow::retranslateUi()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);

    if (m_taskInput->lineEdit()) {
        if (isJa) {
            m_taskInput->lineEdit()->setPlaceholderText("タスクを入力（選択または自由入力）");
        } else {
            m_taskInput->lineEdit()->setPlaceholderText("Enter or select task");
        }
    }
    if (isJa) {
        m_startButton->setText("▶  作業を開始する");
        m_statusLabel->setText("集中中…");
    } else {
        m_startButton->setText("▶  Start Focus");
        m_statusLabel->setText("Focusing...");
    }

    setTaskName(m_rawTaskName);
    updateWorkTick(m_lastRemainingSecs, m_totalWorkSecs);
}

void WorkStartWindow::centerOnScreen()
{
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return;
    QRect sg = screen->availableGeometry();
    adjustSize();
    move(sg.center() - rect().center());
}

void WorkStartWindow::showStartPage()
{
    m_taskInput->clear();
    QStringList history = SessionLogger::instance()->recentTaskNames();
    m_taskInput->addItems(history);

    if (!history.isEmpty()) {
        m_taskInput->setCurrentIndex(0);
    } else {
        m_taskInput->setEditText(QString());
    }

    m_stack->setCurrentIndex(0);
    adjustSize();
    show();
    raise();
    m_taskInput->setFocus();
}

void WorkStartWindow::showWorkingPage()
{
    m_totalWorkSecs = Settings::instance()->workMinutes() * 60;
    m_ring->setProgress(1.0);
    m_ring->setRingColor(QColor(0x26, 0xd0, 0x7a));
    int mins = m_totalWorkSecs / 60;
    m_ring->setTimeText(QString("%1:00").arg(mins, 2, 10, QChar('0')));
    m_ring->setSubText(QString());
    m_stack->setCurrentIndex(1);
    adjustSize();
    show();
    raise();
}

void WorkStartWindow::updateWorkTick(int remainingSecs, int totalSecs)
{
    m_lastRemainingSecs = remainingSecs;
    if (totalSecs > 0) m_totalWorkSecs = totalSecs;

    qreal progress = 0.0;
    if (remainingSecs >= 0) {
        progress = (m_totalWorkSecs > 0)
            ? static_cast<qreal>(remainingSecs) / m_totalWorkSecs
            : 0.0;
    } else {
        progress = 0.0;
    }

    QColor color;
    if (remainingSecs < 0) {
        color = QColor(255, 255, 255); // White for overtime (no red)
    } else if (progress > 0.5) {
        color = QColor(0x26, 0xd0, 0x7a);
    } else if (progress > 0.2) {
        color = QColor(0xff, 0xb3, 0x00);
    } else {
        color = QColor(255, 255, 255); // White for low remaining time (no red)
    }

    m_ring->setProgress(progress);
    m_ring->setRingColor(color);

    int absSecs = qAbs(remainingSecs);
    int mins = absSecs / 60;
    int secs = absSecs % 60;

    QString timeStr;
    if (remainingSecs < 0) {
        timeStr = QString("-%1:%2")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    } else {
        timeStr = QString("%1:%2")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    }
    m_ring->setTimeText(timeStr);

    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (remainingSecs < 0) {
        m_ring->setSubText(isJa ? "超過時間" : "Overtime");
    } else if (remainingSecs <= 300 && remainingSecs > 0) {
        m_ring->setSubText(isJa ? "もうすぐ終了" : "Almost done");
    } else {
        m_ring->setSubText(QString());
    }
}

void WorkStartWindow::setTaskName(const QString& task)
{
    m_rawTaskName = task;
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (task.isEmpty()) {
        m_taskLabel->setText(isJa ? "フリーセッション" : "Free Session");
    } else {
        m_taskLabel->setText("📌 " + task);
    }
}

bool WorkStartWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            m_dragPos = me->globalPos() - frameGeometry().topLeft();
            return false;
        }
    } else if (event->type() == QEvent::MouseMove) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->buttons() & Qt::LeftButton) {
            move(me->globalPos() - m_dragPos);
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void WorkStartWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_dragPos = event->globalPos() - frameGeometry().topLeft();
    QWidget::mousePressEvent(event);
}

void WorkStartWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
        move(event->globalPos() - m_dragPos);
    QWidget::mouseMoveEvent(event);
}
