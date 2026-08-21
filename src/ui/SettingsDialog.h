#pragma once
#include <QDialog>
#include <QVector>
#include <QPair>
class QSpinBox;
class QSlider;
class QLabel;
class QVBoxLayout;
class QTimeEdit;
class QWidget;
class QPushButton;
class QComboBox;
class QGroupBox;
class QDialogButtonBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    void retranslateUi();

private slots:
    void onAddBreak();
    void onRemoveBreak();
    void onSave();
    void onUpdateIdleLabel(int secs);

private:
    void setupUi();
    void loadSettings();
    void applyStyle();

    // General
    QGroupBox* m_genGroup   = nullptr;
    QComboBox* m_langCombo  = nullptr;
    QLabel*    m_langLabel  = nullptr;

    // Timers
    QGroupBox* m_timerGroup = nullptr;
    QLabel*    m_workLabel  = nullptr;
    QLabel*    m_breakLabel = nullptr;
    QSpinBox*  m_workSpin   = nullptr;
    QSpinBox*  m_breakSpin  = nullptr;

    // Idle Detection
    QGroupBox* m_idleGroup    = nullptr;
    QSlider*   m_idleSlider   = nullptr;
    QLabel*    m_idleLabel    = nullptr;
    QLabel*    m_idleMinLabel = nullptr;
    QLabel*    m_idleMaxLabel = nullptr;

    // Company Breaks
    QGroupBox*          m_compGroup      = nullptr;
    QVBoxLayout*        m_breakRowLayout = nullptr;
    QWidget*            m_breakArea      = nullptr;
    // Each pair: (startTimeEdit, endTimeEdit). endTimeEdit may be hidden/unchecked.
    QVector<QPair<QTimeEdit*,QTimeEdit*>> m_timeEdits;
    QVector<QWidget*>   m_breakRows;

    QPushButton*      m_addBreakBtn    = nullptr;
    QPushButton*      m_removeBreakBtn = nullptr;
    QLabel*           m_disclaimerLabel= nullptr;
    QDialogButtonBox* m_btnBox         = nullptr;
};
