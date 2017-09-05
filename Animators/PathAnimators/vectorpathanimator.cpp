#include "vectorpathanimator.h"
#include "Animators/pathanimator.h"
#include "Boxes/boundingbox.h"
#include "edge.h"
#include "nodepoint.h"
#include "canvas.h"

VectorPathAnimator::VectorPathAnimator(PathAnimator *pathAnimator) :
    Animator() {
    prp_setName("path");
    mParentPathAnimator = pathAnimator;
}

void VectorPathAnimator::prp_setAbsFrame(const int &frame) {
    Animator::prp_setAbsFrame(frame);
    //setCurrentPath(getPathAtRelFrame(frame));
    if(prp_hasKeys()) {
        setElementsFromSkPath(getPathAtRelFrame(frame, false));
    }
}

void VectorPathAnimator::setElementsFromSkPath(const SkPath &path) {
    PathContainer::setElementsFromSkPath(path);
    updateNodePointsFromCurrentPath();
}

void VectorPathAnimator::anim_saveCurrentValueToKey(PathKey *key) {
    key->setElementsFromSkPath(getPath());
}

void VectorPathAnimator::setCurrentPosForPtWithId(const int &ptId,
                                                  const SkPoint &pos,
                                                  const bool &finish) {
    setElementPos(ptId, pos);
    if(finish) {
        if(anim_mKeyOnCurrentFrame == NULL) {
            anim_saveCurrentValueAsKey();
        } else {
            ((PathKey*)anim_mKeyOnCurrentFrame)->setElementPos(ptId, pos);
        }
    }
}

void VectorPathAnimator::startPathChange() {
    mPathChanged = true;
}

void VectorPathAnimator::cancelPathChange() {
    mPathChanged = false;
}

void VectorPathAnimator::finishedPathChange() {
    if(mPathChanged) {
        mPathChanged = false;
        if(prp_isRecording()) {
            anim_saveCurrentValueAsKey();
        }
    }
}

void VectorPathAnimator::setElementPos(const int &index,
                                       const SkPoint &pos) {
    PathContainer::setElementPos(index, pos);
    prp_updateInfluenceRangeAfterChanged();
}

void VectorPathAnimator::anim_saveCurrentValueAsKey() {
    if(!anim_mIsRecording) prp_setRecording(true);

    if(anim_mKeyOnCurrentFrame == NULL) {
        anim_mKeyOnCurrentFrame = new PathKey(anim_mCurrentRelFrame,
                                              getPath(),
                                              mElementsPos,
                                              this);
        anim_appendKey(anim_mKeyOnCurrentFrame,
                       true,
                       false);
    } else {
        anim_saveCurrentValueToKey((PathKey*)anim_mKeyOnCurrentFrame);
    }
}

SkPath VectorPathAnimator::getPathAtRelFrame(const int &relFrame,
                                             const bool &considerCurrent) {
    if(relFrame == anim_mCurrentRelFrame && considerCurrent) return getPath();
    int prevId;
    int nextId;
    if(anim_getNextAndPreviousKeyIdForRelFrame(&prevId, &nextId, relFrame) ) {
        if(prevId == nextId) {
            return ((PathKey*)anim_mKeys.at(prevId).get())->getPath();
        }
        PathKey *prevKey = ((PathKey*)anim_mKeys.at(prevId).get());
        PathKey *nextKey = ((PathKey*)anim_mKeys.at(nextId).get());
        int prevRelFrame = prevKey->getRelFrame();
        int nextRelFrame = nextKey->getRelFrame();
        SkScalar weight = ((SkScalar)relFrame - prevRelFrame)/
                (nextRelFrame - prevRelFrame);
        SkPath outPath;
        nextKey->getPath().interpolate(prevKey->getPath(),
                                       weight, &outPath);
        return outPath;
    }
    return getPath();
}

void VectorPathAnimator::anim_removeKey(Key *keyToRemove,
                                        const bool &saveUndoRedo) {
    Animator::anim_removeKey(keyToRemove, saveUndoRedo);
    if(anim_mKeys.count() == 0) {
        setElementsFromSkPath(getPath());
    }
}

