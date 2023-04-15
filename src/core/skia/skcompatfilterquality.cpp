#include "skcompatfilterquality.h"


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


SkSamplingOptions SkiaHelpers::SkFQtoSamplingOpts(CompatSkFilterQuality quality)
{
    //SkSamplingOptions outOpts{};
    switch (quality)
    {
    case SK_NONE:
    {
        return SkSamplingOptions(SkFilterMode::kNearest,SkMipmapMode::kNone);
    }
    case SK_LOW:
    {
        return SkSamplingOptions(SkFilterMode::kLinear,SkMipmapMode::kNone);
    }
    case SK_MEDIUM:
    {
        return SkSamplingOptions(SkFilterMode::kLinear,SkMipmapMode::kLinear);
    }
    case SK_HIGH:
    {
        return SkSamplingOptions(SkCubicResampler::Mitchell());
    }
    }
}
