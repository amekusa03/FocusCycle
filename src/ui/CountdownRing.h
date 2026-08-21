#pragma once
#include <QWidget>
#include <QColor>
#include <QString>

/**
 * Circular Countdown Widget
 * Custom paint with QPainter. Progress (1.0 = Full -> 0.0 = Empty)
 */
class CountdownRing : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)
public:
    explicit CountdownRing(QWidget* parent = nullptr);

    qreal   progress()  const { return m_progress; }
    void    setProgress(qreal p);
    void    setTimeText(const QString& text);
    void    setSubText(const QString& text);
    void    setRingColor(const QColor& color);

    QSize sizeHint() const override { return QSize(150, 150); }
    QSize minimumSizeHint() const override { return QSize(100, 100); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    qreal   m_progress  = 1.0;
    QString m_timeText  = "25:00";
    QString m_subText;
    QColor  m_ringColor { 0xe9, 0x45, 0x60 };
    int     m_ringWidth = 8;
};
