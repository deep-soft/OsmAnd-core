#ifndef _OSMAND_CORE_ATLAS_MAP_RENDERER_SYMBOLS_STAGE_H_
#define _OSMAND_CORE_ATLAS_MAP_RENDERER_SYMBOLS_STAGE_H_

#include "stdlib_common.h"

#include "QtExtensions.h"
#include "ignore_warnings_on_external_includes.h"
#include <QReadWriteLock>
#include "restore_internal_warnings.h"

#include "ignore_warnings_on_external_includes.h"
#include <glm/glm.hpp>
#include "restore_internal_warnings.h"

#include "OsmAndCore.h"
#include "CommonTypes.h"
#include "QuadTree.h"
#include "AtlasMapRendererStage.h"
#include "GPUAPI.h"
#include "VectorMapSymbol.h"

namespace OsmAnd
{
    class MapSymbol;
    class RasterMapSymbol;
    class OnPathRasterMapSymbol;
    class IOnSurfaceMapSymbol;
    class IBillboardMapSymbol;

    class AtlasMapRendererSymbolsStage : public AtlasMapRendererStage
    {
    public:
        struct RenderableSymbol;
        typedef QuadTree< std::shared_ptr<const RenderableSymbol>, AreaI::CoordType > ScreenQuadTree;

        struct RenderableSymbol
        {
            virtual ~RenderableSymbol();

            std::shared_ptr<const MapSymbolsGroup> mapSymbolGroup;
            std::shared_ptr<const MapSymbol> mapSymbol;
            std::shared_ptr<const MapSymbolsGroup::AdditionalSymbolInstanceParameters> genericInstanceParameters;

            std::shared_ptr<const GPUAPI::ResourceInGPU> gpuResource;
            double distanceToCamera;
            ScreenQuadTree::BBox visibleBBox;
            ScreenQuadTree::BBox intersectionBBox;
        };

        struct RenderableBillboardSymbol : RenderableSymbol
        {
            virtual ~RenderableBillboardSymbol();

            std::shared_ptr<const MapSymbolsGroup::AdditionalBillboardSymbolInstanceParameters> instanceParameters;

            PointI offsetFromTarget31;
            PointF offsetFromTarget;
            glm::vec3 positionInWorld;
        };

        struct RenderableOnSurfaceSymbol : RenderableSymbol
        {
            virtual ~RenderableOnSurfaceSymbol();

            std::shared_ptr<const MapSymbolsGroup::AdditionalOnSurfaceSymbolInstanceParameters> instanceParameters;

            PointI offsetFromTarget31;
            PointF offsetFromTarget;
            glm::vec3 positionInWorld;
            std::shared_ptr<std::map<TileId, std::vector<VectorMapSymbol::Vertex>>> tiledMeshes;

            float direction;
        };

        struct RenderableOnPathSymbol : RenderableSymbol
        {
            virtual ~RenderableOnPathSymbol();

            std::shared_ptr<const MapSymbolsGroup::AdditionalOnPathSymbolInstanceParameters> instanceParameters;

            bool is2D;
            glm::vec2 directionInWorld;
            glm::vec2 directionOnScreen;

            struct GlyphPlacement
            {
                inline GlyphPlacement()
                    : width(qSNaN())
                    , angle(qSNaN())
                {
                }

                inline GlyphPlacement(
                    const glm::vec2& anchorPoint_,
                    const float width_,
                    const float angle_,
                    const glm::vec2& vNormal_)
                    : anchorPoint(anchorPoint_)
                    , width(width_)
                    , angle(angle_)
                    , vNormal(vNormal_)
                {
                }

