struct Particle
{
    float3 Position;
    float3 Velocity;
    float4 StartColor;
    float4 EndColor;
    float LifeTime;
    float CurrentLife;
};

StructuredBuffer<Particle> g_SpawnList : register(t0);
RWStructuredBuffer<Particle> g_Particles : register(u0);
RWBuffer<uint> g_FreeList : register(u1);
RWBuffer<uint> g_FreeCount : register(u2);

cbuffer cbSpawn : register(b0)
{
    uint g_SpawnCount;  
    uint3 g_Padding;
}

[numthreads(64, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
    uint spawnIndex = DTid.x;
    
    if (spawnIndex >= g_SpawnCount)
        return;
    
    uint originalCount;
    InterlockedOr(g_FreeCount[0], 0, originalCount); 
    if (originalCount == 0)
        return;

    uint freeIndex;
    InterlockedAdd(g_FreeCount[0], -1, freeIndex);
    if (freeIndex == 0) 
    {
        InterlockedAdd(g_FreeCount[0], 1, freeIndex);
        return;
    }
    
    uint targetIndex = g_FreeList[freeIndex - 1];
    
    Particle newParticle = g_SpawnList[spawnIndex];
    newParticle.CurrentLife = 0;
        
    g_Particles[targetIndex] = newParticle;
}