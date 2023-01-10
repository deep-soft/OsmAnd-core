#ifndef _OSMAND_CORE_MODEL_3D_H_
#define _OSMAND_CORE_MODEL_3D_H_

#include "QtExtensions.h"
#include "Color.h"
#include "PointsAndAreas.h"

#include <QVector>
#include <QHash>
#include <QString>

namespace OsmAnd
{
    class Model3D
    {
    public:
        struct VertexInfo
        {
            float xyz[3];
            float rgba[4];
            QString materialName;
        };

        struct BBox
        {
        private:
            const float _minX, _maxX;
            const float _minY, _maxY;
            const float _minZ, _maxZ;

            float scale = 1.0f;

        public:
            BBox(float minX_, float maxX_, float minY_, float maxY_, float minZ_, float maxZ_)
                : _minX(minX_)
                , _maxX(maxX_)
                , _minY(minY_)
                , _maxY(maxY_)
                , _minZ(minZ_)
                , _maxZ(maxZ_)
            {
            }

            void setScale(const float scale)
            {
                this->scale = scale;
            }

            float minX() const
            {
                return _minX * scale;
            }

            float maxX() const
            {
                return _maxX * scale;
            }

            float minY() const
            {
                return _minY * scale;
            }

            float maxY() const
            {
                return _maxY * scale;
            }

            float minZ() const
            {
                return _minZ * scale;
            }

            float maxZ() const
            {
                return _maxZ * scale;
            }

            float lengthX() const
            {
                return maxX() - minX();
            }

            float lengthY() const
            {
                return maxY() - minY();
            }

            float lengthZ() const
            {
                return maxZ() - minZ();
            }
        };

    private:
        QVector<VertexInfo> _vertexInfos;
        QHash<QString, FColorRGBA> _customMaterialColors;

        BBox _bbox;
    public:
#pragma pack(push, 1)
        struct Vertex {
            float xyz[3];
            float rgba[4];
        };
#pragma pack(pop)

        Model3D(const QVector<VertexInfo>& vertexInfos, const BBox& bbox);
        virtual ~Model3D();

        void setCustomMaterialColors(const QHash<QString, FColorRGBA>& customMaterialColors);
        void useDefaultMaterialColors();
        
        const int getVerticesCount() const;
        const QVector<Model3D::Vertex> getVertices() const;

        const BBox getBBox() const;
    };
}

#endif // !defined(_OSMAND_CORE_MODEL_3D_H_)