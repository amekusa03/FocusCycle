#include "LogWindow.h"
#include "core/SessionLogger.h"
#include "core/Settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QScreen>
#include <QApplication>
#include <QScrollBar>

LogWindow::LogWindow(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window);
    resize(680, 440);
    setupUi();
    retranslateUi();

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect sg = screen->availableGeometry();
        move(sg.center() - rect().center());
    }
}

void LogWindow::setupUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* header = new QWidget;
    header->setObjectName("logHeader");
    auto* hl = new QHBoxLayout(header);
    hl->setContentsMargins(24, 16, 24, 16);

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("logTitleLabel");

    m_refreshBtn = new QPushButton;
    m_refreshBtn->setObjectName("logRefreshBtn");
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setFixedSize(90, 32);

    hl->addWidget(m_titleLabel);
    hl->addStretch();
    hl->addWidget(m_refreshBtn);

    m_table = new QTableWidget;
    m_table->setObjectName("logTable");
    m_table->setColumnCount(4);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);

    root->addWidget(header);
    root->addWidget(m_table, 1);

    setStyleSheet(R"QSS(
LogWindow {
    background-color: #14152a;
    color: #eaeaea;
    font-family: sans-serif;
}
#logHeader {
    background-color: #1a1b32;
    border-bottom: 1px solid #252640;
}
#logTitleLabel {
    font-size: 15px;
    font-weight: bold;
    color: #eaeaea;
}
#logRefreshBtn {
    background: rgba(255,255,255,0.06);
    color: #9999cc;
    border: 1px solid rgba(255,255,255,0.08);
    border-radius: 8px;
    font-size: 12px;
}
#logRefreshBtn:hover { background: rgba(255,255,255,0.1); color: white; }
#logTable {
    background-color: #14152a;
    alternate-background-color: #181930;
    color: #ccccdd;
    border: none;
    font-size: 13px;
    gridline-color: transparent;
}
#logTable::item { padding: 8px 14px; border: none; }
#logTable::item:selected {
    background-color: rgba(74,144,226,0.18);
    color: white;
}
QHeaderView::section {
    background-color: #0f1020;
    color: #5555aa;
    padding: 8px 14px;
    border: none;
    font-size: 11px;
    font-weight: bold;
    text-transform: uppercase;
    letter-spacing: 1px;
}
QScrollBar:vertical {
    background: #14152a;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #2a2b5a;
    border-radius: 4px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
)QSS");

    connect(m_refreshBtn, &QPushButton::clicked, this, &LogWindow::refresh);
}

void LogWindow::retranslateUi()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (isJa) {
        setWindowTitle("FocusCycle — 作業ログ");
        m_titleLabel->setText("📋  作業ログ");
        m_refreshBtn->setText("↻  更新");
        m_table->setHorizontalHeaderLabels({"開始日時", "終了日時", "タスク名", "状態"});
    } else {
        setWindowTitle("FocusCycle - Session Log");
        m_titleLabel->setText("📋  Session Log");
        m_refreshBtn->setText("↻  Refresh");
        m_table->setHorizontalHeaderLabels({"Start Time", "End Time", "Task Name", "Status"});
    }
    refresh();
}

void LogWindow::refresh()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    const auto& records = SessionLogger::instance()->records();
    m_table->setRowCount(records.size());

    for (int i = 0; i < records.size(); ++i) {
        const auto& r = records[i];

        auto* startItem = new QTableWidgetItem(
            r.startTime.toString("yyyy/MM/dd  HH:mm"));
        auto* endItem   = new QTableWidgetItem(
            r.endTime.toString("yyyy/MM/dd  HH:mm"));

        QString defaultTask = isJa ? "(タスク未設定)" : "(No task set)";
        auto* taskItem  = new QTableWidgetItem(
            r.taskName.isEmpty() ? defaultTask : r.taskName);

        QString statusStr;
        if (r.completed) {
            statusStr = isJa ? "完了" : "Completed";
        } else {
            statusStr = isJa ? "中断" : "Interrupted";
        }

        auto* stItem = new QTableWidgetItem(statusStr);
        stItem->setForeground(r.completed
            ? QColor(0x2e, 0xe0, 0x8a)
            : QColor(0xff, 0xb3, 0x00));

        m_table->setItem(i, 0, startItem);
        m_table->setItem(i, 1, endItem);
        m_table->setItem(i, 2, taskItem);
        m_table->setItem(i, 3, stItem);
    }
}
