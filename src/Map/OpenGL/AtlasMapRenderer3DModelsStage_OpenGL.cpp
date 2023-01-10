#include "AtlasMapRenderer3DModelsStage_OpenGL.h"

#include "AtlasMapRenderer_Metrics.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::AtlasMapRenderer3DModelsStage_OpenGL(AtlasMapRenderer_OpenGL* const renderer_)
    : AtlasMapRenderer3DModelsStage(renderer_)
    , AtlasMapRendererStageHelper_OpenGL(this)
{
    typedef std::tuple<ZoomLevel, float> ZoomScale;
    QList<ZoomScale> scalesByZoomRange;
    scalesByZoomRange.push_back(ZoomScale(MinZoomLevel, 2.0f));
    scalesByZoomRange.push_back(ZoomScale(ZoomLevel4, 2.0f));
    scalesByZoomRange.push_back(ZoomScale(ZoomLevel9, 2.5f));
    scalesByZoomRange.push_back(ZoomScale(ZoomLevel18, 3.5f));
    scalesByZoomRange.push_back(ZoomScale(ZoomLevel22, 8.0f));
    scalesByZoomRange.push_back(ZoomScale(MaxZoomLevel, 10.0f));

    for (auto i = 1; i < scalesByZoomRange.size(); i++)
    {
        auto left = scalesByZoomRange.at(i - 1);
        auto right = scalesByZoomRange.at(i);

        auto leftZoom = std::get<0>(left);
        auto rightZoom = std::get<0>(right);
        auto zoomRange = rightZoom - leftZoom;

        auto leftScale = std::get<1>(left);
        auto rightScale = std::get<1>(right);
        auto scaleRange = rightScale - leftScale;

        auto lastRange = i + 1 == scalesByZoomRange.size();
        auto endZoomInclusive = lastRange ? rightZoom : rightZoom - 1;
        for (auto zoom = leftZoom; zoom <= endZoomInclusive; zoom = static_cast<ZoomLevel>(zoom + 1))
        {
            float scale = leftScale + scaleRange * (zoom - leftZoom) / zoomRange;
            _allZoomScales.push_back(scale);
        }
    }
}

OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::~AtlasMapRenderer3DModelsStage_OpenGL()
{
}

