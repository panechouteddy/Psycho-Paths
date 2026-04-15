
struct Particle
{
    float3 Position;
    float3 Velocity;
    float4 StartColor;
    float4 EndColor;
    float LifeTime;
    float CurrentLife;
};

StructuredBuffer<Particle> g_ParticleBuffer : register(t0);

cbuffer cbViewProj : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float time;

    float deltaTime;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float Size : PSIZE;
};

VertexOut VS(uint instanceId : SV_InstanceID)
{
    VertexOut vout;
    
    Particle p = g_ParticleBuffer[instanceId];
    
    if (p.CurrentLife >= p.LifeTime)
    {
        vout.PosH = float4(0, 0, 0, 0);
        vout.Size = 0.f;
        return vout;
    }
    
    float4 posW = float4(p.Position, 1.0f);
    vout.PosH = mul(posW, mul(view, proj));

    float t = saturate(p.CurrentLife / p.LifeTime);
    vout.Color = lerp(p.StartColor, p.EndColor, t);

    vout.Size = 1.f;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    clip(pin.Size - 0.001);
    return pin.Color;
}