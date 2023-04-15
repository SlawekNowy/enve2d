#ifndef SKCOMPATFILTERQUALITY_H
#define SKCOMPATFILTERQUALITY_H


/*
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

#include "core_global.h"
#include "skia/skiaincludes.h"
enum CompatSkFilterQuality {
    SK_NONE =0,
    SK_LOW = 1,
    SK_MEDIUM = 2,
    SK_HIGH = 3
};

//TODO: Singleton?
namespace SkiaHelpers
{

    CORE_EXPORT extern SkSamplingOptions SkFQtoSamplingOpts(CompatSkFilterQuality quality);


};

#endif // SKCOMPATFILTERQUALITY_H
