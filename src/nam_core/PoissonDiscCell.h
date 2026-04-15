#pragma once
#include "DirectXMath.h"
namespace nam
{
    struct PoissonDiscCell
    {
        bool hasPoint = false;
        DirectX::XMFLOAT3 point = { 0.f,0.f,0.f };
    };
}
