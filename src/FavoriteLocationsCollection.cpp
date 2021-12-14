#include "FavoriteLocationsCollection.h"
#include "FavoriteLocationsCollection_P.h"

#include "IFavoriteLocation.h"

OsmAnd::FavoriteLocationsCollection::FavoriteLocationsCollection()
	: FavoriteLocationsCollection(new FavoriteLocationsCollection_P(this))
{
}

OsmAnd::FavoriteLocationsCollection::FavoriteLocationsCollection(FavoriteLocationsCollection_P* const p)
	: _p(p)
{
}

OsmAnd::FavoriteLocationsCollection::~FavoriteLocationsCollection()
{
	_p->_containerLink->release();
}

std::shared_ptr<OsmAnd::IFavoriteLocation> OsmAnd::FavoriteLocationsCollection::createFavoriteLocation(
    const PointI position31,
    const QString& elevation /*= QString::null*/,
    const QString& time /*= QString::null*/,
    const QString& creationTime /*= QString::null*/,
    const QString& title /*= QString::null*/,
    const QString& description /*= QString::null*/,
    const QString& address /*= QString::null*/,
    const QString& group /*= QString::null*/,
    const QString& icon /*= QString::null*/,
    const QString& background /*= QString::null*/,
    const ColorRGB color /*= ColorRGB()*/,
    const bool calendarEvent /*= false*/)
{
    return _p->createFavoriteLocation(position31, elevation, time, creationTime, title, description, address, group, icon, background, color, calendarEvent);
}

std::shared_ptr<OsmAnd::IFavoriteLocation> OsmAnd::FavoriteLocationsCollection::createFavoriteLocation(
    const LatLon latLon,
    const QString& elevation /*= QString::null*/,
    const QString& time /*= QString::null*/,
    const QString& creationTime /*= QString::null*/,
    const QString& title /*= QString::null*/,
    const QString& description /*= QString::null*/,
    const QString& address /*= QString::null*/,
    const QString& group /*= QString::null*/,
    const QString& icon /*= QString::null*/,
    const QString& background /*= QString::null*/,
    const ColorRGB color /*= ColorRGB()*/,
    const bool calendarEvent /*= false*/)
{
    return _p->createFavoriteLocation(latLon, elevation, time, creationTime, title, description, address, group, icon, background, color, calendarEvent);
}

std::shared_ptr<OsmAnd::IFavoriteLocation> OsmAnd::FavoriteLocationsCollection::copyFavoriteLocation(const std::shared_ptr<const IFavoriteLocation>& other)
{
    if (other->getLocationSource() == IFavoriteLocation::LocationSource::Point31)
    {
        return _p->createFavoriteLocation(
            other->getPosition31(),
            other->getElevation(),
            other->getTime(),
            other->getCreationTime(),
            other->getTitle(),
            other->getDescription(),
            other->getAddress(),
            other->getGroup(),
            other->getIcon(),
            other->getBackground(),
            other->getColor(),
            other->getCalendarEvent());
    }
    else //if (other->getLocationSource() == IFavoriteLocation::LocationSource::LatLon)
    {
        return _p->createFavoriteLocation(
            other->getLatLon(),
            other->getElevation(),
            other->getTime(),
            other->getCreationTime(),
            other->getTitle(),
            other->getDescription(),
            other->getAddress(),
            other->getGroup(),
            other->getIcon(),
            other->getBackground(),
            other->getColor(),
            other->getCalendarEvent());
    }
}

bool OsmAnd::FavoriteLocationsCollection::removeFavoriteLocation(const std::shared_ptr<IFavoriteLocation>& favoriteLocation)
{
    return _p->removeFavoriteLocation(favoriteLocation);
}

bool OsmAnd::FavoriteLocationsCollection::removeFavoriteLocations(const QList< std::shared_ptr<IFavoriteLocation> >& favoriteLocations)
{
    return _p->removeFavoriteLocations(favoriteLocations);
}

void OsmAnd::FavoriteLocationsCollection::clearFavoriteLocations()
{
    _p->clearFavoriteLocations();
}

unsigned int OsmAnd::FavoriteLocationsCollection::getFavoriteLocationsCount() const
{
    return _p->getFavoriteLocationsCount();
}

QList< std::shared_ptr<OsmAnd::IFavoriteLocation> > OsmAnd::FavoriteLocationsCollection::getFavoriteLocations() const
{
    return _p->getFavoriteLocations();
}

QSet<QString> OsmAnd::FavoriteLocationsCollection::getGroups() const
{
    return _p->getGroups();
}

void OsmAnd::FavoriteLocationsCollection::copyFrom(const std::shared_ptr<const IFavoriteLocationsCollection>& otherCollection)
{
    _p->copyFrom(otherCollection->getFavoriteLocations());
}

void OsmAnd::FavoriteLocationsCollection::copyFrom(const QList< std::shared_ptr<IFavoriteLocation> >& otherCollection)
{
    _p->copyFrom(otherCollection);
}

void OsmAnd::FavoriteLocationsCollection::copyFrom(const QList< std::shared_ptr<const IFavoriteLocation> >& otherCollection)
{
    _p->copyFrom(otherCollection);
}

void OsmAnd::FavoriteLocationsCollection::mergeFrom(const std::shared_ptr<const IFavoriteLocationsCollection>& otherCollection)
{
    _p->mergeFrom(otherCollection->getFavoriteLocations());
}

void OsmAnd::FavoriteLocationsCollection::mergeFrom(const QList< std::shared_ptr<IFavoriteLocation> >& otherCollection)
{
    _p->mergeFrom(otherCollection);
}

void OsmAnd::FavoriteLocationsCollection::mergeFrom(const QList< std::shared_ptr<const IFavoriteLocation> >& otherCollection)
{
    _p->mergeFrom(otherCollection);
}