bool OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::initialize()
{
    ObjParser parser(QString("./model/model.obj"), QString("./model/"));
    _successfulLoadModel = parser.parse(_model, true);

    // Uncomment for custom coloring
    // QHash<QString, FColorRGBA> customColors;
    // customColors.insert("Body Material", FColorRGBA(0, 0, 1, 1));
    // customColors.insert("Glass Material", FColorRGBA(0, 1, 0, 1));
    // _model->setCustomMaterialColors(customColors);

    const auto gpuAPI = getGPUAPI();

    GL_CHECK_PRESENT(glGenBuffers);
    GL_CHECK_PRESENT(glBindBuffer);
    GL_CHECK_PRESENT(glBufferData);
    GL_CHECK_PRESENT(glEnableVertexAttribArray);
    GL_CHECK_PRESENT(glVertexAttribPointer);
    GL_CHECK_PRESENT(glDeleteShader);
    GL_CHECK_PRESENT(glDeleteProgram);

    // Compile vertex shader
    const QString vertexShader = QLatin1String(
        // Input data
        "INPUT vec3 in_vs_vertexPosition;                                                                                   ""\n"
        "INPUT vec4 in_vs_vertexColor;                                                                                      ""\n"
        "                                                                                                                   ""\n"
        // Output data to next shader stages
        "PARAM_OUTPUT vec4 v2f_color;                                                                                       ""\n"
        "                                                                                                                   ""\n"
        // Parameters: common data
        "uniform mat4 param_vs_mPerspectiveProjectionView;                                                                  ""\n"
        "                                                                                                                   ""\n"
        // Parameters: per-model data
        "uniform mat4 param_vs_mModel;                                                                                      ""\n"
        "                                                                                                                   ""\n"
        "void main()                                                                                                        ""\n"
        "{                                                                                                                  ""\n"
        "    vec4 v = vec4(in_vs_vertexPosition, 1.0);                                                                      ""\n"
        "    gl_Position = param_vs_mPerspectiveProjectionView * param_vs_mModel * v;                                       ""\n"
        "    v2f_color = in_vs_vertexColor;                                                                                 ""\n"
        "}                                                                                                                  ""\n");
    auto preprocessedVertexShader = vertexShader;
    gpuAPI->preprocessVertexShader(preprocessedVertexShader);
    gpuAPI->optimizeVertexShader(preprocessedVertexShader);
    const auto vsId = gpuAPI->compileShader(GL_VERTEX_SHADER, qPrintable(preprocessedVertexShader));
    if (vsId == 0)
    {
        LogPrintf(LogSeverityLevel::Error,
            "Failed to compile AtlasMapRenderer3DModelsStage_OpenGL vertex shader");
        return false;
    }

    // Compile fragment shader
    const QString fragmentShader = QLatin1String(
        // Input data
        "PARAM_INPUT vec4 v2f_color;                                                                                        ""\n"
        "                                                                                                                   ""\n"
        "void main()                                                                                                        ""\n"
        "{                                                                                                                  ""\n"
        "    FRAGMENT_COLOR_OUTPUT = v2f_color;                                                                             ""\n"
        "}                                                                                                                  ""\n");
    auto preprocessedFragmentShader = fragmentShader;
    QString preprocessedFragmentShader_UnrolledPerLayerProcessingCode;
    gpuAPI->preprocessFragmentShader(preprocessedFragmentShader);
    gpuAPI->optimizeFragmentShader(preprocessedFragmentShader);
    const auto fsId = gpuAPI->compileShader(GL_FRAGMENT_SHADER, qPrintable(preprocessedFragmentShader));
    if (fsId == 0)
    {
        glDeleteShader(vsId);
        GL_CHECK_RESULT;

        LogPrintf(LogSeverityLevel::Error,
            "Failed to compile AtlasMapRenderer3DModelsStage_OpenGL fragment shader");
        return false;
    }

    // Link everything into program object
    const GLuint shaders[] = { vsId, fsId };
    QHash< QString, GPUAPI_OpenGL::GlslProgramVariable > variablesMap;
    _program.id = gpuAPI->linkProgram(2, shaders, true, &variablesMap);
    if (!_program.id.isValid())
    {
        LogPrintf(LogSeverityLevel::Error,
            "Failed to link AtlasMapRenderer3DModelsStage_OpenGL program");
        return false;
    }

    bool ok = true;
    const auto& lookup = gpuAPI->obtainVariablesLookupContext(_program.id, variablesMap);
    ok = ok && lookup->lookupLocation(_program.vs.in.vertexPosition, "in_vs_vertexPosition", GlslVariableType::In);    
    ok = ok && lookup->lookupLocation(_program.vs.in.vertexColor, "in_vs_vertexColor", GlslVariableType::In);
    ok = ok && lookup->lookupLocation(_program.vs.params.mPerspectiveProjectionView, "param_vs_mPerspectiveProjectionView", GlslVariableType::Uniform);
    ok = ok && lookup->lookupLocation(_program.vs.params.mModel, "param_vs_mModel", GlslVariableType::Uniform);
    if (!ok)
    {
        glDeleteProgram(_program.id);
        GL_CHECK_RESULT;
        _program.id.reset();

        return false;
    }

    _3DModelVAO = gpuAPI->allocateUninitializedVAO();

    // Create vertex buffer and associate it with VAO
    glGenBuffers(1, &_3DModelVBO);
    GL_CHECK_RESULT;
    glBindBuffer(GL_ARRAY_BUFFER, _3DModelVBO);
    GL_CHECK_RESULT;
    glBufferData(GL_ARRAY_BUFFER, _model->getVerticesCount() * sizeof(Model3D::Vertex), _model->getVertices().constData(), GL_STATIC_DRAW);
    GL_CHECK_RESULT;
    glEnableVertexAttribArray(*_program.vs.in.vertexPosition);
    GL_CHECK_RESULT;
    glVertexAttribPointer(
        *_program.vs.in.vertexPosition,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Model3D::Vertex),
        reinterpret_cast<GLvoid*>(offsetof(Model3D::Vertex, xyz)));
    GL_CHECK_RESULT;
    glEnableVertexAttribArray(*_program.vs.in.vertexColor);
    GL_CHECK_RESULT;
    glVertexAttribPointer(
        *_program.vs.in.vertexColor,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Model3D::Vertex),
        reinterpret_cast<GLvoid*>(offsetof(Model3D::Vertex, rgba)));
    GL_CHECK_RESULT;

    // Unbind buffers
    gpuAPI->initializeVAO(_3DModelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK_RESULT;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GL_CHECK_RESULT;

    return true;
}

