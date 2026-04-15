#pragma once
#include <BaseTypes.h>

struct ID3D12PipelineState;

namespace nam
{
    struct RootSignature;
    class InputLayout;
    struct Shader;

    enum class PsoType : u8
    {
        _Undefined,
        _Render3D,
        _Render2D,
        _RenderText,
        ParticleDraw,
        ParticleUpdate,
        ParticleSpawn,

        _Count
    };

    struct PipelineStateBuilder
    {
        Shader* mp_shader = nullptr;
        InputLayout* mp_inputLayout = nullptr;
        RootSignature* mp_rootSignature = nullptr;

        ID3D12PipelineState* mp_PSO = nullptr;
       
        ~PipelineStateBuilder();
        
        void UnInit();

        void Build(Shader* p_shader, InputLayout* p_inputLayout, RootSignature* p_rootSignature, bool for2D, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
        void BuildCompute(Shader* p_shader, RootSignature* p_rootSignature);

    private:
        bool m_for2D = false;

        void BuildPSO(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
    };
}

