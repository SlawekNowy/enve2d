#include "drawableautotiledsurface.h"
#include "castmacros.h"
#include "skia/skiahelpers.h"

DrawableAutoTiledSurface::DrawableAutoTiledSurface() :
    mRowCount(mTileImgs.fRowCount),
    mColumnCount(mTileImgs.fColumnCount),
    mZeroTileRow(mTileImgs.fZeroTileRow),
    mZeroTileCol(mTileImgs.fZeroTileCol),
    mImgs(mTileImgs.fImgs) {}

void DrawableAutoTiledSurface::drawOnCanvas(SkCanvas * const canvas,
                                            const QPoint &dst,
                                            const QRect * const minPixSrc,
                                            SkPaint * const paint) const {
    const QRect maxRect = tileBoundingRect();
    QRect tileRect;
    if(minPixSrc) {
        const QRect tileSrc = pixRectToTileRect(*minPixSrc);
        if(!tileSrc.intersects(maxRect)) return;
        tileRect = tileSrc.intersected(maxRect);
    } else tileRect = maxRect;
    for(int tx = tileRect.left(); tx <= tileRect.right(); tx++) {
        const SkScalar drawX = dst.x() + tx*TILE_SIZE;
        for(int ty = tileRect.top(); ty <= tileRect.bottom(); ty++) {
            const auto img = imageForTile(tx, ty);
            if(!img) continue;
            const SkScalar drawY = dst.y() + ty*TILE_SIZE;
            canvas->drawImage(img, drawX, drawY, paint);
        }
    }
}

void TilesTmpFileDataSaver::writeToFile(QIODevice * const file) {
    file->write(rcConstChar(&mImages.fRowCount), sizeof(int));
    file->write(rcConstChar(&mImages.fColumnCount), sizeof(int));
    file->write(rcConstChar(&mImages.fZeroTileRow), sizeof(int));
    file->write(rcConstChar(&mImages.fZeroTileCol), sizeof(int));
    for(const auto& col : mImages.fImgs) {
        for(const auto& tile : col)
            SkiaHelpers::writeImg(tile, file);
    }
}

void TilesTmpFileDataLoader::readFromFile(QIODevice * const file) {
    file->read(rcChar(&mTileImgs.fRowCount), sizeof(int));
    file->read(rcChar(&mTileImgs.fColumnCount), sizeof(int));
    file->read(rcChar(&mTileImgs.fZeroTileRow), sizeof(int));
    file->read(rcChar(&mTileImgs.fZeroTileCol), sizeof(int));
    for(int i = 0; i < mTileImgs.fColumnCount; i++) {
        mTileImgs.fImgs.append(QList<sk_sp<SkImage>>());
        auto& col = mTileImgs.fImgs.last();
        for(int j = 0; j < mTileImgs.fZeroTileRow; j++)
            col.append(SkiaHelpers::readImg(file));
    }
}