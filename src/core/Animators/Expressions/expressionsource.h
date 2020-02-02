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

#ifndef EXPRESSIONSOURCE_H
#define EXPRESSIONSOURCE_H
#include "expressionsourcebase.h"

class ExpressionSource : public ExpressionSourceBase {
public:
    ExpressionSource(QrealAnimator* const parent);

    static sptr sCreate(const QString& path, QrealAnimator* const parent);

    qreal calculateValue(const qreal relFrame) const override;
    FrameRange identicalRange(const qreal relFrame) const override;
    QString toString() const override
    { return mPath; }

    bool dependsOn(QrealAnimator* const source) const override;

    void setPath(const QString& path);
private:
    void lookForSource();
    void updateSourcePath();

    QString mPath;
};

#endif // EXPRESSIONSOURCE_H
