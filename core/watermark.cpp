#include "watermark.h"
#include <QFont>
#include <QFontMetrics>
#include <QPainter>

QImage applyWatermark(const QImage &source, const WatermarkSettings &s)
{
    QImage result = source.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    if (result.isNull() || s.text.isEmpty())
        return result;

    QPainter painter(&result);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setOpacity(s.opacity);

    QFont font;
    font.setPixelSize(qMax(1, int(result.width() * s.sizeFraction)));
    painter.setFont(font);
    painter.setPen(Qt::white);

    const QFontMetrics fm(font);
    const double textWidth = fm.horizontalAdvance(s.text);

    double xPos;
    double yPos;

    switch(s.hAlign){
    case HAlign::Left:
        xPos = s.marginFraction * result.width();
        break;

    case HAlign::Center:
        xPos = (result.width() / 2.0) - (textWidth / 2.0);
        break;

    case HAlign::Right:
        xPos = result.width() - textWidth - s.marginFraction * result.width();
        break;
    default:
        xPos = (result.width() / 2.0) - (textWidth / 2.0);
        break;
    }

    switch(s.vAlign){
    case VAlign::Bottom:
        yPos = result.height() - s.marginFraction * result.width() - fm.descent();
        break;

    case VAlign::Center:
        yPos = result.height() / 2.0 + (fm.ascent() - fm.descent()) / 2.0;
        break;

    case VAlign::Top:
        yPos = s.marginFraction * result.width() + fm.ascent();
        break;
    default:
        yPos = result.height() / 2.0 + (fm.ascent() - fm.descent()) / 2.0;
        break;
    }

    const QPointF pos(xPos, yPos);
    painter.drawText(pos, s.text);

    return result;
}