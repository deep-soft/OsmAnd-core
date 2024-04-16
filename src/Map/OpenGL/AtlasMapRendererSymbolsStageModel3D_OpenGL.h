#ifndef _OSMAND_CORE_ATLAS_MAP_RENDERER_SYMBOLS_STAGE_MODEL_3D_OPENGL_H_
#define _OSMAND_CORE_ATLAS_MAP_RENDERER_SYMBOLS_STAGE_MODEL_3D_OPENGL_H_

#include "AtlasMapRendererSymbolsStage_OpenGL.h"
#include "AtlasMapRendererSymbolsStageModel3D.h"

namespace OsmAnd
{
    class AtlasMapRendererSymbolsStageModel3D_OpenGL : public AtlasMapRendererSymbolsStageModel3D
    {
    private:
        OsmAnd::GPUAPI_OpenGL* getGPUAPI() const;
        OsmAnd::AtlasMapRenderer_OpenGL* getRenderer() const;
        OsmAnd::AtlasMapRendererSymbolsStage_OpenGL* getSymbolsStage() const;

    protected:
        struct Model3DProgram
        {
            GLname id;

            // Vertex data
            struct
            {
                // Input data
                struct
                {
                    GLlocation vertexPosition;
                    GLlocation vertexColor;
                } in;

                // Params
                struct
                {
                    // Per-model data
                    GLlocation mModel;

                    // Common data
                    GLlocation mPerspectiveProjectionView;
                } params;
            } vs;
        } _program;

    public:
        AtlasMapRendererSymbolsStageModel3D_OpenGL(AtlasMapRendererSymbolsStage_OpenGL* const symbolsStage);
        virtual ~AtlasMapRendererSymbolsStageModel3D_OpenGL();

        bool initialize() override;
        bool render(
            const std::shared_ptr<const RenderableModel3DSymbol>& renderable,
            AlphaChannelType& currentAlphaChannelType) override;
        bool release(const bool gpuContextLost) override;
    };
}

#endif // !defined(_OSMAND_CORE_ATLAS_MAP_RENDERER_SYMBOLS_STAGE_MODEL_3D_OPENGL_H_)
