#include "ICoreResourcesProvider.h"

#include "ignore_warnings_on_external_includes.h"
#include <SkData.h>
#include <SkImage.h>
#include "restore_internal_warnings.h"

#include "SkiaUtilities.h"

OsmAnd::ICoreResourcesProvider::ICoreResourcesProvider()
{
}

OsmAnd::ICoreResourcesProvider::~ICoreResourcesProvider()
{
}

sk_sp<SkImage> OsmAnd::ICoreResourcesProvider::getResourceAsImage(
    const QString& name,
    const float displayDensityFactor) const
{
    bool ok = false;
    const auto data = getResource(name, displayDensityFactor, &ok);
    if (!ok)
        return nullptr;

    return SkiaUtilities::createImageFromData(data);
}

sk_sp<SkImage> OsmAnd::ICoreResourcesProvider::getResourceAsImage(const QString& name) const
{
    bool ok = false;
    const auto data = getResource(name, &ok);
    if (!ok)
        return nullptr;
    
    return SkiaUtilities::createImageFromData(data);
}
