#pragma once
#include <QImage>
#include <QString>

// Horizontal and vertical anchor choices for placing the watermark.
enum class HAlign { Left, Center, Right };
enum class VAlign { Top, Center, Bottom };

// Size and margin are fractions of the image width, so the result looks
// the same on photos of any resolution.
struct WatermarkSettings {
    QString text;
    double sizeFraction = 0.05;     // font pixel size as a fraction of image width
    double marginFraction = 0.03;   // distance from the image edge, as a fraction of image width
    double opacity = 0.5;           // 0.0 to 1.0
    HAlign hAlign = HAlign::Right;  // horizontal anchor
    VAlign vAlign = VAlign::Bottom; // vertical anchor
};

// Returns a new image with the watermark drawn on it. Does not modify source.
QImage applyWatermark(const QImage &source, const WatermarkSettings &settings);