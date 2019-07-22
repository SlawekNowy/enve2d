#include "minimalscrollwidgetvisiblepart.h"
#include <QPainter>
#include "singlewidgetabstraction.h"
#include "singlewidget.h"
#include "scrollwidget.h"
#include "singlewidgettarget.h"
#include "GUI/mainwindow.h"
#include "global.h"

MinimalScrollWidgetVisiblePart::MinimalScrollWidgetVisiblePart(
        MinimalScrollWidget * const parent) :
    QWidget(parent) {
    Q_ASSERT(parent);
    mParentWidget = parent;
}

void MinimalScrollWidgetVisiblePart::setVisibleTop(const int top) {
    mVisibleTop = top;
    updateVisibleWidgetsContent();
}

void MinimalScrollWidgetVisiblePart::setVisibleHeight(const int height) {
    setFixedHeight(height);
    mVisibleHeight = height;
    updateVisibleWidgets();
}

void MinimalScrollWidgetVisiblePart::updateWidgetsWidth() {
    for(const auto& widget : mSingleWidgets) {
        widget->setFixedWidth(width() - widget->x());
    }
}

void MinimalScrollWidgetVisiblePart::callUpdaters() {
    updateParentHeightIfNeeded();
    updateVisibleWidgetsContentIfNeeded();
    update();
}

void MinimalScrollWidgetVisiblePart::scheduleContentUpdate() {
    planScheduleUpdateParentHeight();
    planScheduleUpdateVisibleWidgetsContent();
}

bool MinimalScrollWidgetVisiblePart::event(QEvent *event) {
    if(event->type() == QEvent::User) {
        mEventSent = false;
        callUpdaters();
        return true;
    } else return QWidget::event(event);
}
#include <QApplication>
void MinimalScrollWidgetVisiblePart::postEvent() {
    if(mEventSent) return;
    QApplication::postEvent(this, new QEvent(QEvent::User));
    mEventSent = true;
}

void MinimalScrollWidgetVisiblePart::planScheduleUpdateVisibleWidgetsContent() {
    if(mVisibleWidgetsContentUpdateScheduled) return;
    mVisibleWidgetsContentUpdateScheduled = true;
    postEvent();
}

void MinimalScrollWidgetVisiblePart::updateVisibleWidgetsContentIfNeeded() {
    if(mVisibleWidgetsContentUpdateScheduled) {
        mVisibleWidgetsContentUpdateScheduled = false;
        updateVisibleWidgetsContent();
    }
}

void MinimalScrollWidgetVisiblePart::planScheduleUpdateParentHeight() {
    if(mParentHeightUpdateScheduled) return;
    mParentHeightUpdateScheduled = true;
    postEvent();
}

void MinimalScrollWidgetVisiblePart::updateParentHeightIfNeeded() {
    if(mParentHeightUpdateScheduled) {
        mParentHeightUpdateScheduled = false;
        updateParentHeight();
    }
}

void MinimalScrollWidgetVisiblePart::updateVisibleWidgets() {
    int neededWidgets = qCeil(mVisibleHeight/
                              static_cast<qreal>(MIN_WIDGET_DIM));
    int currentNWidgets = mSingleWidgets.count();

    if(neededWidgets == currentNWidgets) return;
    if(neededWidgets > currentNWidgets) {
        for(int i = neededWidgets - currentNWidgets; i > 0; i--) {
            mSingleWidgets.append(createNewSingleWidget());
        }
    } else {
        for(int i = currentNWidgets - neededWidgets; i > 0; i--) {
            delete mSingleWidgets.takeLast();
        }
    }

    int yT = 0;
    for(const auto& widget : mSingleWidgets) {
        widget->move(widget->x(), yT);
        widget->setFixedWidth(width() - widget->x());
        yT += MIN_WIDGET_DIM;
    }

    updateVisibleWidgetsContent();
}

void MinimalScrollWidgetVisiblePart::updateVisibleWidgetsContent() {
    int idP = 0;
//    int currX;
//    int currY;
//    currX = 0;
//    currY = MIN_WIDGET_HEIGHT/2;
//    mMainAbstraction->setAbstractions(
//                mVisibleTop,
//                mVisibleTop + mVisibleHeight + currY,
//                &currY, currX,
//                &mSingleWidgets,
//                &idP,
//                mCurrentRulesCollection,
//                true,
//                false);

    for(int i = idP; i < mSingleWidgets.count(); i++) {
        mSingleWidgets.at(i)->hide();
    }
}

void MinimalScrollWidgetVisiblePart::updateParentHeight() {
    mParentWidget->updateHeight();
}