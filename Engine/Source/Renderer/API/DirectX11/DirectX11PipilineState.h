#pragma once

#include <Core.h>

#include "Renderer/Base/PipelineState.h"
#include "Platform/Platform.h"


namespace Pawn::Render
{
    class DirectX11Shader;
    class DirectX11VertexBuffer;

    class DirectX11PipilineState : public PipelineState
    {
    public:
        DirectX11PipilineState();
        ~DirectX11PipilineState();

    public:
        virtual void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) override;
        virtual void SetBlendState(bool enableBlend, BlendMask mask) override;
        virtual void SetRasterizerState(RasterizerCull cull, RasterizerFill fill, bool frontCounterClockwide, int32 depthBias, float32 depthBiasClamp) override;

        virtual void Bind() override;

    private:
        void Init();

    private:
        ID3D11InputLayout* m_InputLayout;
        ID3D11DepthStencilState* m_DepthStencilState;
        ID3D11RasterizerState* m_RasterizerState;
        ID3D11BlendState* m_BlendState;

    };
}

