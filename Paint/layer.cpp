#include "layer.h"
#include <QDebug>
#include "canvashandler.h"
#include "windowvariables.h"
#include "canvas.h"

typedef struct {
    float brush_size;
    float scale;
    const char *brush_file;
} SurfaceTestData;

Layer::Layer(QString layer_name_t,
             int width_t, int height_t) {
    setLayerName(layer_name_t);
    if(width_t != 0) {
        setNewPaintLibSurface(width_t, height_t);
    }
}

void Layer::setLayerName(const QString &layerName) {
    mLayerName = layerName;
}

const QString &Layer::getLayerName() {
    return mLayerName;
}

bool isVisible() { return true; }
bool isSelected() { return true; }

void Layer::clear() {
    paintlib_surface->clear();
}

void Layer::startStroke(const qreal &x_t,
                        const qreal &y_t,
                        const qreal &pressure,
                        Brush *brush) {
    qDebug() << "startStroke";
    mCurrentBrush = brush;
    paintlib_surface->startNewStroke(mCurrentBrush,
                                     x_t, y_t, pressure);
}

void Layer::setNewPaintLibSurface(int width_t, int height_t) {
    paintlib_surface = new Surface(width_t, height_t);
}

void Layer::tabletEvent(const qreal &x_t,
                        const qreal &y_t,
                        const ulong &time_stamp,
                        const qreal &pressure,
                        const bool &erase) {
    Q_UNUSED(time_stamp);
    qDebug() << "tableEvent";
    paintlib_surface->strokeTo(mCurrentBrush,
                               x_t, y_t, pressure, 100, erase);
}

void Layer::tabletReleaseEvent() {
    mCurrentBrush = NULL;
    qDebug() << "tableReleaseEvent";
}

void Layer::tabletPressEvent(const qreal &x_t,
                             const qreal &y_t,
                             const ulong &time_stamp,
                             const qreal &pressure,
                             const bool &erase,
                             Brush *brush) {
    qDebug() << "tablePressEvent";
    startStroke(x_t, y_t, pressure, brush);
    tabletEvent(x_t, y_t, time_stamp, pressure, erase);
}
