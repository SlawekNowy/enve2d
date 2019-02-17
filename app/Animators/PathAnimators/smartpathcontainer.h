﻿#ifndef SMARTPATHCONTAINER_H
#define SMARTPATHCONTAINER_H
#include "simplemath.h"
#include "skia/skiaincludes.h"
#include "exceptions.h"
#include "pointhelpers.h"
#include "nodelist.h"

#include "framerange.h"
#include "smartPointers/sharedpointerdefs.h"

class SmartPath {
public:
    SmartPath();
    SmartPath(const SmartPath & src);
    void actionRemoveNormalNode(const int& nodeId);

    void actionAddFirstNode(const QPointF& c0,
                            const QPointF& p1,
                            const QPointF& c2);

    void actionInsertNodeBetween(const int &prevId,
                                 const int& nextId,
                                 const QPointF &c0,
                                 const QPointF &p1,
                                 const QPointF &c2);
    void actionInsertNodeBetween(const int &prevId,
                                 const int& nextId,
                                 const qreal& t);

    void actionPromoteDissolvedNodeToNormal(const int& nodeId);

    void actionDisconnectNodes(const int& node1Id, const int& node2Id);

    void actionConnectNodes(const int& node1Id, const int& node2Id);

    void removeNodeWithIdAndTellPrevToDoSame(const int& nodeId);

    void removeNodeWithIdAndTellNextToDoSame(const int& nodeId);

    void setPrev(SmartPath * const prev);
    void setNext(SmartPath * const next);

    void setDissolvedNodeT(const int& nodeId, const qreal& t) {
        Node& node = mNodes->at(nodeId);
        if(!node.isDissolved()) RuntimeThrow("Setting dissolved node value "
                                             "on a node of a different type");
        node.fT = t;
    }

    void setNormalNodeValues(const int& nodeId,
                             const QPointF& c0,
                             const QPointF& p1,
                             const QPointF& c2) {
        Node& node = mNodes->at(nodeId);
        if(!node.isNormal()) RuntimeThrow("Setting normal node values "
                                          "on a node of a different type");
        node.fC0 = c0;
        node.fP1 = p1;
        node.fC2 = c2;
    }

    SkPath getPathAt() const;
    SkPath getPathForPrev() const;
    SkPath getPathForNext() const;

    stdsptr<NodeList> getNodesListForPrev() const {
        if(!mPrev) return SPtrCreate(NodeList)(mNodes.get());
        return getNodesListFor(mPrev);
    }

    stdsptr<NodeList> getNodesListForNext() const {
        if(!mNext) return SPtrCreate(NodeList)(mNodes.get());
        return getNodesListFor(mNext);
    }

    SkPath interpolateWithNext(const qreal& nextWeight) const;
    SkPath interpolateWithPrev(const qreal& prevWeight) const;
    stdsptr<NodeList> interpolateNodesListWithNext(
            const qreal& nextWeight) const;
    stdsptr<NodeList> interpolateNodesListWithPrev(
            const qreal& prevWeight) const;

    void save() {
        mSaved = SPtrCreate(NodeList)(mNodes.get());
    }

    void restore() {
        if(mSaved == nullptr) return;
        mNodes = mSaved;
        mSaved.reset();

        updateAllNodesTypeAfterNeighbourChanged();
        if(mPrev) mPrev->updateAllNodesTypeAfterNeighbourChanged();
        if(mNext) mNext->updateAllNodesTypeAfterNeighbourChanged();
    }
protected:
    void updateAllNodesTypeAfterNeighbourChanged() {
        mNodes->updateAllNodesTypeAfterNeighbourChanged();
    }

    NodeList *getNodes() const;
private:
    stdsptr<NodeList> getNodesListFor(SmartPath * const neighbour) const;
    SkPath getPathFor(SmartPath * const neighbour) const;
    void insertNodeBetween(const int &prevId, const int &nextId,
                           const Node &nodeBlueprint);

    SmartPath * mPrev = nullptr;
    SmartPath * mNext = nullptr;
    stdsptr<NodeList> mNodes;
    stdsptr<NodeList> mSaved;
};

#endif // SMARTPATHCONTAINER_H
