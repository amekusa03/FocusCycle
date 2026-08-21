#pragma once
#include <QDialog>
#include <QPoint>
class QLabel;
class QPushButton;

class BreakDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BreakDialog(QWidget* parent = nullptr);
    void setTaskName(const QString& task);
    void retranslateUi();

signals:
    void continueRequested();
    void completeRequested();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUi();
    QLabel*       m_titleLabel   = nullptr;
    QLabel*       m_taskLabel    = nullptr;
    QLabel*       m_questionLabel= nullptr;
    QPushButton*  m_completeBtn  = nullptr;
    QPushButton*  m_continueBtn  = nullptr;
    QString       m_rawTaskName;
    QPoint        m_dragPos;
};
