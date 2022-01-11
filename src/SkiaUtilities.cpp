#include "SkiaUtilities.h"

#include "ignore_warnings_on_external_includes.h"
#include <SkData.h>
#include <SkImage.h>
#include <SkBitmap.h>
#include <SkTypeface.h>
#include <SkCanvas.h>
#include "restore_internal_warnings.h"

#include "Logging.h"

OsmAnd::SkiaUtilities::SkiaUtilities()
{
}

OsmAnd::SkiaUtilities::~SkiaUtilities()
{
}

sk_sp<SkImage> OsmAnd::SkiaUtilities::createImageFromFile(const QFileInfo& fileInfo)
{
    return SkImage::MakeFromEncoded(SkData::MakeFromFileName(qPrintable(fileInfo.absoluteFilePath())));
}

sk_sp<SkImage> OsmAnd::SkiaUtilities::createImageFromData(const QByteArray& data)
{
    return SkImage::MakeFromEncoded(SkData::MakeWithProc(
        data.constData(),
        data.length(),
        [](const void* ptr, void* context) { delete reinterpret_cast<QByteArray*>(context); },
        new QByteArray(data)
    ));
}

sk_sp<SkImage> OsmAnd::SkiaUtilities::scaleImage(
    const sk_sp<const SkImage>& original,
    float xScale,
    float yScale)
{
    if (!original || original->width() <= 0 || original->height() <= 0)
        return nullptr;

    const auto scaledWidth = qCeil(original->width() * xScale);
    const auto scaledHeight = qCeil(original->height() * yScale);

    if (scaledWidth <= 0 || scaledHeight <= 0)
        return nullptr;

    SkBitmap target;
    if (!target.tryAllocPixels(original->imageInfo().makeWH(scaledWidth, scaledHeight)))
        return nullptr;
    
    if (!original->scalePixels(target.pixmap(), {}))
        return nullptr;

    return target.asImage();
}

sk_sp<SkImage> OsmAnd::SkiaUtilities::offsetImage(
    const sk_sp<const SkImage>& original,
    float xOffset,
    float yOffset)
{
    if (!original || original->width() <= 0 || original->height() <= 0)
        return nullptr;

    const auto newWidth = original->width() + qAbs(xOffset);
    const auto newHeight = original->height() + qAbs(yOffset);

    SkBitmap target;
    if (!target.tryAllocPixels(original->imageInfo().makeWH(newWidth, newHeight)))
        return nullptr;
    target.eraseColor(SK_ColorTRANSPARENT);

    SkCanvas canvas(target);
    canvas.drawImage(original.get(), xOffset > 0.0f ? xOffset : 0, yOffset > 0.0f ? yOffset : 0);
    canvas.flush();

    return target.asImage();
}

sk_sp<SkImage> OsmAnd::SkiaUtilities::createTileImage(
    const sk_sp<const SkImage>& first,
    const sk_sp<const SkImage>& second,
    float yOffset)
{
    SkImageInfo imageInfo;
    if (first && first->width() > 0 && first->height() > 0)
        imageInfo = first->imageInfo();

    if (second && second->width() > 0 && second->height() > 0)
        imageInfo = second->imageInfo();

    if (imageInfo.isEmpty())
        return nullptr;

    SkBitmap target;
    if (!target.tryAllocPixels(imageInfo))
        return nullptr;
    target.eraseColor(SK_ColorTRANSPARENT);

    SkCanvas canvas(target);
    if (first)
        canvas.drawImage(first.get(), 0, -yOffset);
    if (second)
        canvas.drawImage(second.get(), 0, -yOffset + imageInfo.height());
    canvas.flush();

    return target.asImage();
}

sk_sp<SkTypeface> OsmAnd::SkiaUtilities::createTypefaceFromData(const QByteArray& data)
{
    return SkTypeface::MakeFromData(SkData::MakeWithCopy(data.constData(), data.length()));
}

sk_sp<SkImage> OsmAnd::SkiaUtilities::mergeImages(const QList< sk_sp<const SkImage> >& images)
{
    if (images.isEmpty())
        return nullptr;

    int maxWidth = 0;
    int maxHeight = 0;
    for (const auto& image : constOf(images))
    {
        maxWidth = qMax(maxWidth, image->width());
        maxHeight = qMax(maxHeight, image->height());
    }

    if (maxWidth <= 0 || maxHeight <= 0)
        return nullptr;

    SkBitmap target;
    if (!target.tryAllocPixels(images.first()->imageInfo().makeWH(maxWidth, maxHeight)))
        return nullptr;
    target.eraseColor(SK_ColorTRANSPARENT);

    SkCanvas canvas(target);
    for (const auto& image : constOf(images))
    {
        canvas.drawImage(image.get(),
            (maxWidth - image->width()) / 2.0f,
            (maxHeight - image->height()) / 2.0f
        );
    }
    canvas.flush();

    return target.asImage();
}
