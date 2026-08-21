#pragma once
#include <QObject>
#include <QSettings>
#include <QTime>
#include <QVector>

/**
 * A single company break entry with optional end time.
 * If endTime is invalid (QTime()), the break has no automatic end.
 */
struct CompanyBreak {
    QTime startTime;
    QTime endTime;   // invalid = no automatic end

    bool hasEndTime() const { return endTime.isValid(); }
};

/**
 * Application Settings persistence (QSettings/INI format)
 * Singleton pattern
 */
class Settings : public QObject
{
    Q_OBJECT
public:
    enum class Language {
        English = 0,
        Japanese = 1
    };
    Q_ENUM(Language)

    static Settings* instance();

    Language language() const { return m_language; }
    void setLanguage(Language lang);

    int workMinutes() const { return m_workMinutes; }
    void setWorkMinutes(int v);

    int breakMinutes() const { return m_breakMinutes; }
    void setBreakMinutes(int v);

    int idleThresholdSeconds() const { return m_idleThresholdSeconds; }
    void setIdleThresholdSeconds(int v);

    QVector<CompanyBreak> companyBreaks() const { return m_companyBreaks; }
    void setCompanyBreaks(const QVector<CompanyBreak>& breaks);

    void save();

signals:
    void settingsChanged();

private:
    explicit Settings(QObject* parent = nullptr);
    static Settings* m_instance;

    QSettings m_settings;
    Language m_language             = Language::English;
    int m_workMinutes              = 25;
    int m_breakMinutes             = 5;
    int m_idleThresholdSeconds     = 10;
    QVector<CompanyBreak> m_companyBreaks;
};
