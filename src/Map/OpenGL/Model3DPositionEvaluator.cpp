#include "Model3DPositionEvaluator.h"

OsmAnd::Model3DPositionEvaluator::Model3DPositionEvaluator(const Model3D::BBox& bbox_, const glm::mat4& mTransform_, const int poinstForLine_)
    : _bbox(bbox_)
    , _mTransform(mTransform_)
    , _poinstForLine(qMax(2, poinstForLine_))
{
}

OsmAnd::Model3DPositionEvaluator::~Model3DPositionEvaluator()
{
}

void OsmAnd::Model3DPositionEvaluator::evaluate(float& outHeight, float& outRotationX, float& outRotationZ) const 
{
    const auto leftBack = _mTransform * glm::vec4(_bbox.minX(), 0.0f, _bbox.minZ(), 1.0f);
    const auto leftFront = _mTransform * glm::vec4(_bbox.minX(), 0.0f, _bbox.maxZ(), 1.0f);
    const auto rightBack = _mTransform * glm::vec4(_bbox.maxX(), 0.0f, _bbox.minZ(), 1.0f);
    const auto rightFront = _mTransform * glm::vec4(_bbox.maxX(), 0.0f, _bbox.maxZ(), 1.0f);

    
}