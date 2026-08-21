#include "CompanyBreakDialog.h"
#include "core/Settings.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

CompanyBreakDialog::CompanyBreakDialog(QWidget* parent)
    : QDialog(parent,
              Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("FocusCycle - Scheduled Break");
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

void CompanyBreakDialog::setupUi()
{
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(12, 12, 12, 12);

    auto* card = new QWidget;
    card->setObjectName("cbCard");
    card->setFixedWidth(280);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(25);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 150));
    card->setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(22, 22, 22, 20);
    layout->setSpacing(10);

    auto* icon = new QLabel("🏢");
    icon->setAlignment(Qt::AlignCenter);
    icon->setObjectName("cbIcon");

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("cbTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_subtitleLabel = new QLabel;
    m_subtitleLabel->setObjectName("cbSubtitle");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    auto* div = new QWidget;
    div->setObjectName("cbDivider");
    div->setFixedHeight(1);

    m_restBtn = new QPushButton;
    m_restBtn->setObjectName("cbRestBtn");
    m_restBtn->setCursor(Qt::PointingHandCursor);

    m_continueBtn = new QPushButton;
    m_continueBtn->setObjectName("cbContinueBtn");
    m_continueBtn->setCursor(Qt::PointingHandCursor);

    layout->addWidget(icon);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_subtitleLabel);
    layout->addWidget(div);
    layout->addSpacing(2);
    layout->addWidget(m_restBtn);
    layout->addWidget(m_continueBtn);

    outerLayout->addWidget(card);

    card->installEventFilter(this);
    icon->installEventFilter(this);
    m_titleLabel->installEventFilter(this);
    m_subtitleLabel->installEventFilter(this);

    setStyleSheet(R"QSS(
CompanyBreakDialog { background: transparent; }
QWidget {
    font-family: sans-serif;
    background: transparent;
}
#cbCard {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #1a1f38, stop:1 #12162a);
    border-radius: 16px;
    border: 1px solid rgba(74,144,226,0.25);
}
#cbIcon { font-size: 36px; background: transparent; }
#cbTitle {
    font-size: 17px; font-weight: bold; color: #eaeaea;
}
#cbSubtitle { font-size: 12px; color: #6666aa; }
#cbDivider { background: rgba(74,144,226,0.15); }
#cbRestBtn {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #4a90e2, stop:1 #2c68b8);
    color: white; border: none; border-radius: 10px;
    padding: 11px; font-size: 13px; font-weight: bold;
}
#cbRestBtn:hover {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #5fa3f5, stop:1 #3d7fd1);
}
#cbContinueBtn {
    background: rgba(255,255,255,0.03);
    color: #7070a0;
    border: 1px solid rgba(255,255,255,0.08);
    border-radius: 10px; padding: 9px; font-size: 12px;
}
#cbContinueBtn:hover { color: #ccccdd; border-color: rgba(74,144,226,0.4); }
)QSS");

    connect(m_restBtn, &QPushButton::clicked, this, [this]() {
        emit restRequested();
        accept();
    });
    connect(m_continueBtn, &QPushButton::clicked, this, [this]() {
        emit continueWorkRequested();
        accept();
    });
}

void CompanyBreakDialog::retranslateUi()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (isJa) {
        m_titleLabel->setText("会社の休憩時間です");
        m_subtitleLabel->setText("いますぐ休憩しますか？");
        m_restBtn->setText("☕  休憩する");
        m_continueBtn->setText("⚡  作業を継続する");
    } else {
        m_titleLabel->setText("Company Break Time");
        m_subtitleLabel->setText("Would you like to rest now?");
        m_restBtn->setText("☕  Take a Break");
        m_continueBtn->setText("⚡  Keep Working");
    }
}

bool CompanyBreakDialog::eventFilter(QObject* watched, QEvent* event)
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

void CompanyBreakDialog::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_dragPos = e->globalPos() - frameGeometry().topLeft();
    QDialog::mousePressEvent(e);
}

void CompanyBreakDialog::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton)
        move(e->globalPos() - m_dragPos);
    QDialog::mouseMoveEvent(e);
}
