#include "SettingsDialog.h"
#include "core/Settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QTimeEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QScreen>
#include <QApplication>
#include <QScrollArea>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
    setWindowFlags(Qt::Window);
    resize(460, 580);
    setupUi();
    loadSettings();
    retranslateUi();

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect sg = screen->availableGeometry();
        move(sg.center() - rect().center());
    }
}

void SettingsDialog::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("settingsScroll");

    auto* container = new QWidget;
    container->setObjectName("settingsContainer");
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(18);

    // 1. General Settings
    m_genGroup = new QGroupBox;
    m_genGroup->setObjectName("settingsGroup");
    auto* gl = new QHBoxLayout(m_genGroup);
    gl->setContentsMargins(16, 20, 16, 16);

    m_langLabel = new QLabel;
    m_langLabel->setObjectName("settingsLabel");

    m_langCombo = new QComboBox;
    m_langCombo->setObjectName("settingsCombo");
    m_langCombo->addItem("English", static_cast<int>(Settings::Language::English));
    m_langCombo->addItem("日本語",   static_cast<int>(Settings::Language::Japanese));

    gl->addWidget(m_langLabel);
    gl->addWidget(m_langCombo, 1);

    // 2. Timer Settings
    m_timerGroup = new QGroupBox;
    m_timerGroup->setObjectName("settingsGroup");
    auto* tl = new QVBoxLayout(m_timerGroup);
    tl->setContentsMargins(16, 20, 16, 16);
    tl->setSpacing(12);

    auto* workRow = new QHBoxLayout;
    m_workLabel = new QLabel;
    m_workLabel->setObjectName("settingsLabel");
    m_workSpin = new QSpinBox;
    m_workSpin->setObjectName("settingsSpin");
    m_workSpin->setRange(1, 55);
    workRow->addWidget(m_workLabel);
    workRow->addWidget(m_workSpin, 1);

    auto* breakRow = new QHBoxLayout;
    m_breakLabel = new QLabel;
    m_breakLabel->setObjectName("settingsLabel");
    m_breakSpin = new QSpinBox;
    m_breakSpin->setObjectName("settingsSpin");
    m_breakSpin->setRange(1, 15);
    breakRow->addWidget(m_breakLabel);
    breakRow->addWidget(m_breakSpin, 1);

    tl->addLayout(workRow);
    tl->addLayout(breakRow);

    // 3. Idle Detection Settings
    m_idleGroup = new QGroupBox;
    m_idleGroup->setObjectName("settingsGroup");
    auto* il = new QVBoxLayout(m_idleGroup);
    il->setContentsMargins(16, 20, 16, 16);
    il->setSpacing(8);

    m_idleLabel = new QLabel;
    m_idleLabel->setObjectName("settingsSubLabel");
    m_idleLabel->setWordWrap(true);

    auto* sliderRow = new QHBoxLayout;
    m_idleMinLabel = new QLabel("0");
    m_idleMinLabel->setObjectName("idleMinMax");
    m_idleMaxLabel = new QLabel("30");
    m_idleMaxLabel->setObjectName("idleMinMax");

    m_idleSlider = new QSlider(Qt::Horizontal);
    m_idleSlider->setObjectName("settingsSlider");
    m_idleSlider->setRange(0, 30);
    m_idleSlider->setTickPosition(QSlider::TicksBelow);
    m_idleSlider->setTickInterval(5);

    sliderRow->addWidget(m_idleMinLabel);
    sliderRow->addWidget(m_idleSlider, 1);
    sliderRow->addWidget(m_idleMaxLabel);

    il->addWidget(m_idleLabel);
    il->addLayout(sliderRow);

    // 4. Company Breaks (Max 10)
    m_compGroup = new QGroupBox;
    m_compGroup->setObjectName("settingsGroup");
    auto* cl = new QVBoxLayout(m_compGroup);
    cl->setContentsMargins(16, 20, 16, 16);
    cl->setSpacing(8);

    auto* breakScroll = new QScrollArea;
    breakScroll->setWidgetResizable(true);
    breakScroll->setFrameShape(QFrame::NoFrame);
    breakScroll->setObjectName("breakScroll");
    breakScroll->setMaximumHeight(160);

    auto* breakWidget = new QWidget;
    breakWidget->setObjectName("breakArea");
    m_breakRowLayout = new QVBoxLayout(breakWidget);
    m_breakRowLayout->setContentsMargins(0, 0, 0, 0);
    m_breakRowLayout->setSpacing(6);
    m_breakRowLayout->addStretch();

    breakScroll->setWidget(breakWidget);

    auto* btnRow = new QHBoxLayout;
    m_addBreakBtn = new QPushButton;
    m_addBreakBtn->setObjectName("addBreakBtn");
    m_addBreakBtn->setCursor(Qt::PointingHandCursor);

    m_removeBreakBtn = new QPushButton;
    m_removeBreakBtn->setObjectName("removeBreakBtn");
    m_removeBreakBtn->setCursor(Qt::PointingHandCursor);

    btnRow->addWidget(m_addBreakBtn);
    btnRow->addWidget(m_removeBreakBtn);

    cl->addWidget(breakScroll);
    cl->addLayout(btnRow);

    // 5. Disclaimer Notice
    m_disclaimerLabel = new QLabel;
    m_disclaimerLabel->setObjectName("disclaimerLabel");
    m_disclaimerLabel->setWordWrap(true);

    layout->addWidget(m_genGroup);
    layout->addWidget(m_timerGroup);
    layout->addWidget(m_idleGroup);
    layout->addWidget(m_compGroup);
    layout->addWidget(m_disclaimerLabel);

    scrollArea->setWidget(container);
    rootLayout->addWidget(scrollArea, 1);

    // Bottom Dialog Buttons
    m_btnBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    m_btnBox->setObjectName("settingsBtnBox");
    rootLayout->addWidget(m_btnBox);

    setStyleSheet(R"QSS(
SettingsDialog {
    background-color: #14152a;
    color: #eaeaea;
    font-family: sans-serif;
}
#settingsScroll, #settingsContainer {
    background-color: #14152a;
}
#settingsGroup {
    background-color: #1a1b32;
    border: 1px solid #252640;
    border-radius: 12px;
    font-size: 13px;
    font-weight: bold;
    color: #ffffff;
    margin-top: 8px;
}
#settingsGroup::title {
    subcontrol-origin: margin;
    left: 14px;
    padding: 0 6px;
    color: #ffffff;
}
#settingsLabel {
    font-size: 13px;
    color: #9999cc;
}
#settingsSubLabel {
    font-size: 12px;
    color: #aaaacc;
    background: transparent;
}
#idleMinMax {
    font-size: 11px;
    color: #5555aa;
}
#disclaimerLabel {
    font-size: 10px;
    color: #555577;
    padding: 4px 6px;
}
#settingsCombo, #settingsSpin, #breakTimeEdit, #breakEndTimeEdit {
    background: rgba(255,255,255,0.05);
    color: #eaeaea;
    border: 1px solid rgba(255,255,255,0.1);
    border-radius: 8px;
    padding: 6px 10px;
    font-size: 13px;
}
#settingsCombo QAbstractItemView {
    background-color: #1a1b32;
    color: #eaeaea;
    selection-background-color: #4a90e2;
}
#settingsSlider::groove:horizontal {
    height: 6px;
    background: rgba(255,255,255,0.08);
    border-radius: 3px;
}
#settingsSlider::sub-page:horizontal {
    background: #4a90e2;
    border-radius: 3px;
}
#settingsSlider::handle:horizontal {
    background: #4a90e2;
    width: 16px;
    height: 16px;
    margin: -5px 0;
    border-radius: 8px;
}
#breakArea { background: transparent; }
#breakRowWidget {
    background: rgba(255,255,255,0.02);
    border-radius: 8px;
    border: 1px solid rgba(255,255,255,0.04);
}
#breakRowWidget:hover {
    background: rgba(255,255,255,0.03);
}
#breakRowLabel { color: #6666aa; font-size: 12px; min-width: 50px; }
#breakArrowLabel { color: #5555aa; font-size: 13px; }
#addBreakBtn {
    background: rgba(74,144,226,0.15);
    color: #4a90e2;
    border: 1px solid rgba(74,144,226,0.3);
    border-radius: 8px;
    padding: 6px 14px;
    font-size: 12px;
}
#addBreakBtn:hover { background: rgba(74,144,226,0.25); }
#removeBreakBtn {
    background: rgba(255,255,255,0.06);
    color: #ccccdd;
    border: 1px solid rgba(255,255,255,0.1);
    border-radius: 8px;
    padding: 6px 14px;
    font-size: 12px;
}
#removeBreakBtn:hover { background: rgba(255,255,255,0.12); color: white; }
#settingsBtnBox {
    background-color: #14152a;
    padding: 12px 24px;
    border-top: 1px solid #20213d;
}
#settingsBtnBox QPushButton {
    background: rgba(255,255,255,0.06);
    color: #ccccdd;
    border: 1px solid rgba(255,255,255,0.1);
    border-radius: 8px;
    padding: 7px 20px;
    font-size: 13px;
    min-width: 75px;
}
#settingsBtnBox QPushButton[text="Save"],
#settingsBtnBox QPushButton[text="保存"] {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #4a90e2, stop:1 #2c68b8);
    color: white;
    border: none;
    font-weight: bold;
}
)QSS");

    connect(m_idleSlider, &QSlider::valueChanged,
            this, &SettingsDialog::onUpdateIdleLabel);
    connect(m_addBreakBtn, &QPushButton::clicked,
            this, &SettingsDialog::onAddBreak);
    connect(m_removeBreakBtn, &QPushButton::clicked,
            this, &SettingsDialog::onRemoveBreak);
    connect(m_btnBox, &QDialogButtonBox::accepted,
            this, &SettingsDialog::onSave);
    connect(m_btnBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

void SettingsDialog::loadSettings()
{
    Settings* s = Settings::instance();

    int langIdx = m_langCombo->findData(static_cast<int>(s->language()));
    if (langIdx >= 0) m_langCombo->setCurrentIndex(langIdx);

    m_workSpin->setValue(s->workMinutes());
    m_breakSpin->setValue(s->breakMinutes());
    m_idleSlider->setValue(s->idleThresholdSeconds());
    onUpdateIdleLabel(s->idleThresholdSeconds());

    for (const CompanyBreak& cb : s->companyBreaks()) {
        onAddBreak();
        if (!m_timeEdits.isEmpty()) {
            m_timeEdits.last().first->setTime(cb.startTime);
            QTimeEdit* endEdit = m_timeEdits.last().second;
            if (cb.hasEndTime()) {
                auto* row = m_breakRows.last();
                auto* chk = row->findChild<QCheckBox*>("endChk");
                if (chk) chk->setChecked(true);
                endEdit->setEnabled(true);
                endEdit->setVisible(true);
                endEdit->setTime(cb.endTime);
            }
        }
    }
}

void SettingsDialog::retranslateUi()
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (isJa) {
        setWindowTitle("FocusCycle — 設定");
        m_genGroup->setTitle("🌐  全般設定");
        m_langLabel->setText("言語 (Language)：");
        m_timerGroup->setTitle("⏱  タイマー設定");
        m_workLabel->setText("作業時間：");
        m_breakLabel->setText("休憩時間：");
        m_workSpin->setSuffix(" 分");
        m_breakSpin->setSuffix(" 分");
        m_idleGroup->setTitle("🖱  操作検出しきい値");
        m_idleMinLabel->setText("0秒\n（即時）");
        m_idleMaxLabel->setText("30秒");
        m_compGroup->setTitle("🏢  会社の休憩時間（最大10件）");
        m_addBreakBtn->setText("＋  追加");
        m_removeBreakBtn->setText("－  削除");
        m_btnBox->button(QDialogButtonBox::Save)->setText("保存");
        m_btnBox->button(QDialogButtonBox::Cancel)->setText("キャンセル");
        m_disclaimerLabel->setText("※ Pomodoro® および Pomodoro Technique® は Francesco Cirillo 氏の登録商標です。本アプリは同氏と提携・公認関係にあるものではありません。");
    } else {
        setWindowTitle("FocusCycle - Settings");
        m_genGroup->setTitle("🌐  General");
        m_langLabel->setText("Language:");
        m_timerGroup->setTitle("⏱  Timer Settings");
        m_workLabel->setText("Work Duration:");
        m_breakLabel->setText("Break Duration:");
        m_workSpin->setSuffix(" min");
        m_breakSpin->setSuffix(" min");
        m_idleGroup->setTitle("🖱  Idle Detection Threshold");
        m_idleMinLabel->setText("0 sec\n(Instant)");
        m_idleMaxLabel->setText("30 sec");
        m_compGroup->setTitle("🏢  Company Breaks (Max 10)");
        m_addBreakBtn->setText("＋  Add");
        m_removeBreakBtn->setText("－  Remove");
        m_btnBox->button(QDialogButtonBox::Save)->setText("Save");
        m_btnBox->button(QDialogButtonBox::Cancel)->setText("Cancel");
        m_disclaimerLabel->setText("Pomodoro® and The Pomodoro Technique® are registered trademarks of Francesco Cirillo. This application is not affiliated with, associated with, or endorsed by Francesco Cirillo.");
    }

    onUpdateIdleLabel(m_idleSlider->value());

    for (int i = 0; i < m_breakRows.size(); ++i) {
        auto* lbl = m_breakRows[i]->findChild<QLabel*>("breakRowLabel");
        if (lbl)
            lbl->setText(isJa ? QString("休憩 %1").arg(i + 1) : QString("Break %1").arg(i + 1));
        auto* chk = m_breakRows[i]->findChild<QCheckBox*>("endChk");
        if (chk)
            chk->setText(isJa ? "終了" : "End");
    }
}

