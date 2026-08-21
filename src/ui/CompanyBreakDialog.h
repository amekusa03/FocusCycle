#pragma once
#include <QDialog>
#include <QPoint>
class QLabel;
class QPushButton;

class CompanyBreakDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CompanyBreakDialog(QWidget* parent = nullptr);
    void retranslateUi();

signals:
    void restRequested();
    void continueWorkRequested();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUi();
    QLabel*      m_titleLabel    = nullptr;
    QLabel*      m_subtitleLabel = nullptr;
    QPushButton* m_restBtn       = nullptr;
    QPushButton* m_continueBtn   = nullptr;
    QPoint       m_dragPos;
};
