#ifndef _OSMAND_CORE_MODEL3D_POSITION_EVALUATOR_H_
#define _OSMAND_CORE_MODEL3D_POSITION_EVALUATOR_H_

#include "QtExtensions.h"
#include "PointsAndAreas.h"
#include "Model3D.h"

#include <glm/glm.hpp>

#include <QVector>

namespace OsmAnd
{
    class Model3DPositionEvaluator
    {
    private:
        const Model3D::BBox _bbox;
        const glm::mat4 _mTransform;
        const int _poinstForLine;

        QVector<float> getLineHeights();
    public:
        Model3DPositionEvaluator(const Model3D::BBox& bbox, const glm::mat4& mTransform, const int poinstForLine);
        virtual ~Model3DPositionEvaluator();

        void evaluate(float& outHeight, float& outRotationX, float& outRotationZ) const;
    };
}

#endif // !defined(_OSMAND_CORE_MODEL3D_POSITION_EVALUATOR_H_)