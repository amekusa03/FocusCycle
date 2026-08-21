#pragma once
#include <QWidget>
class QTableWidget;
class QLabel;
class QPushButton;

class LogWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LogWindow(QWidget* parent = nullptr);
    void refresh();
    void retranslateUi();

private:
    void setupUi();
    QTableWidget* m_table        = nullptr;
    QLabel*       m_titleLabel   = nullptr;
    QPushButton*  m_refreshBtn   = nullptr;
};
