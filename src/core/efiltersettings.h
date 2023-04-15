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

#ifndef EFILTERSETTINGS_H
#define EFILTERSETTINGS_H






#include <QObject>

#include "core_global.h"
#include "skia/skiaincludes.h"
#include "skia/skcompatfilterquality.h"
#include "simplemath.h"




class CORE_EXPORT eFilterSettings : public QObject {
    Q_OBJECT
public:
    eFilterSettings();

    static eFilterSettings* sInstance;

    static void sSetEnveRenderFilter(const CompatSkFilterQuality filter) {
        sInstance->setEnveRenderFilter(filter);
    }

    static void sSetOutputRenderFilter(const CompatSkFilterQuality filter) {
        sInstance->setOutputRenderFilter(filter);
    }

    static void sSetDisplayFilter(const CompatSkFilterQuality filter) {
        sSetSmartDisplay(false);
        sInstance->mDisplayFilter = filter;
    }

    static void sSetSmartDisplay(const bool smart) {
        sInstance->mSmartDisplay = smart;
    }

    static CompatSkFilterQuality sRender() {
        return sInstance->mRender;
    }

    static CompatSkFilterQuality sDisplay() {
        return sInstance->mDisplayFilter;
    }

    static bool sSmartDisplat() {
        return sInstance->mSmartDisplay;
    }

    static CompatSkFilterQuality sDisplay(const qreal zoom,
                                    const qreal resolution) {
        if(sInstance->mSmartDisplay) {
            const qreal scale = zoom/resolution;
            if(isOne4Dec(scale)) return CompatSkFilterQuality::SK_NONE;
            else if(scale > 2.5) return CompatSkFilterQuality::SK_NONE;
            else if(scale < 0.5) return CompatSkFilterQuality::SK_MEDIUM;
            return CompatSkFilterQuality::SK_LOW;
        } else return sDisplay();
    }

    static void sSwitchToEnveRender() {
        sInstance->mCurrentRender = RenderFilter::enve;
        sInstance->updateRenderFilter();
    }

    static void sSwitchToOutputRender() {
        sInstance->mCurrentRender = RenderFilter::output;
        sInstance->updateRenderFilter();
    }

    void setEnveRenderFilter(const CompatSkFilterQuality filter);
    void setOutputRenderFilter(const CompatSkFilterQuality filter);
signals:
    void renderFilterChanged(const CompatSkFilterQuality filter);
private:
    void updateRenderFilter() {
        CompatSkFilterQuality newFilter;
        if(mCurrentRender == RenderFilter::enve) newFilter = mEnveRender;
        else newFilter = mOutputRender;
        if(newFilter == mRender) return;
        mRender = newFilter;
        emit renderFilterChanged(newFilter);
    }

    CompatSkFilterQuality mEnveRender = CompatSkFilterQuality::SK_HIGH;
    CompatSkFilterQuality mOutputRender = CompatSkFilterQuality::SK_HIGH;

    enum class RenderFilter { enve, output };
    RenderFilter mCurrentRender = RenderFilter::enve;
    CompatSkFilterQuality mRender = mEnveRender;

    bool mSmartDisplay = true;
    CompatSkFilterQuality mDisplayFilter = CompatSkFilterQuality::SK_HIGH;
};

#endif // EFILTERSETTINGS_H
