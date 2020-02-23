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

#include "expressionsinglechild.h"
#include "expressionplainvalue.h"

ExpressionSingleChild::ExpressionSingleChild(
        const QString &name, const sptr& value) :
    ExpressionValue(false), mName(name), mValue(value) {
    connect(mValue.get(), &ExpressionValue::currentValueChanged,
            this, &ExpressionValue::updateValue);
    connect(mValue.get(), &ExpressionValue::relRangeChanged,
            this, &ExpressionValue::relRangeChanged);
}

void ExpressionSingleChild::collapse() {
    mValue->collapse();
    if(mValue->isPlainValue()) {
        mValue = ExpressionPlainValue::sCreate(mValue->currentValue());
    }
}

bool ExpressionSingleChild::isPlainValue() const
{ return mValue->isPlainValue(); }

bool ExpressionSingleChild::setRelFrame(const qreal relFrame) {
    if(!setRelFrameValue(relFrame)) return false;
    return mValue->setRelFrame(relFrame);
}