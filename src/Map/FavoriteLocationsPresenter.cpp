#include "FavoriteLocationsPresenter.h"
#include "FavoriteLocationsPresenter_P.h"

#include "EmbeddedResources.h"

OsmAnd::FavoriteLocationsPresenter::FavoriteLocationsPresenter(
    const std::shared_ptr<const IFavoriteLocationsCollection>& collection_,
    const std::shared_ptr<const SkBitmap>& favoriteLocationPinIconBitmap_ /*= nullptr*/)
    : _p(new FavoriteLocationsPresenter_P(this))
    , collection(collection_)
    , favoriteLocationPinIconBitmap(favoriteLocationPinIconBitmap_)
{
    _p->subscribeToChanges();
    _p->syncFavoriteLocationMarkers();
}

OsmAnd::FavoriteLocationsPresenter::~FavoriteLocationsPresenter()
{
    _p->unsubscribeToChanges();
}

std::shared_ptr<const SkBitmap> OsmAnd::FavoriteLocationsPresenter::getDefaultFavoriteLocationPinIconBitmap()
{
    static const std::shared_ptr<const SkBitmap> defaultFavoriteLocationPinIconBitmap(
        EmbeddedResources::getBitmapResource(QLatin1String("map/stubs/favorite_location_pin_icon.png")));
    return defaultFavoriteLocationPinIconBitmap;
}

bool OsmAnd::FavoriteLocationsPresenter::isGroupVisible(const QString& group) const
{
    return _p->isGroupVisible(group);
}

void OsmAnd::FavoriteLocationsPresenter::setIsGroupVisible(const QString& group, const bool isVisible)
{
    _p->setIsGroupVisible(group, isVisible);
}

void OsmAnd::FavoriteLocationsPresenter::showGroup(const QString& group)
{
    _p->showGroup(group);
}

void OsmAnd::FavoriteLocationsPresenter::hideGroup(const QString& group)
{
    _p->hideGroup(group);
}

bool OsmAnd::FavoriteLocationsPresenter::isFavoriteLocationVisible(const std::shared_ptr<const IFavoriteLocation>& favoriteLocation, const bool checkGroup /*= true*/) const
{
    return _p->isFavoriteLocationVisible(favoriteLocation, checkGroup);
}

void OsmAnd::FavoriteLocationsPresenter::setIsFavoriteLocationVisible(const std::shared_ptr<const IFavoriteLocation>& favoriteLocation, const bool isVisible)
{
    _p->setIsFavoriteLocationVisible(favoriteLocation, isVisible);
}

void OsmAnd::FavoriteLocationsPresenter::showFavoriteLocation(const std::shared_ptr<const IFavoriteLocation>& favoriteLocation)
{
    _p->showFavoriteLocation(favoriteLocation);
}

void OsmAnd::FavoriteLocationsPresenter::hideFavoriteLocation(const std::shared_ptr<const IFavoriteLocation>& favoriteLocation)
{
    _p->hideFavoriteLocation(favoriteLocation);
}

QList<OsmAnd::IMapKeyedSymbolsProvider::Key> OsmAnd::FavoriteLocationsPresenter::getProvidedDataKeys() const
{
    return _p->getProvidedDataKeys();
}

bool OsmAnd::FavoriteLocationsPresenter::obtainData(const Key key, std::shared_ptr<MapKeyedData>& outKeyedData, const IQueryController* const queryController /*= nullptr*/)
{
    return _p->obtainData(key, outKeyedData, queryController);
}