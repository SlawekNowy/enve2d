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

#ifndef BOXSCROLLWIDGET_H
#define BOXSCROLLWIDGET_H

#include "OptimalScrollArea/scrollwidget.h"
#include "GUI/keyfocustarget.h"
class BoxScroller;
class ScrollArea;
class WindowSingleWidgetTarget;
class Document;
class Canvas;

class BoxScrollWidget : public ScrollWidget {
    Q_OBJECT
public:
    explicit BoxScrollWidget(Document& document,
                             ScrollArea * const parent = nullptr);
    ~BoxScrollWidget();
    BoxScroller *getVisiblePartWidget();
    void setCurrentScene(Canvas* const scene);
    //    void updateAbstraction();
protected:
    stdptr<SWT_Abstraction> mCoreAbs;
    void createVisiblePartWidget();
};

#endif // BOXSCROLLWIDGET_H
