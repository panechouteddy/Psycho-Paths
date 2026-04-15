
cbuffer cbViewProj : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float time;

    float deltaTime;
};

struct Particle
{
    float3 Position;
    float3 Velocity;
    float4 StartColor;
    float4 EndColor;
    float LifeTime;
    float CurrentLife;
};

RWStructuredBuffer<Particle> g_Particles : register(u0);
RWBuffer<uint> g_FreeList : register(u1);
RWBuffer<uint> g_FreeCount : register(u2);

[numthreads(256, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    
    Particle p = g_Particles[index];
    
    if (p.CurrentLife >= p.LifeTime)
        return;
        
    p.CurrentLife += deltaTime;
    if (p.CurrentLife >= p.LifeTime)
    {        
        uint freeIndex;
        InterlockedAdd(g_FreeCount[0], 1, freeIndex);
        g_FreeList[freeIndex] = index;  
    }
    else
    {
        p.Position += p.Velocity * deltaTime;
    }
    
    g_Particles[index] = p;    
}