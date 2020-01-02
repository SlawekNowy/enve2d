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

#ifndef BOXESLISTACTIONBUTTON_H
#define BOXESLISTACTIONBUTTON_H

#include <QWidget>

class BoxesListActionButton : public QWidget {
    Q_OBJECT
public:
    BoxesListActionButton(QWidget * const parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    bool mHover = false;
signals:
    void pressed();
};
#include <functional>
class PixmapActionButton : public BoxesListActionButton {
public:
    PixmapActionButton(QWidget * const parent = nullptr) :
        BoxesListActionButton(parent) {}

    void setPixmapChooser(const std::function<QPixmap*()>& func) {
        mPixmapChooser = func;
    }
protected:
    void paintEvent(QPaintEvent*);
private:
    std::function<QPixmap*()> mPixmapChooser;
};


#endif // BOXESLISTACTIONBUTTON_H
