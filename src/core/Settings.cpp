#include "Settings.h"
#include <QCoreApplication>
#include <algorithm>

Settings* Settings::m_instance = nullptr;

Settings* Settings::instance()
{
    if (!m_instance) {
        m_instance = new Settings();
        // Ensure the singleton is deleted when QCoreApplication exits.
        qAddPostRoutine([]{ delete m_instance; m_instance = nullptr; });
    }
    return m_instance;
}

Settings::Settings(QObject* parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope,
                 "FocusCycle", "FocusCycle")
{
    int langInt = m_settings.value("General/Language", static_cast<int>(Language::English)).toInt();
    m_language = (langInt == static_cast<int>(Language::Japanese)) ? Language::Japanese : Language::English;

    m_workMinutes          = m_settings.value("Timer/WorkMinutes", 25).toInt();
    m_breakMinutes         = m_settings.value("Timer/BreakMinutes", 5).toInt();
    m_idleThresholdSeconds = m_settings.value("IdleDetection/ThresholdSeconds", 10).toInt();

    int count = m_settings.value("CompanyBreaks/Count", 0).toInt();
    for (int i = 0; i < count; ++i) {
        QString key = QString("CompanyBreaks/Break%1").arg(i);
        QString val = m_settings.value(key).toString();
        // Format: "HH:mm" (start only) or "HH:mm-HH:mm" (start-end)
        CompanyBreak cb;
        if (val.contains('-')) {
            QStringList parts = val.split('-');
            cb.startTime = QTime::fromString(parts[0].trimmed(), "HH:mm");
            cb.endTime   = QTime::fromString(parts[1].trimmed(), "HH:mm");
        } else {
            cb.startTime = QTime::fromString(val.trimmed(), "HH:mm");
        }
        if (cb.startTime.isValid())
            m_companyBreaks.append(cb);
    }
}

void Settings::setLanguage(Language lang)
{
    if (m_language != lang) {
        m_language = lang;
        emit settingsChanged();
    }
}

void Settings::setWorkMinutes(int v)
{
    m_workMinutes = qBound(1, v, 55);
    emit settingsChanged();
}

void Settings::setBreakMinutes(int v)
{
    m_breakMinutes = qBound(1, v, 15);
    emit settingsChanged();
}

void Settings::setIdleThresholdSeconds(int v)
{
    m_idleThresholdSeconds = qBound(0, v, 30);
    emit settingsChanged();
}

void Settings::setCompanyBreaks(const QVector<CompanyBreak>& breaks)
{
    m_companyBreaks = breaks.mid(0, 10);
    emit settingsChanged();
}

void Settings::save()
{
    m_settings.setValue("General/Language",            static_cast<int>(m_language));
    m_settings.setValue("Timer/WorkMinutes",           m_workMinutes);
    m_settings.setValue("Timer/BreakMinutes",          m_breakMinutes);
    m_settings.setValue("IdleDetection/ThresholdSeconds", m_idleThresholdSeconds);
    m_settings.setValue("CompanyBreaks/Count", m_companyBreaks.size());
    for (int i = 0; i < m_companyBreaks.size(); ++i) {
        const CompanyBreak& cb = m_companyBreaks[i];
        QString val = cb.startTime.toString("HH:mm");
        if (cb.hasEndTime())
            val += "-" + cb.endTime.toString("HH:mm");
        m_settings.setValue(QString("CompanyBreaks/Break%1").arg(i), val);
    }
    m_settings.sync();
}
