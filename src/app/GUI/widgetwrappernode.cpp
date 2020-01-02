// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "widgetwrappernode.h"

StackWrapperMenu::StackWrapperMenu() {
    setStyleSheet("QMenuBar#cornerMenuBar::item { padding-top: 1px; margin-bottom: 2px; }");
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

WidgetWrapperNode::WidgetWrapperNode(const WrapperNode::WrapperNodeCreator &creator) :
    WrapperNode(WrapperNodeType::widget, creator) {
    mCornerMenu = new WidgetWrapperCornerMenu(this);
    mLayout = new QVBoxLayout(this);
    setLayout(mLayout);
    mLayout->setSpacing(0);
    mLayout->setMargin(0);
}

void WidgetWrapperNode::setMenuBar(StackWrapperMenu * const menu) {
    if(mMenuBar) delete mMenuBar;
    mMenuBar = menu;
    mMenuBar->setCornerWidget(mCornerMenu);
    mLayout->insertWidget(0, mMenuBar);
}

void WidgetWrapperNode::setCentralWidget(QWidget * const widget) {
    if(mCenterWidget) delete mCenterWidget;
    mCenterWidget = widget;
    mLayout->insertWidget(-1, mCenterWidget);
    mCenterWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                 QSizePolicy::MinimumExpanding);
}

void WidgetWrapperNode::close() {
    if(fParent) fParent->closeChild(this);
}

void WidgetWrapperNode::splitV() {
    Q_ASSERT(fParent);
    const auto vStack = new VWidgetStackNode(fCreator);
    vStack->fChild1 = this;
    vStack->fChild2 = fCreator(nullptr);

    fParent->replaceChild(this, vStack);

    vStack->appendWidget(vStack->fChild1->widget());
    vStack->appendWidget(vStack->fChild2->widget());

    vStack->fChild1->fParent = vStack;
    vStack->fChild2->fParent = vStack;
}

void WidgetWrapperNode::splitH() {
    Q_ASSERT(fParent);
    const auto hStack = new HWidgetStackNode(fCreator);
    hStack->fChild1 = this;
    hStack->fChild2 = fCreator(nullptr);

    fParent->replaceChild(this, hStack);

    hStack->appendWidget(hStack->fChild1->widget());
    hStack->appendWidget(hStack->fChild2->widget());

    hStack->fChild1->fParent = hStack;
    hStack->fChild2->fParent = hStack;
}