VectorPathEdge *VectorPathAnimator::getEdge(const QPointF &absPos,
                                            const qreal &canvasScaleInv) {
    qreal pressedT;
    NodePoint *prevPoint = NULL;
    NodePoint *nextPoint = NULL;
    if(getTAndPointsForMouseEdgeInteraction(absPos, &pressedT,
                                            &prevPoint, &nextPoint,
                                            canvasScaleInv)) {
        if(pressedT > 0.0001 && pressedT < 0.9999 && prevPoint && nextPoint) {
            VectorPathEdge *edge = prevPoint->getNextEdge();
            edge->setPressedT(pressedT);
            return edge;
        } else {
            return NULL;
        }
    }
    return NULL;
}

bool VectorPathAnimator::getTAndPointsForMouseEdgeInteraction(
                                          const QPointF &absPos,
                                          qreal *pressedT,
                                          NodePoint **prevPoint,
                                          NodePoint **nextPoint,
                                          const qreal &canvasScaleInv) {
    const QMatrix &combinedTransform =
            mParentPathAnimator->getParentBox()->getCombinedTransform();
    qreal xScaling = combinedTransform.map(
                        QLineF(0., 0., 1., 0.)).length();
    qreal yScaling = combinedTransform.map(
                        QLineF(0., 0., 0., 1.)).length();
    qreal maxDistX = 4./xScaling*canvasScaleInv;
    qreal maxDistY = 4./yScaling*canvasScaleInv;
    QPointF relPos = combinedTransform.inverted().map(absPos);

//    if(!mPath.intersects(distRect) ||
//        mPath.contains(distRect)) {
//        return false;
//    }

    QPointF nearestPos = relPos + QPointF(maxDistX, maxDistY)*2;
    qreal nearestT = 0.5;
    qreal nearestError = 1000000.;

    foreach(NodePoint *nodePoint, mPoints) {
        VectorPathEdge *edgeT = nodePoint->getNextEdge();
        if(edgeT == NULL) continue;
        QPointF posT;
        qreal tT;
        qreal errorT;
        edgeT->getNearestRelPosAndT(relPos,
                                    &posT,
                                    &tT,
                                    &errorT);
        if(errorT < nearestError) {
            nearestError = errorT;
            nearestT = tT;
            nearestPos = posT;
            *prevPoint = nodePoint;
        }
    }
    QPointF distT = nearestPos - relPos;
    if(qAbs(distT.x()) > maxDistX ||
       qAbs(distT.y()) > maxDistY) return false;

    *pressedT = nearestT;
    if(*prevPoint == NULL) return false;

    *nextPoint = (*prevPoint)->getNextPoint();
    if(*nextPoint == NULL) return false;

    return true;
}

