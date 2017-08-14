#include "vectorpath.h"
#include <QPainter>
#include "canvas.h"
#include <QDebug>
#include "undoredo.h"
#include "mainwindow.h"
#include "updatescheduler.h"
#include "pathpivot.h"
#include "pointhelpers.h"
#include "Animators/animatorupdater.h"
#include "pathpoint.h"
#include "Animators/pathanimator.h"
#include "gradientpoints.h"

VectorPath::VectorPath() :
    PathBox(BoundingBoxType::TYPE_VECTOR_PATH) {
    mPathAnimator =
            (new PathAnimator(this))->ref<PathAnimator>();
    setName("Path");
    mPathAnimator->prp_setUpdater(new PathPointUpdater(this));
    mPathAnimator->prp_blockUpdater();
    ca_addChildAnimator(mPathAnimator.data());
}

#include <QSqlError>
int VectorPath::saveToSql(QSqlQuery *query, const int &parentId) {
    int boundingBoxId = PathBox::saveToSql(query, parentId);
    mPathAnimator->saveToSql(query, boundingBoxId);

    return boundingBoxId;
}

void VectorPath::selectAllPoints(Canvas *canvas) {
    mPathAnimator->selectAllPoints(canvas);
}

PathPoint *VectorPath::createNewPointOnLineNear(const QPointF &absPos,
                                                const bool &adjust,
                                                const qreal &canvasScaleInv) {
    return mPathAnimator->createNewPointOnLineNear(absPos, adjust,
                                                   canvasScaleInv);
}

void VectorPath::loadFromSql(const int &boundingBoxId) {
    PathBox::loadFromSql(boundingBoxId);
    mPathAnimator->loadFromSql(boundingBoxId);
}

VectorPath *VectorPath::createPathFromSql(int boundingBoxId) {
    VectorPath *path = new VectorPath();
    path->loadFromSql(boundingBoxId);

    return path;
}

VectorPath::~VectorPath() {

}

PathAnimator *VectorPath::getPathAnimator() {
    return mPathAnimator.data();
}

void VectorPath::applyCurrentTransformation() {
    mPathAnimator->applyTransformToPoints(
                mTransformAnimator->getCurrentTransformationMatrix());

    mTransformAnimator->reset(true);
    centerPivotPosition(true);
}

VectorPathEdge *VectorPath::getEgde(const QPointF &absPos,
                                    const qreal &canvasScaleInv) {
    return mPathAnimator->getEgde(absPos, canvasScaleInv);
}

void VectorPath::loadPathFromSkPath(const SkPath &path) {
    mPathAnimator->loadPathFromSkPath(path);
}


//#include <QMenu>
//void VectorPath::showContextMenu(const QPoint &globalPos) {
//    QMenu menu(mMainWindow);

//    QAction *outlineScaled = new QAction("Scale outline");
//    outlineScaled->setCheckable(true);
//    outlineScaled->setChecked(mOutlineAffectedByScale);
//    menu.addAction(outlineScaled);

//    menu.addAction("Delete");
//    QAction *selected_action = menu.exec(globalPos);
//    if(selected_action != NULL)
//    {
//        if(selected_action->text() == "Delete")
//        {

//        } else if(selected_action == outlineScaled) {
//            setOutlineAffectedByScale(!mOutlineAffectedByScale);
//        }
//    } else {

//    }
//}

void VectorPath::drawSelectedSk(SkCanvas *canvas,
                              const CanvasMode &currentCanvasMode,
                              const SkScalar &invScale) {
    if(isVisibleAndInVisibleDurationRect()) {
        canvas->save();
        drawBoundingRectSk(canvas, invScale);
        mPathAnimator->drawSelected(canvas,
                                    currentCanvasMode,
                                    invScale,
                                    QMatrixToSkMatrix(getCombinedTransform()));
        if(currentCanvasMode == CanvasMode::MOVE_POINT) {
            mFillGradientPoints->drawGradientPointsSk(canvas, invScale);
            mStrokeGradientPoints->drawGradientPointsSk(canvas, invScale);
        } else if(currentCanvasMode == MOVE_PATH) {
            mTransformAnimator->getPivotMovablePoint()->
                    drawSk(canvas, invScale);
        }
        canvas->restore();
    }
}

MovablePoint *VectorPath::getPointAtAbsPos(const QPointF &absPtPos,
                                     const CanvasMode &currentCanvasMode,
                                     const qreal &canvasScaleInv) {
    MovablePoint *pointToReturn = PathBox::getPointAtAbsPos(absPtPos,
                                                            currentCanvasMode,
                                                            canvasScaleInv);
    if(pointToReturn == NULL) {
        pointToReturn = mPathAnimator->getPointAtAbsPos(absPtPos,
                                                      currentCanvasMode,
                                                      canvasScaleInv);
    }
    return pointToReturn;
}

void VectorPath::selectAndAddContainedPointsToList(const QRectF &absRect,
                                                   QList<MovablePoint *> *list) {
    mPathAnimator->selectAndAddContainedPointsToList(absRect, list);
}

SkPath VectorPath::getPathAtRelFrame(const int &relFrame) {
     return mPathAnimator->getPathAtRelFrame(relFrame);
}

void VectorPath::duplicatePathAnimatorFrom(
        PathAnimator *source) {
    source->duplicatePathsTo(mPathAnimator.data());
}

void VectorPath::makeDuplicate(Property *targetBox) {
    PathBox::makeDuplicate(targetBox);
    ((VectorPath*)targetBox)->
            duplicatePathAnimatorFrom(mPathAnimator.data());
}

BoundingBox *VectorPath::createNewDuplicate() {
    return new VectorPath();
}
