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

#ifndef TRANSFORMEFFECTCOLLECTION_H
#define TRANSFORMEFFECTCOLLECTION_H

#include "transformeffect.h"
#include "Animators/dynamiccomplexanimator.h"

CORE_EXPORT
qsptr<TransformEffect> readIdCreateTransformEffect(eReadStream& src);
CORE_EXPORT
void writeTransformEffectType(TransformEffect* const obj, eWriteStream& dst);

CORE_EXPORT
qsptr<TransformEffect> readIdCreateTransformEffectXEV(const QDomElement& ele);
CORE_EXPORT
void writeTransformEffectTypeXEV(TransformEffect* const obj, QDomElement& ele);

typedef DynamicComplexAnimator<
    TransformEffect,
    writeTransformEffectType,
    readIdCreateTransformEffect,
    writeTransformEffectTypeXEV,
    readIdCreateTransformEffectXEV> TransformEffectCollectionBase;


class CORE_EXPORT TransformEffectCollection :
        public TransformEffectCollectionBase {
    e_OBJECT
protected:
    TransformEffectCollection();
public:
    void prp_setupTreeViewMenu(PropertyMenu * const menu);

    void prp_readProperty(eReadStream &src);

    bool hasEffects();
    void readTransformEffect(eReadStream &target);

    void applyEffects(const qreal relFrame,
                      qreal& pivotX, qreal& pivotY,
                      qreal& posX, qreal& posY,
                      qreal& rot,
                      qreal& scaleX, qreal& scaleY,
                      qreal& shearX, qreal& shearY,
                      BoundingBox* const parent) const;
};

#endif // TRANSFORMEFFECTCOLLECTION_H
