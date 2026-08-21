#include "SessionLogger.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <algorithm>

SessionLogger* SessionLogger::m_instance = nullptr;

SessionLogger* SessionLogger::instance()
{
    if (!m_instance) {
        m_instance = new SessionLogger();
        qAddPostRoutine([]{ delete m_instance; m_instance = nullptr; });
    }
    return m_instance;
}

SessionLogger::SessionLogger(QObject* parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(dataDir);
    m_logPath = dataDir + "/history.csv";
    loadRecords();
}

void SessionLogger::loadRecords()
{
    QFile f(m_logPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&f);
    in.setCodec("UTF-8");
    if (!in.atEnd()) in.readLine(); // Skip header

    QVector<SessionRecord> tmp;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        int first  = line.indexOf(',');
        int second = line.indexOf(',', first + 1);
        int last   = line.lastIndexOf(',');
        if (first < 0 || second < 0 || last <= second) continue;
        SessionRecord r;
        r.startTime = QDateTime::fromString(line.left(first), Qt::ISODate);
        r.endTime   = QDateTime::fromString(line.mid(first + 1, second - first - 1), Qt::ISODate);
        r.taskName  = line.mid(second + 1, last - second - 1);
        QString status = line.mid(last + 1).trimmed();
        r.completed = (status == "Completed" || status == "完了");
        tmp.append(r);
    }
    std::reverse(tmp.begin(), tmp.end());
    m_records = tmp;
}

void SessionLogger::logTaskStart(const QString& taskName)
{
    SessionRecord r;
    r.startTime = QDateTime::currentDateTime();
    r.endTime   = r.startTime;
    r.taskName  = taskName;
    r.completed = false;
    logSession(r);
}

void SessionLogger::logSession(const SessionRecord& record)
{
    m_records.prepend(record);
    appendRecord(record);
}

void SessionLogger::appendRecord(const SessionRecord& record)
{
    bool needHeader = !QFile::exists(m_logPath);
    QFile f(m_logPath);
    if (!f.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&f);
    out.setCodec("UTF-8");
    if (needHeader)
        out << "Start Time,End Time,Task Name,Status\n";

    QString safeName = record.taskName;
    if (safeName.contains(',') || safeName.contains('"')) {
        safeName.replace(QLatin1String("\""), QLatin1String("\"\""));
        safeName = QLatin1String("\"") + safeName + QLatin1String("\"");
    }
    out << record.startTime.toString(Qt::ISODate) << ","
        << record.endTime.toString(Qt::ISODate)   << ","
        << safeName                                << ","
        << record.statusString()                   << "\n";
}

QStringList SessionLogger::recentTaskNames() const
{
    QStringList list;
    for (const auto& r : m_records) {
        QString task = r.taskName.trimmed();
        if (!task.isEmpty() && !list.contains(task)) {
            list.append(task);
            if (list.size() >= 5) break;
        }
    }
    return list;
}

QString SessionLogger::lastTaskName() const
{
    QStringList tasks = recentTaskNames();
    return tasks.isEmpty() ? QString() : tasks.first();
}