void SettingsDialog::onUpdateIdleLabel(int secs)
{
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);
    if (secs == 0) {
        m_idleLabel->setText(isJa
            ? "作業タイマー完了後すぐに休憩ダイアログを表示"
            : "Show break dialog immediately when work timer finishes");
    } else {
        m_idleLabel->setText(isJa
            ? QString("未操作が %1 秒続いたら休憩ダイアログを表示").arg(secs)
            : QString("Show break dialog after %1 sec of inactivity").arg(secs));
    }
}

void SettingsDialog::onAddBreak()
{
    if (m_timeEdits.size() >= 10) return;

    int idx = m_timeEdits.size() + 1;
    bool isJa = (Settings::instance()->language() == Settings::Language::Japanese);

    auto* row = new QWidget;
    row->setObjectName("breakRowWidget");
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(8, 4, 8, 4);
    rl->setSpacing(8);

    auto* lbl = new QLabel(isJa ? QString("休憩 %1").arg(idx) : QString("Break %1").arg(idx));
    lbl->setObjectName("breakRowLabel");

    auto* startEdit = new QTimeEdit(QTime(12, 0));
    startEdit->setDisplayFormat("HH:mm");
    startEdit->setObjectName("breakTimeEdit");

    auto* arrow = new QLabel("→");
    arrow->setObjectName("breakArrowLabel");

    auto* chk = new QCheckBox(isJa ? "終了" : "End");
    chk->setObjectName("endChk");

    auto* endEdit = new QTimeEdit(QTime(13, 0));
    endEdit->setDisplayFormat("HH:mm");
    endEdit->setObjectName("breakEndTimeEdit");
    endEdit->setEnabled(false);
    endEdit->setVisible(false);

    connect(chk, &QCheckBox::toggled, row, [endEdit, arrow](bool on) {
        endEdit->setEnabled(on);
        endEdit->setVisible(on);
        arrow->setVisible(on);
    });
    arrow->setVisible(false);

    rl->addWidget(lbl);
    rl->addWidget(startEdit);
    rl->addWidget(arrow);
    rl->addWidget(chk);
    rl->addWidget(endEdit);
    rl->addStretch();

    int insertPos = m_breakRowLayout->count() - 1;
    m_breakRowLayout->insertWidget(insertPos, row);

    m_timeEdits.append(qMakePair(startEdit, endEdit));
    m_breakRows.append(row);
    m_addBreakBtn->setEnabled(m_timeEdits.size() < 10);
}

void SettingsDialog::onRemoveBreak()
{
    if (m_breakRows.isEmpty()) return;

    auto* row = m_breakRows.takeLast();
    m_timeEdits.removeLast();
    m_breakRowLayout->removeWidget(row);
    delete row;

    m_addBreakBtn->setEnabled(m_timeEdits.size() < 10);
}

void SettingsDialog::onSave()
{
    Settings* s = Settings::instance();
    auto lang = static_cast<Settings::Language>(m_langCombo->currentData().toInt());
    s->setLanguage(lang);
    s->setWorkMinutes(m_workSpin->value());
    s->setBreakMinutes(m_breakSpin->value());
    s->setIdleThresholdSeconds(m_idleSlider->value());

    QVector<CompanyBreak> breaks;
    for (auto& pair : m_timeEdits) {
        CompanyBreak cb;
        cb.startTime = pair.first->time();
        auto* chk = pair.first->parentWidget()->findChild<QCheckBox*>("endChk");
        if (chk && chk->isChecked())
            cb.endTime = pair.second->time();
        breaks.append(cb);
    }
    s->setCompanyBreaks(breaks);
    s->save();

    accept();
}
