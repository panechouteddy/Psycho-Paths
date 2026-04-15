#pragma once
#include <string>
#include <d3dcommon.h>
#include <wrl.h>

#define PATH_SHADER_TEXTURE_3D L"../../src/nam_render/Shaders/texture.hlsl"
#define PATH_SHADER_TEXTURE_2D L"../../src/nam_render/Shaders/texture2D.hlsl"

#define PATH_SHADER_PARTICLE_DRAW L"../../src/nam_render/Shaders/particleDraw.hlsl"
#define PATH_SHADER_PARTICLE_UPDATE L"../../src/nam_render/Shaders/particleUpdate.hlsl"
#define PATH_SHADER_PARTICLE_SPAWN L"../../src/nam_render/Shaders/particleSpawn.hlsl"

namespace nam
{
    struct Shader
    {
        Microsoft::WRL::ComPtr<ID3DBlob> mp_vsByteCode;
        Microsoft::WRL::ComPtr<ID3DBlob> mp_psByteCode;
        Microsoft::WRL::ComPtr<ID3DBlob> mp_csByteCode;
        std::string m_name = "None";

        Shader() = default;
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void Compile(const std::string& name, std::wstring path, bool isCS);

        static Shader* Create(const std::string& name, std::wstring path, bool isCS = false);
    };
}

