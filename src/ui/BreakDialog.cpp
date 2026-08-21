#include "BreakDialog.h"
#include "core/Settings.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

BreakDialog::BreakDialog(QWidget* parent)
    : QDialog(parent,
              Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("FocusCycle - Break");
    setModal(false);
    setupUi();
    retranslateUi();

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect sg = screen->availableGeometry();
        adjustSize();
        move(sg.center() - rect().center());
    }
}

void BreakDialog::setupUi()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(12, 12, 12, 12);

    auto* card = new QWidget;
    card->setObjectName("breakCard");
    card->setFixedWidth(280);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(25);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 150));
    card->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 22, 22, 20);
    layout->setSpacing(10);

    auto* icon = new QLabel("🎉");
    icon->setAlignment(Qt::AlignCenter);
    icon->setObjectName("breakIcon");

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("breakTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_taskLabel = new QLabel;
    m_taskLabel->setObjectName("breakTaskLabel");
    m_taskLabel->setAlignment(Qt::AlignCenter);
    m_taskLabel->setWordWrap(true);

    auto* div = new QWidget;
    div->setObjectName("breakDivider");
    div->setFixedHeight(1);

    m_questionLabel = new QLabel;
    m_questionLabel->setObjectName("breakQuestion");
    m_questionLabel->setAlignment(Qt::AlignCenter);

    m_completeBtn = new QPushButton;
    m_completeBtn->setObjectName("breakCompleteBtn");
    m_completeBtn->setCursor(Qt::PointingHandCursor);

    m_continueBtn = new QPushButton;
    m_continueBtn->setObjectName("breakContinueBtn");
    m_continueBtn->setCursor(Qt::PointingHandCursor);

    layout->addWidget(icon);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_taskLabel);
    layout->addWidget(div);
    layout->addSpacing(2);
    layout->addWidget(m_questionLabel);
    layout->addSpacing(2);
    layout->addWidget(m_completeBtn);
    layout->addWidget(m_continueBtn);

    outerLayout->addWidget(card);

    card->installEventFilter(this);
    icon->installEventFilter(this);
    m_titleLabel->installEventFilter(this);
    m_taskLabel->installEventFilter(this);
    m_questionLabel->installEventFilter(this);

    setStyleSheet(R"QSS(
BreakDialog { background: transparent; }
QWidget {
    font-family: sans-serif;
    background: transparent;
}
#breakCard {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #1e1f35, stop:1 #161724);
    border-radius: 16px;
    border: 1px solid rgba(255,255,255,0.08);
}
#breakIcon { font-size: 34px; background: transparent; }
#breakTitle {
    font-size: 17px;
    font-weight: bold;
    color: #eaeaea;
}
#breakTaskLabel {
    font-size: 13px;
    color: #ccccdd;
    background: rgba(255,255,255,0.04);
    border-radius: 8px;
    padding: 6px 12px;
}
#breakDivider { background: rgba(255,255,255,0.06); }
#breakQuestion { font-size: 12px; color: #6666aa; }
#breakCompleteBtn {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #4a90e2, stop:1 #2c68b8);
    color: white; border: none; border-radius: 10px;
    padding: 11px; font-size: 13px; font-weight: bold;
}
#breakCompleteBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #5fa3f5, stop:1 #3d7fd1);
}
#breakContinueBtn {
    background: rgba(255,255,255,0.03);
    color: #7070a0;
    border: 1px solid rgba(255,255,255,0.08);
    border-radius: 10px; padding: 9px;
    font-size: 12px;
}
#breakContinueBtn:hover { color: #ccccdd; border-color: rgba(255,255,255,0.2); }
)QSS");

    connect(m_completeBtn, &QPushButton::clicked, this, [this]() {
        emit completeRequested();
        accept();
    });
    connect(m_continueBtn, &QPushButton::clicked, this, [this]() {
        emit continueRequested();
        accept();
    });
}

void BreakDialog::retranslateUi()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (isJa) {
        m_titleLabel->setText("お疲れ様でした！");
        m_questionLabel->setText("次の操作を選択してください");
        m_completeBtn->setText("☕  作業完了（休憩に入る）");
        m_continueBtn->setText("⚡  継続（タイマーリセット）");
    } else {
        m_titleLabel->setText("Great job!");
        m_questionLabel->setText("Select next action");
        m_completeBtn->setText("☕  Complete Work (Take Break)");
        m_continueBtn->setText("⚡  Continue (Reset Timer)");
    }
    setTaskName(m_rawTaskName);
}

void BreakDialog::setTaskName(const QString& task)
{
    m_rawTaskName = task;
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (task.isEmpty()) {
        m_taskLabel->setText(isJa ? "フリーセッション" : "Free Session");
    } else {
        m_taskLabel->setText("📌 " + task);
    }
}

bool BreakDialog::eventFilter(QObject* watched, QEvent* event)
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
    return QDialog::eventFilter(watched, event);
}

void BreakDialog::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_dragPos = e->globalPos() - frameGeometry().topLeft();
    QDialog::mousePressEvent(e);
}

void BreakDialog::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton)
        move(e->globalPos() - m_dragPos);
    QDialog::mouseMoveEvent(e);
}