                glm::vec2 anchorPoint;
                float width;
                float angle;
                glm::vec2 vNormal;
            };
            QVector< GlyphPlacement > glyphsPlacement;
        };
    private:
        bool obtainRenderableSymbols(
            QList< std::shared_ptr<const RenderableSymbol> >& outRenderableSymbols,
            ScreenQuadTree& outIntersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool obtainRenderableSymbols(
            const MapRenderer::PublishedMapSymbolsByOrder& mapSymbolsByOrder,
            QList< std::shared_ptr<const RenderableSymbol> >& outRenderableSymbols,
            ScreenQuadTree& outIntersections,
            MapRenderer::PublishedMapSymbolsByOrder* pOutAcceptedMapSymbolsByOrder,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        mutable MapRenderer::PublishedMapSymbolsByOrder _lastAcceptedMapSymbolsByOrder;

        mutable QReadWriteLock _lastPreparedIntersectionsLock;
        ScreenQuadTree _lastPreparedIntersections;

        mutable QReadWriteLock _lastVisibleSymbolsLock;
        ScreenQuadTree _lastVisibleSymbols;

        // Path calculations cache
        struct ComputedPathData
        {
            QVector<glm::vec2> pathInWorld;
            QVector<float> pathSegmentsLengthsInWorld;
            QVector<glm::vec2> pathOnScreen;
            QVector<float> pathSegmentsLengthsOnScreen;
        };
        typedef QHash< std::shared_ptr< const QVector<PointI> >, ComputedPathData > ComputedPathsDataCache;

        void obtainRenderablesFromSymbol(
            const std::shared_ptr<const MapSymbolsGroup>& mapSymbolGroup,
            const std::shared_ptr<const MapSymbol>& mapSymbol,
            const std::shared_ptr<const MapSymbolsGroup::AdditionalSymbolInstanceParameters>& instanceParameters,
            const MapRenderer::MapSymbolReferenceOrigins& referenceOrigins,
            ComputedPathsDataCache& computedPathsDataCache,
            QList< std::shared_ptr<RenderableSymbol> >& outRenderableSymbols,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;

        bool plotSymbol(
            const std::shared_ptr<RenderableSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;

        // Billboard symbols:
        void obtainRenderablesFromBillboardSymbol(
            const std::shared_ptr<const MapSymbolsGroup>& mapSymbolGroup,
            const std::shared_ptr<const IBillboardMapSymbol>& billboardMapSymbol,
            const std::shared_ptr<const MapSymbolsGroup::AdditionalBillboardSymbolInstanceParameters>& instanceParameters,
            const MapRenderer::MapSymbolReferenceOrigins& referenceOrigins,
            QList< std::shared_ptr<RenderableSymbol> >& outRenderableSymbols,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotBillboardSymbol(
            const std::shared_ptr<RenderableBillboardSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotBillboardRasterSymbol(
            const std::shared_ptr<RenderableBillboardSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotBillboardVectorSymbol(
            const std::shared_ptr<RenderableBillboardSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;

        // On-surface symbols:
        void obtainRenderablesFromOnSurfaceSymbol(
            const std::shared_ptr<const MapSymbolsGroup>& mapSymbolGroup,
            const std::shared_ptr<const IOnSurfaceMapSymbol>& onSurfaceMapSymbol,
            const std::shared_ptr<const MapSymbolsGroup::AdditionalOnSurfaceSymbolInstanceParameters>& instanceParameters,
            const MapRenderer::MapSymbolReferenceOrigins& referenceOrigins,
            QList< std::shared_ptr<RenderableSymbol> >& outRenderableSymbols,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotOnSurfaceSymbol(
            const std::shared_ptr<RenderableOnSurfaceSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotOnSurfaceRasterSymbol(
            const std::shared_ptr<RenderableOnSurfaceSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotOnSurfaceVectorSymbol(
            const std::shared_ptr<RenderableOnSurfaceSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;

        // On-path symbols:
        void obtainRenderablesFromOnPathSymbol(
            const std::shared_ptr<const MapSymbolsGroup>& mapSymbolGroup,
            const std::shared_ptr<const OnPathRasterMapSymbol>& onPathMapSymbol,
            const std::shared_ptr<const MapSymbolsGroup::AdditionalOnPathSymbolInstanceParameters>& instanceParameters,
            const MapRenderer::MapSymbolReferenceOrigins& referenceOrigins,
            ComputedPathsDataCache& computedPathsDataCache,
            QList< std::shared_ptr<RenderableSymbol> >& outRenderableSymbols,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool plotOnPathSymbol(
            const std::shared_ptr<RenderableOnPathSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;

        // Intersection-related:
        bool applyVisibilityFiltering(
            const ScreenQuadTree::BBox& visibleBBox,
            const ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool applyIntersectionWithOtherSymbolsFiltering(
            const std::shared_ptr<const RenderableSymbol>& renderable,
            const ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool applyMinDistanceToSameContentFromOtherSymbolFiltering(
            const std::shared_ptr<const RenderableSymbol>& renderable,
            const ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;
        bool addToIntersections(
            const std::shared_ptr<const RenderableSymbol>& renderable,
            ScreenQuadTree& intersections,
            AtlasMapRenderer_Metrics::Metric_renderFrame* const metric) const;

        // Utilities:
        QVector<glm::vec2> convertPoints31ToWorld(
            const QVector<PointI>& points31) const;
        QVector<glm::vec2> convertPoints31ToWorld(
            const QVector<PointI>& points31,
            const unsigned int startIndex,
            const unsigned int endIndex) const;

        QVector<glm::vec2> projectFromWorldToScreen(
            const QVector<glm::vec2>& pointsInWorld) const;
        QVector<glm::vec2> projectFromWorldToScreen(
            const QVector<glm::vec2>& pointsInWorld,
            const unsigned int startIndex,
            const unsigned int endIndex) const;

        static std::shared_ptr<const GPUAPI::ResourceInGPU> captureGpuResource(
            const MapRenderer::MapSymbolReferenceOrigins& resources,
            const std::shared_ptr<const MapSymbol>& mapSymbol);

        static QVector<float> computePathSegmentsLengths(const QVector<glm::vec2>& path);

        static bool computePointIndexAndOffsetFromOriginAndOffset(
            const QVector<float>& pathSegmentsLengths,
            const unsigned int originPathPointIndex,
            const float nOffsetFromOriginPathPoint,
            const float offsetToPoint,
            unsigned int& outPathPointIndex,
            float& outOffsetFromPathPoint);

        static glm::vec2 computeExactPointFromOriginAndOffset(
            const QVector<glm::vec2>& path,
            const QVector<float>& pathSegmentsLengths,
            const unsigned int originPathPointIndex,
            const float offsetFromOriginPathPoint);

        static glm::vec2 computeExactPointFromOriginAndNormalizedOffset(
            const QVector<glm::vec2>& path,
            const unsigned int originPathPointIndex,
            const float nOffsetFromOriginPathPoint);
        static bool pathRenderableAs2D(
            const QVector<glm::vec2>& pathOnScreen,
            const unsigned int startPathPointIndex,
            const glm::vec2& exactStartPointOnScreen,
            const unsigned int endPathPointIndex,
            const glm::vec2& exactEndPointOnScreen);

        static bool segmentValidFor2D(const glm::vec2& vSegment);

        static glm::vec2 computePathDirection(
            const QVector<glm::vec2>& path,
            const unsigned int startPathPointIndex,
            const glm::vec2& exactStartPoint,
            const unsigned int endPathPointIndex,
            const glm::vec2& exactEndPoint);

        double computeDistanceBetweenCameraToPath(
            const QVector<glm::vec2>& pathInWorld,
            const unsigned int startPathPointIndex,
            const glm::vec2& exactStartPointInWorld,
            const unsigned int endPathPointIndex,
            const glm::vec2& exactEndPointInWorld) const;

        QVector<RenderableOnPathSymbol::GlyphPlacement> computePlacementOfGlyphsOnPath(
            const bool is2D,
            const QVector<glm::vec2>& path,
            const QVector<float>& pathSegmentsLengths,
            const unsigned int startPathPointIndex,
            const float offsetFromStartPathPoint,
            const unsigned int endPathPointIndex,
            const glm::vec2& directionOnScreen,
            const QVector<float>& glyphsWidths) const;

        OOBBF calculateOnPath2dOOBB(const std::shared_ptr<RenderableOnPathSymbol>& renderable) const;

        OOBBF calculateOnPath3dOOBB(const std::shared_ptr<RenderableOnPathSymbol>& renderable) const;

        // Debug-related:
        void addPathDebugLine(
            const QVector<PointI>& path31,
            const ColorARGB color) const;

        void addIntersectionDebugBox(
            const std::shared_ptr<const RenderableSymbol>& renderable,
            const ColorARGB color,
            const bool drawBorder = true) const;

        void addIntersectionDebugBox(
            const ScreenQuadTree::BBox intersectionBBox,
            const ColorARGB color,
            const bool drawBorder = true) const;

        static void convertRenderableSymbolsToMapSymbolInformation(
            const QList< std::shared_ptr<const RenderableSymbol> >& input,
            QList<IMapRenderer::MapSymbolInformation>& output);
    protected:
        QList< std::shared_ptr<const RenderableSymbol> > renderableSymbols;

        void prepare(AtlasMapRenderer_Metrics::Metric_renderFrame* const metric);
    public:
        AtlasMapRendererSymbolsStage(AtlasMapRenderer* const renderer);
        virtual ~AtlasMapRendererSymbolsStage();

        void queryLastPreparedSymbolsAt(
            const PointI screenPoint,
            QList<IMapRenderer::MapSymbolInformation>& outMapSymbols) const;
        void queryLastPreparedSymbolsIn(
            const AreaI screenArea,
            QList<IMapRenderer::MapSymbolInformation>& outMapSymbols,
            const bool strict = false) const;
        void queryLastVisibleSymbolsAt(
            const PointI screenPoint,
            QList<IMapRenderer::MapSymbolInformation>& outMapSymbols) const;
        void queryLastVisibleSymbolsIn(
            const AreaI screenArea,
            QList<IMapRenderer::MapSymbolInformation>& outMapSymbols,
            const bool strict = false) const;
    };
}

#endif // !defined(_OSMAND_CORE_ATLAS_MAP_RENDERER_SYMBOLS_STAGE_H_)