NodePoint *VectorPathAnimator::createNewPointOnLineNear(
                                    const QPointF &absPos,
                                    const bool &adjust,
                                    const qreal &canvasScaleInv) {
    qreal pressedT;
    NodePoint *prevPoint = NULL;
    NodePoint *nextPoint = NULL;
    if(getTAndPointsForMouseEdgeInteraction(absPos, &pressedT,
                                            &prevPoint, &nextPoint,
                                            canvasScaleInv)) {
        if(pressedT > 0.0001 && pressedT < 0.9999) {
            int prevNodeId = prevPoint->getNodeId();
            int newNodeId = prevNodeId + 1;

            NodeSettings *prevNodeSettings =
                    getNodeSettingsForNodeId(prevNodeId);
            NodeSettings *nextNodeSettings;
            int nextNodeId;
            int prevPtId = nodeIdToPointId(prevNodeId);
            int nextPtId;
            if(nextPoint->getNodeId() == 0) {
                nextNodeSettings =
                        getNodeSettingsForNodeId(0);

                nextNodeId = 0;
                nextPtId = 1;
            } else {
                nextNodeSettings = getNodeSettingsForNodeId(prevNodeId + 1);
                nextPtId = nodeIdToPointId(prevNodeId + 1);
                nextNodeId = prevNodeId + 2;
            }

            bool newPtSmooth = false;

            
            NodePoint *newPoint = new NodePoint(this);
            NodeSettings *newNodeSettings;
            if(adjust) {
                if(!prevNodeSettings->endEnabled &&
                   !nextNodeSettings->startEnabled) {
                    newNodeSettings = insertNodeSettingsForNodeId(newNodeId,
                                                NodeSettings(false, false,
                                                             CTRLS_CORNER));
                } else {
                    newPtSmooth = true;
                    newNodeSettings = insertNodeSettingsForNodeId(newNodeId,
                                                NodeSettings(true, true,
                                                             CTRLS_SMOOTH));
                    if(prevNodeSettings->ctrlsMode == CTRLS_SYMMETRIC &&
                       prevNodeSettings->endEnabled &&
                       prevNodeSettings->startEnabled) {
                        replaceNodeSettingsForNodeId(
                                    prevNodeId,
                                    NodeSettings(true, true,
                                                 CTRLS_SMOOTH));
                    }
                    if(nextNodeSettings->ctrlsMode == CTRLS_SYMMETRIC &&
                       nextNodeSettings->endEnabled &&
                       nextNodeSettings->startEnabled) {
                        replaceNodeSettingsForNodeId(
                                    nextNodeId,
                                    NodeSettings(true, true,
                                                 CTRLS_SMOOTH));
                    }
                }

            } else {
                newNodeSettings = insertNodeSettingsForNodeId(newNodeId,
                                            NodeSettings(false, false,
                                                         CTRLS_CORNER));
            }
            newPoint->setCurrentNodeSettings(newNodeSettings);

            nextPoint->setPointAsPrevious(newPoint);
            prevPoint->setPointAsNext(newPoint);
            updateNodePointIds();

            if(anim_mKeys.isEmpty()) {
                addNewPointAtTBetweenPts(pressedT,
                                         prevPtId,
                                         nextPtId,
                                         newPtSmooth);
                nextPtId = nodeIdToPointId(nextNodeId);
                prevPoint->setElementsPos(SkPointToQPointF(mElementsPos.at(prevPtId - 1)),
                                         SkPointToQPointF(mElementsPos.at(prevPtId)),
                                         SkPointToQPointF(mElementsPos.at(prevPtId + 1)));
                newPoint->setElementsPos(SkPointToQPointF(mElementsPos.at(prevPtId + 2)),
                                         SkPointToQPointF(mElementsPos.at(prevPtId + 3)),
                                         SkPointToQPointF(mElementsPos.at(prevPtId + 4)));
                nextPoint->setElementsPos(SkPointToQPointF(mElementsPos.at(nextPtId - 1)),
                                         SkPointToQPointF(mElementsPos.at(nextPtId)),
                                         SkPointToQPointF(mElementsPos.at(nextPtId + 1)));
            } else {
                foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
                    ((PathKey*)key.get())->
                            addNewPointAtTBetweenPts(pressedT,
                                                     prevPtId,
                                                     nextPtId,
                                                     newPtSmooth);
                }
            }
            setElementsFromSkPath(getPathAtRelFrame(anim_mCurrentRelFrame));
            return newPoint;
        }
    }
    return NULL;
}

