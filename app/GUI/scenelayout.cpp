#include "scenelayout.h"
#include "document.h"

SceneLayout::SceneLayout(QWidget * const parent) :
    QWidget(parent) {
    setLayout(new QHBoxLayout);
    layout()->setMargin(0);
    layout()->setSpacing(0);

    reset();
}

SceneBaseStackItem* SceneLayout::extract() {
    const auto tmp = mBaseStack;
    mBaseStack = nullptr;
    return tmp;
}

void SceneLayout::reset() {
    mBaseStack = nullptr;
    clearWidget();
}

void SceneLayout::setCurrent(SceneBaseStackItem* const item) {
    if(!item) return reset();
    clearWidget();
    item->create(this, layout());
    mBaseStack = item;
}

void SceneLayout::saveData() {
    mBaseStack->saveData();
}

void SceneLayout::clearWidget() {
    while(layout()->count() > 0) {
        const auto item = layout()->takeAt(0);
        delete item->widget();
        delete item->layout();
        delete item;
    }
}
