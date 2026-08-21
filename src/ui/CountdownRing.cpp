#include "CountdownRing.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QRadialGradient>
#include <cmath>

CountdownRing::CountdownRing(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void CountdownRing::setProgress(qreal p)
{
    m_progress = qBound(0.0, p, 1.0);
    update();
}

void CountdownRing::setTimeText(const QString& text)
{
    m_timeText = text;
    update();
}

void CountdownRing::setSubText(const QString& text)
{
    m_subText = text;
    update();
}

void CountdownRing::setRingColor(const QColor& color)
{
    m_ringColor = color;
    update();
}

void CountdownRing::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    const int w  = width();
    const int h  = height();
    const int sz = qMin(w, h);
    const int margin = m_ringWidth + 4;

    QRectF ring((w - sz) / 2.0 + margin,
                (h - sz) / 2.0 + margin,
                sz - margin * 2,
                sz - margin * 2);

    // Background Circle
    p.setPen(Qt::NoPen);
    QRadialGradient bgGrad(ring.center(), ring.width() / 2.0);
    bgGrad.setColorAt(0.0, QColor(35, 35, 65, 200));
    bgGrad.setColorAt(1.0, QColor(20, 20, 46, 220));
    p.setBrush(bgGrad);
    p.drawEllipse(ring);

    // Track Ring
    QPen trackPen(QColor(255, 255, 255, 18), m_ringWidth, Qt::SolidLine, Qt::RoundCap);
    p.setPen(trackPen);
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(ring);

    // Progress Arc
    if (m_progress > 0.0) {
        QPen glowPen(QColor(m_ringColor.red(), m_ringColor.green(),
                            m_ringColor.blue(), 40),
                     m_ringWidth + 6, Qt::SolidLine, Qt::RoundCap);
        p.setPen(glowPen);
        int spanAngle = static_cast<int>(m_progress * 360.0 * 16);
        p.drawArc(ring, 90 * 16, -spanAngle);

        QPen arcPen(m_ringColor, m_ringWidth, Qt::SolidLine, Qt::RoundCap);
        p.setPen(arcPen);
        p.drawArc(ring, 90 * 16, -spanAngle);

        if (m_progress < 1.0 && m_progress > 0.01) {
            double angleDeg = 90.0 - m_progress * 360.0;
            double angleRad = angleDeg * M_PI / 180.0;
            double r2       = ring.width() / 2.0;
            double cx = ring.center().x() + r2 * std::cos(angleRad);
            double cy = ring.center().y() - r2 * std::sin(angleRad);

            QRadialGradient dot(cx, cy, 6);
            QColor dotColor = m_ringColor;
            dotColor.setAlpha(200);
            dot.setColorAt(0.0, dotColor);
            dotColor.setAlpha(0);
            dot.setColorAt(1.0, dotColor);
            p.setPen(Qt::NoPen);
            p.setBrush(dot);
            p.drawEllipse(QPointF(cx, cy), 6.0, 6.0);
        }
    }

    // Text Rendering
    const int innerPad = m_ringWidth + 8;
    QRectF textRect = ring.adjusted(innerPad, innerPad, -innerPad, -innerPad);

    int timeFontSize = qBound(12, static_cast<int>(sz * 0.13), 20);
    int subFontSize  = qBound(8, static_cast<int>(sz * 0.06), 11);

    if (m_subText.isEmpty()) {
        QFont f("Sans-Serif", timeFontSize, QFont::Bold);
        p.setFont(f);
        p.setPen(QColor(0xea, 0xea, 0xea));
        p.drawText(textRect, Qt::AlignCenter, m_timeText);
    } else {
        QRectF upper = textRect;
        upper.setBottom(textRect.center().y() + 4);
        QFont timeFont("Sans-Serif", timeFontSize, QFont::Bold);
        p.setFont(timeFont);
        p.setPen(QColor(0xea, 0xea, 0xea));
        p.drawText(upper, Qt::AlignHCenter | Qt::AlignBottom, m_timeText);

        QRectF lower = textRect;
        lower.setTop(textRect.center().y() + 6);
        QFont subFont("Sans-Serif", subFontSize);
        p.setFont(subFont);
        p.setPen(QColor(0x88, 0x88, 0xaa));
        p.drawText(lower, Qt::AlignHCenter | Qt::AlignTop, m_subText);
    }
}