void VectorPathAnimator::updateNodePointsFromCurrentPath() {
    NodePoint *currOldNode = mFirstPoint;
    NodePoint *newFirstPt = NULL;
    int elementsCount = mElementsPos.count();
    int nodesCount = elementsCount/3;
    if(nodesCount == 0) return;
    NodePoint *lastP = NULL;
    for(int i = 0; i < nodesCount; i++) {
        int nodePtId = nodeIdToPointId(i);
        NodePoint *newP;
        if(currOldNode == NULL) {
            newP = new NodePoint(this);
            mPoints.append(newP);
        } else {
            newP = currOldNode;
            currOldNode = currOldNode->getNextPoint();
            if(currOldNode == mFirstPoint) {
                newP->setPointAsNext(NULL);
                currOldNode = NULL;
            }
        }
        newP->setCurrentNodeSettings(getNodeSettingsForNodeId(i));
        newP->setElementsPos(SkPointToQPointF(mElementsPos.at(nodePtId - 1)),
                             SkPointToQPointF(mElementsPos.at(nodePtId)),
                             SkPointToQPointF(mElementsPos.at(nodePtId + 1)));
        newP->setNodeId(i);

        if(i == 0) {
            newFirstPt = newP;
        } else {
            newP->setPointAsPrevious(lastP);
        }
        lastP = newP;
    }
    if(mPathClosed) {
        newFirstPt->setPointAsPrevious(lastP);
    }

    while(currOldNode != NULL && currOldNode != mFirstPoint) {
        NodePoint *oldOldNode = currOldNode;
        currOldNode = currOldNode->getNextPoint();
        mPoints.removeOne(oldOldNode);
        delete oldOldNode;
    }
    mFirstPoint = newFirstPt;
}

void VectorPathAnimator::removeNodeAtAndApproximate(const int &nodeId) {
    if(nodeId <= 0 || nodeId >= mElementsPos.count()/3) return;

    setNodeCtrlsMode(nodeId + 1, CtrlsMode::CTRLS_CORNER);
    setNodeCtrlsMode(nodeId - 1, CtrlsMode::CTRLS_CORNER);
    PathContainer::removeNodeAtAndApproximate(nodeId);
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        ((PathKey*)key.get())->
                removeNodeAtAndApproximate(nodeId);
    }
}

void VectorPathAnimator::removeNodeAt(const int &nodeId) {
    PathContainer::removeNodeAt(nodeId);
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        ((PathKey*)key.get())->
                removeNodeAt(nodeId);
    }
}

NodePoint *VectorPathAnimator::addNodeAbsPos(
        const QPointF &absPos,
        NodePoint *targetPt) {
    BoundingBox *parentBox = mParentPathAnimator->getParentBox();
    return addNodeRelPos(parentBox->mapAbsPosToRel(absPos),
                          targetPt);
}

NodePoint *VectorPathAnimator::addNodeRelPos(
        const QPointF &relPos,
        NodePoint *targetPt) {
    return addNodeRelPos(relPos, relPos, relPos, targetPt);
}

void VectorPathAnimator::selectAllPoints(Canvas *canvas) {
    Q_FOREACH(NodePoint *point, mPoints) {
        canvas->addPointToSelection(point);
    }
}

void VectorPathAnimator::applyTransformToPoints(
        const QMatrix &transform) {
    Q_FOREACH(NodePoint *point, mPoints) {
        point->applyTransform(transform);
    }
}

MovablePoint *VectorPathAnimator::getPointAtAbsPos(
                        const QPointF &absPtPos,
                        const CanvasMode &currentCanvasMode,
                        const qreal &canvasScaleInv) {
    Q_FOREACH(NodePoint *point, mPoints) {
        MovablePoint *pointToReturn =
                point->getPointAtAbsPos(absPtPos,
                                        currentCanvasMode,
                                        canvasScaleInv);
        if(pointToReturn == NULL) continue;
        return pointToReturn;
    }
    return NULL;
}

void VectorPathAnimator::selectAndAddContainedPointsToList(
                                const QRectF &absRect,
                                QList<MovablePoint *> *list) {
    Q_FOREACH(NodePoint *point, mPoints) {
        point->rectPointsSelection(absRect, list);
    }
}

void VectorPathAnimator::startAllPointsTransform() {
    Q_FOREACH(NodePoint *point, mPoints) {
        point->startTransform();
    }
}

void VectorPathAnimator::finishAllPointsTransform() {
    Q_FOREACH(NodePoint *point, mPoints) {
        point->finishTransform();
    }
}