bool OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::render(IMapRenderer_Metrics::Metric_renderFrame* const metric_)
{
    if (!_successfulLoadModel)
        return true;

    const auto gpuAPI = getGPUAPI();

    GL_PUSH_GROUP_MARKER(QLatin1String("3DModels"));

    GL_CHECK_PRESENT(glUseProgram);
    GL_CHECK_PRESENT(glDrawArrays);

    const auto& internalState = getInternalState();

    gpuAPI->useVAO(_3DModelVAO);

    // Activate program
    glUseProgram(_program.id);
    GL_CHECK_RESULT;

    // Common data
    glUniformMatrix4fv(
        _program.vs.params.mPerspectiveProjectionView,
        1,
        GL_FALSE,
        glm::value_ptr(internalState.mPerspectiveProjectionView));
    GL_CHECK_RESULT;

    // Per-model data
    auto modelScale = 1 / currentState.visualZoom;
    const auto zoomLevel = currentState.zoomLevel;
    const auto visualZoom = currentState.visualZoom;
    const auto zoomScale = _allZoomScales.at(zoomLevel);
    
    // Calculate scale of model using zoom level and zoom fractional part
    if (visualZoom >= 1.0f)
    {
        if (zoomLevel == MaxZoomLevel)
            modelScale *= zoomScale;
        else
        {
            const auto nextZoomScale = _allZoomScales.at(zoomLevel + 1);
            const auto deltaScale = nextZoomScale - zoomScale;
            const auto zoomFraction = visualZoom - 1.0f;
            modelScale *= zoomScale + deltaScale * zoomFraction;
        }
    }
    else
    {
        if (zoomLevel == MinZoomLevel)
            modelScale *= zoomScale;
        else
        {
            const auto prevZoomScale = _allZoomScales.at(zoomLevel - 1);
            const auto deltaScale = zoomScale - prevZoomScale;
            const auto zoomFraction = -2 * (visualZoom - 1);
            modelScale *= zoomScale - deltaScale * zoomFraction;
        }
    }

    const auto mModelScale = glm::scale(glm::vec3(modelScale));

    auto modelBBox = _model->getBBox();
    modelBBox.setScale(modelScale);

    const auto centerElevation = getModelPointHeight(0.0f, 0.0f);
    const auto farLeftElevation = getModelPointHeight(modelBBox.minX(), modelBBox.minZ());
    const auto farRightElevation = getModelPointHeight(modelBBox.maxX(), modelBBox.minZ());
    const auto nearLeftElevation = getModelPointHeight(modelBBox.minX(), modelBBox.maxZ());
    const auto nearRightElevation = getModelPointHeight(modelBBox.maxX(), modelBBox.maxZ());

    const auto leftFar = PointF(modelBBox.minX(), modelBBox.minZ());
    const auto rightFar = PointF(modelBBox.maxX(), modelBBox.minZ());
    const auto leftNear = PointF(modelBBox.minX(), modelBBox.maxZ());
    const auto rightNear = PointF(modelBBox.maxX(), modelBBox.maxZ());

    const auto leftHeights = getModelLineHeights(leftFar, leftNear);
    const auto rightHeights = getModelLineHeights(rightFar, rightNear);
    const auto farHeights = getModelLineHeights(leftFar, rightFar);
    const auto nearHeights = getModelLineHeights(leftNear, rightNear);


    // Replace with sum of points on lines from above 
    const auto avgHeight = (farLeftElevation + farRightElevation + nearLeftElevation + nearRightElevation) / 4.0f;
    const auto modelHeight = centerElevation > avgHeight ? (avgHeight + centerElevation) / 2.0f : avgHeight;
    const auto mModelElevationTranslate = glm::translate(glm::vec3(0.0f, modelHeight, 0.0f));

    const auto leftHeightsHalfSize = leftHeights.size() / 2;
    const auto leftHeightsFarHalf = leftHeights.mid(0, leftHeightsHalfSize);
    const auto leftHeightsNearHalf = leftHeights.mid(leftHeights.size() - leftHeightsHalfSize, -1);
    const auto leftHeightsFarHalfSum = std::accumulate(leftHeightsFarHalf.begin(), leftHeightsFarHalf.end(), 0.0f);
    const auto leftHeightsNearHalfSum = std::accumulate(leftHeightsNearHalf.begin(), leftHeightsNearHalf.end(), 0.0f);
    const auto leftHeightsAvgDiff = (leftHeightsFarHalfSum - leftHeightsNearHalfSum) / leftHeightsHalfSize;

    const auto rightHeightsHalfSize = rightHeights.size() / 2;
    const auto rightHeightsFarHalf = rightHeights.mid(0, rightHeightsHalfSize);
    const auto rightHeightsNearHalf = rightHeights.mid(rightHeights.size() - rightHeightsHalfSize, -1);
    const auto rightHeightsFarHalfSum = std::accumulate(rightHeightsFarHalf.begin(), rightHeightsFarHalf.end(), 0.0f);
    const auto rightHeightsNearHalfSum = std::accumulate(rightHeightsNearHalf.begin(), rightHeightsNearHalf.end(), 0.0f);
    const auto rightHeightsAvgDiff = (rightHeightsFarHalfSum - rightHeightsNearHalfSum) / rightHeightsHalfSize;

    const auto avgElevationDiffZ = (leftHeightsAvgDiff + rightHeightsAvgDiff) / 2.0f;
    const auto rotationAngleX = static_cast<float>(qAtan(avgElevationDiffZ / (modelBBox.lengthZ() / 2.0f)));
    const auto mModelRotationX = glm::rotate(rotationAngleX, glm::vec3(1.0f, 0.0f, 0.0f));

    const auto farHeightsHalfSize = farHeights.size() / 2;
    const auto farHeightsLeftHalf = farHeights.mid(0, farHeightsHalfSize);
    const auto farHeightsRightHalf = farHeights.mid(farHeights.size() - farHeightsHalfSize, -1);
    const auto farHeightsLeftHalfSum = std::accumulate(farHeightsLeftHalf.begin(), farHeightsLeftHalf.end(), 0.0f);
    const auto farHeightsRightHalfSum = std::accumulate(farHeightsRightHalf.begin(), farHeightsRightHalf.end(), 0.0f);
    const auto farHeightsAvgDiff = (farHeightsRightHalfSum - farHeightsLeftHalfSum) / farHeightsHalfSize;

    const auto nearHeightsHalfSize = nearHeights.size() / 2;
    const auto nearHeightsLeftHalf = nearHeights.mid(0, nearHeightsHalfSize);
    const auto nearHeightsRightHalf = nearHeights.mid(nearHeights.size() - nearHeightsHalfSize, -1);
    const auto nearHeightsLeftHalfSum = std::accumulate(nearHeightsLeftHalf.begin(), nearHeightsLeftHalf.end(), 0.0f);
    const auto nearHeightsRightHalfSum = std::accumulate(nearHeightsRightHalf.begin(), nearHeightsRightHalf.end(), 0.0f);
    const auto nearHeightsAvgDiff = (nearHeightsRightHalfSum - nearHeightsLeftHalfSum) / nearHeightsHalfSize;

    const auto avgElevationDiffX = (farHeightsAvgDiff + nearHeightsAvgDiff) / 2.0f;
    const auto rotationAngleZ = static_cast<float>(qAtan(avgElevationDiffX / (modelBBox.lengthX() / 2.0f)));
    const auto mModelRotationZ = glm::rotate(rotationAngleZ, glm::vec3(0.0f, 0.0f, 1.0f));

    const auto mModelRotateOnRelief = mModelRotationX * mModelRotationZ;
    const auto mModel = mModelElevationTranslate * mModelRotateOnRelief * mModelScale;
    glUniformMatrix4fv(
        _program.vs.params.mModel,
        1,
        GL_FALSE,
        glm::value_ptr(mModel));
    GL_CHECK_RESULT;

    // Uncomment to debug
    // const auto mDebugTransform = mModelElevationTranslate * mModelRotateOnRelief;
    // addDebugLines(modelBBox, mDebugTransform, farLeftElevation, farRightElevation, nearLeftElevation, nearRightElevation);

    // Draw models actually
    glDrawArrays(GL_TRIANGLES, 0, _model->getVerticesCount());
    GL_CHECK_RESULT;

    // Deactivate program
    glUseProgram(0);
    GL_CHECK_RESULT;

    gpuAPI->unuseVAO();

    GL_POP_GROUP_MARKER;

    return true;
}

