#pragma once
#include <QWidget>
#include <QPoint>

class QStackedWidget;
class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class CountdownRing;

/**
 * Work Start & Countdown Window (2 Pages)
 * Frameless Window + Compact & Draggable
 */
class WorkStartWindow : public QWidget
{
    Q_OBJECT
public:
    explicit WorkStartWindow(QWidget* parent = nullptr);

    void showStartPage();
    void showWorkingPage();
    void updateWorkTick(int remainingSecs, int totalSecs);
    void setTaskName(const QString& task);
    void retranslateUi();

signals:
    void workStartRequested(const QString& taskName);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUi();
    void applyStyle();
    void centerOnScreen();
    void installDragFilter(QWidget* widget);

    QStackedWidget* m_stack       = nullptr;

    // ── Start Page ──────────────────────────────────────────
    QWidget*     m_startPage    = nullptr;
    QComboBox*   m_taskInput    = nullptr;
    QPushButton* m_startButton  = nullptr;
    QLabel*      m_logoLabel    = nullptr;
    QLabel*      m_logoSubLabel = nullptr;

    // ── Work Page ───────────────────────────────────────────
    QWidget*      m_workPage    = nullptr;
    CountdownRing* m_ring       = nullptr;
    QLabel*       m_taskLabel   = nullptr;
    QLabel*       m_statusLabel = nullptr;

    int     m_totalWorkSecs     = 25 * 60;
    int     m_lastRemainingSecs = 25 * 60;
    QString m_rawTaskName;
    QPoint  m_dragPos;
};