void VectorPathAnimator::drawSelected(SkCanvas *canvas,
                                      const CanvasMode &currentCanvasMode,
                                      const qreal &invScale,
                                      const SkMatrix &combinedTransform) {

    if(currentCanvasMode == CanvasMode::MOVE_POINT) {
        for(int i = mPoints.count() - 1; i >= 0; i--) {
            NodePoint *point = mPoints.at(i);
            point->drawSk(canvas, currentCanvasMode, invScale);
        }
    } else if(currentCanvasMode == CanvasMode::ADD_POINT) {
        for(int i = mPoints.count() - 1; i >= 0; i--) {
            NodePoint *point = mPoints.at(i);
            if(point->isEndPoint() || point->isSelected()) {
                point->drawSk(canvas, currentCanvasMode, invScale);
            }
        }
    }
}

void VectorPathAnimator::connectPoints(NodePoint *pt1,
                                       NodePoint *pt2) {
    pt1->connectToPoint(pt2);
    setPathClosed(true);
}

void VectorPathAnimator::appendToPointsList(NodePoint *pt) {
    mPoints << pt;
}

void VectorPathAnimator::disconnectPoints(NodePoint *pt1,
                                          NodePoint *pt2) {
    pt1->disconnectFromPoint(pt2);
    setPathClosed(false);
}

NodePoint *VectorPathAnimator::createNewNode(const int &targetNodeId,
                                             const QPointF &startRelPos,
                                             const QPointF &relPos,
                                             const QPointF &endRelPos,
                                             const NodeSettings &nodeSettings) {
    NodeSettings *nodeSettingsPtr =
            insertNodeSettingsForNodeId(targetNodeId,
                                        nodeSettings);
    int nodePtId = nodeIdToPointId(targetNodeId) - 1;
    insertElementPos(nodePtId, QPointFToSkPoint(endRelPos - relPos));
    insertElementPos(nodePtId, QPointFToSkPoint(relPos));
    insertElementPos(nodePtId, QPointFToSkPoint(startRelPos - relPos));
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        ((PathKey*)key.get())->insertElementPos(
                    nodePtId,
                    QPointFToSkPoint(endRelPos - relPos));
        ((PathKey*)key.get())->insertElementPos(
                    nodePtId,
                    QPointFToSkPoint(relPos));
        ((PathKey*)key.get())->insertElementPos(
                    nodePtId,
                    QPointFToSkPoint(startRelPos - relPos));
    }

    NodePoint *newP = new NodePoint(this);
    newP->setCurrentNodeSettings(nodeSettingsPtr);
    newP->setElementsPos(startRelPos - relPos,
                         relPos,
                         endRelPos - relPos);
    return newP;
}

NodePoint *VectorPathAnimator::addNodeRelPos(
        const QPointF &startRelPos,
        const QPointF &relPos,
        const QPointF &endRelPos,
        NodePoint *targetPt,
        const NodeSettings &nodeSettings) {
    int targetNodeId;
    if(targetPt == NULL) {
        targetNodeId = 0;
    } else {
        if(targetPt->getNodeId() == 0 &&
           mFirstPoint->hasNextPoint()) {
            targetNodeId = 0;
        } else {
            targetNodeId = targetPt->getNodeId() + 1;
        }
    }

    NodePoint *newP = createNewNode(targetNodeId,
                                    startRelPos, relPos, endRelPos,
                                    nodeSettings);

    if(targetPt == NULL) {
        mFirstPoint = newP;
    } else {
        targetPt->connectToPoint(newP);
    }
    updateNodePointIds();
    prp_updateInfluenceRangeAfterChanged();
    return newP;
}

void VectorPathAnimator::updateNodePointIds() {
    if(mFirstPoint == NULL) return;
    int pointId = 0;
    NodePoint *nextPoint = mFirstPoint;
    while(true) {
        nextPoint->setNodeId(pointId);
        pointId++;
        nextPoint = nextPoint->getNextPoint();
        if(nextPoint == NULL || nextPoint == mFirstPoint) break;
    }
}