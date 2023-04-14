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


/**
 *  Controls how much filtering to be done when scaling/transforming complex colors
 *  e.g. images.
    These values are persisted to logs. Entries should not be renumbered and
    numeric values should never be reused.
    LONG SINCE REMOVED, use this for SkSamplingOptions presets


enum SkFilterQuality {
    kNone_SkFilterQuality   = 0,    //!< nearest-neighbor; fastest but lowest quality
    kLow_SkFilterQuality    = 1,    //!< bilerp
    kMedium_SkFilterQuality = 2,    //!< bilerp + mipmaps; good for down-scaling
    kHigh_SkFilterQuality   = 3,    //!< bicubic resampling; slowest but good quality
    kLast_SkFilterQuality = kHigh_SkFilterQuality,
};
 */

#include <QObject>

#include "core_global.h"
#include "skia/skiaincludes.h"
#include "simplemath.h"

class CORE_EXPORT eFilterSettings : public QObject {
    Q_OBJECT
public:
    eFilterSettings();

    static eFilterSettings* sInstance;

    static void sSetEnveRenderFilter(const SkFilterQuality filter) {
        sInstance->setEnveRenderFilter(filter);
    }

    static void sSetOutputRenderFilter(const SkFilterQuality filter) {
        sInstance->setOutputRenderFilter(filter);
    }

    static void sSetDisplayFilter(const SkFilterQuality filter) {
        sSetSmartDisplay(false);
        sInstance->mDisplayFilter = filter;
    }

    static void sSetSmartDisplay(const bool smart) {
        sInstance->mSmartDisplay = smart;
    }

    static SkFilterQuality sRender() {
        return sInstance->mRender;
    }

    static SkFilterQuality sDisplay() {
        return sInstance->mDisplayFilter;
    }

    static bool sSmartDisplat() {
        return sInstance->mSmartDisplay;
    }

    static SkFilterQuality sDisplay(const qreal zoom,
                                    const qreal resolution) {
        if(sInstance->mSmartDisplay) {
            const qreal scale = zoom/resolution;
            if(isOne4Dec(scale)) return kNone_SkFilterQuality;
            else if(scale > 2.5) return kNone_SkFilterQuality;
            else if(scale < 0.5) return kMedium_SkFilterQuality;
            return kLow_SkFilterQuality;
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

    void setEnveRenderFilter(const SkFilterQuality filter);
    void setOutputRenderFilter(const SkFilterQuality filter);
signals:
    void renderFilterChanged(const SkFilterQuality filter);
private:
    void updateRenderFilter() {
        SkFilterQuality newFilter;
        if(mCurrentRender == RenderFilter::enve) newFilter = mEnveRender;
        else newFilter = mOutputRender;
        if(newFilter == mRender) return;
        mRender = newFilter;
        emit renderFilterChanged(newFilter);
    }

    SkFilterQuality mEnveRender = SkFilterQuality::kHigh_SkFilterQuality;
    SkFilterQuality mOutputRender = SkFilterQuality::kHigh_SkFilterQuality;

    enum class RenderFilter { enve, output };
    RenderFilter mCurrentRender = RenderFilter::enve;
    SkFilterQuality mRender = mEnveRender;

    bool mSmartDisplay = true;
    SkFilterQuality mDisplayFilter = SkFilterQuality::kNone_SkFilterQuality;
};

#endif // EFILTERSETTINGS_H