bool OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::release(bool gpuContextLost)
{
    const auto gpuAPI = getGPUAPI();

    GL_CHECK_PRESENT(glDeleteBuffers);

    if (_3DModelVAO.isValid())
    {
        gpuAPI->releaseVAO(_3DModelVAO, gpuContextLost);
        _3DModelVAO.reset();
    }

    if (_3DModelVBO.isValid())
    {
        if (!gpuContextLost)
        {
            glDeleteBuffers(1, &_3DModelVBO);
            GL_CHECK_RESULT;
        }
        _3DModelVBO.reset();
    }

    if (_program.id.isValid())
    {
        if (!gpuContextLost)
        {
            glDeleteProgram(_program.id);
            GL_CHECK_RESULT;
        }
        _program.id.reset();
    }

    return true;
}

QVector<float> OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::getModelLineHeights(const PointF& start, const PointF& end) const
{
    const auto deltaX = end.x - start.x;
    const auto deltaZ = end.y - start.y;
    const auto length = static_cast<float>(qSqrt(qPow(deltaX, 2.0f) + qPow(deltaZ, 2.0f)));
    
    int pointsCount = 4;
    
    QVector<float> heights;
    for (auto i = 0; i < pointsCount - 1; i++)
    {
        const auto percent = static_cast<float>(i) / pointsCount;
        const auto x = start.x + deltaX * percent;
        const auto z = start.y + deltaZ * percent;
        const auto height = getModelPointHeight(x, z);
        heights.push_back(height);
    }
    heights.push_back(getModelPointHeight(end.x, end.y));

    return heights;
}

float OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::getModelPointHeight(const float x, const float z) const
{
    const auto tileSize31 = Utilities::getPowZoom(ZoomLevel::MaxZoomLevel - currentState.zoomLevel);
    const auto offsetFromTargetX31 = x * tileSize31 / AtlasMapRenderer::TileSize3D;
    const auto offsetFromTargetY31 = z * tileSize31 / AtlasMapRenderer::TileSize3D;

    const auto vertexX31 = currentState.target31.x + offsetFromTargetX31;
    const auto vertexY31 = currentState.target31.y + offsetFromTargetY31;
    const PointI vertex31 = PointI(static_cast<int32_t>(vertexX31), static_cast<int32_t>(vertexY31));

    return AtlasMapRendererStageHelper_OpenGL::getRenderer()->getHeightOfLocation(vertex31);
}

void OsmAnd::AtlasMapRenderer3DModelsStage_OpenGL::addDebugLines(
    const Model3D::BBox& modelBBox,
    const glm::mat4& mTransform,
    const float farLeftElevation,
    const float farRightElevation,
    const float nearLeftElevation,
    const float nearRightElevation)
{
    const auto debugStage = AtlasMapRendererStageHelper_OpenGL::getRenderer()->debugStage;
    const auto bboxColor = ColorARGB(255, 0, 255, 0).argb;

    // Bottom quad of bbox
    QVector<glm::vec3> lines;
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.minY(), modelBBox.minZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.minY(), modelBBox.maxZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.minY(), modelBBox.maxZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.minY(), modelBBox.minZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.minY(), modelBBox.minZ(), 1.0f));
    debugStage->addLine3D(lines, bboxColor);

    // Top quad of bbox
    lines.clear();
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.maxY(), modelBBox.minZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.maxY(), modelBBox.maxZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.maxY(), modelBBox.maxZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.maxY(), modelBBox.minZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.maxY(), modelBBox.minZ(), 1.0f));
    debugStage->addLine3D(lines, bboxColor);

    // Vertical lines of bbox
    lines.clear();
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.minY(), modelBBox.minZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.maxY(), modelBBox.minZ(), 1.0f));
    debugStage->addLine3D(lines, bboxColor);
    lines.clear();
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.minY(), modelBBox.maxZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.minX(), modelBBox.maxY(), modelBBox.maxZ(), 1.0f));
    debugStage->addLine3D(lines, bboxColor);
    lines.clear();
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.minY(), modelBBox.minZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.maxY(), modelBBox.minZ(), 1.0f));
    debugStage->addLine3D(lines, bboxColor);
    lines.clear();
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.minY(), modelBBox.maxZ(), 1.0f));
    lines.push_back(mTransform * glm::vec4(modelBBox.maxX(), modelBBox.maxY(), modelBBox.maxZ(), 1.0f));
    debugStage->addLine3D(lines, bboxColor);

    // Quad diagonals of bbox heights intersecting with relief 
    const auto elevationColor = ColorARGB(255, 0, 0, 255).argb;
    QVector<glm::vec3> elevations;

    elevations.push_back(glm::vec4(modelBBox.minX(), farLeftElevation, modelBBox.minZ(), 1.0f));
    elevations.push_back(glm::vec4(modelBBox.maxX(), nearRightElevation, modelBBox.maxZ(), 1.0f));
    debugStage->addLine3D(elevations, elevationColor);

    elevations.clear();
    elevations.push_back(glm::vec4(modelBBox.minX(), nearLeftElevation, modelBBox.maxZ(), 1.0f));
    elevations.push_back(glm::vec4(modelBBox.maxX(), farRightElevation, modelBBox.minZ(), 1.0f));
    debugStage->addLine3D(elevations, elevationColor);
}