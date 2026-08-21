#pragma once
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>

/**
 * Pomodoro Session Record
 */
struct SessionRecord {
    QDateTime startTime;
    QDateTime endTime;
    QString   taskName;
    bool      completed; ///< true=Completed, false=Interrupted

    QString statusString() const {
        return completed ? QStringLiteral("Completed") : QStringLiteral("Interrupted");
    }
};

/**
 * Session Logger in CSV format
 * Saved to: ~/.local/share/FocusCycle/history.csv
 */
class SessionLogger : public QObject
{
    Q_OBJECT
public:
    static SessionLogger* instance();

    void logTaskStart(const QString& taskName);
    void logSession(const SessionRecord& record);
    const QVector<SessionRecord>& records() const { return m_records; }
    QStringList recentTaskNames() const;
    QString lastTaskName() const;

private:
    explicit SessionLogger(QObject* parent = nullptr);
    static SessionLogger* m_instance;

    QString m_logPath;
    QVector<SessionRecord> m_records;

    void loadRecords();
    void appendRecord(const SessionRecord& record);
};
