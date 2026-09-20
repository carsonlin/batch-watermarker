#pragma once
#include <QImage>
#include <QString>

enum class HAlign { Left, Center, Right };
enum class VAlign { Top, Center, Bottom };


struct WatermarkSettings {
    QString text;
    double sizeFraction = 0.05;    // font pixel size as a fraction of image width
    double marginFraction = 0.03;  // distance from the image edge, as a fraction of image width
    double opacity = 1.0;          // 0.0 to 1.0
    HAlign hAlign = HAlign::Center; // horizontal anchor
    VAlign vAlign = VAlign::Bottom; // vertical anchor
};

QImage applyWatermark(const QImage &source, const WatermarkSettings &settings);