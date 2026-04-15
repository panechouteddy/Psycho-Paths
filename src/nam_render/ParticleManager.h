#pragma once
#include "RenderManager.h"

namespace nam
{
    class ParticleManager
    {
    private:
        int m_maxParticles;

        UploadBuffer<CBufferParticleCompute>* m_particleUploadBuffer;
        ID3D12Resource* m_particleBufferResource;

        UploadBuffer<CBufferParticleCompute>* m_spawnUploadBuffer;
        ID3D12Resource* m_spawnBufferResource;
        Vector<CBufferParticleCompute> m_pendingSpawns;

        UploadBuffer<CBufferSpawn>* m_particleSpawnCountBuffer; 
        ID3D12Resource* m_particleSpawnCountCBResource;

        ID3D12Resource* m_freeListBuffer;        
        ID3D12Resource* m_freeListUploadBuffer;    
        ID3D12Resource* m_freeCountBuffer;      
        ID3D12Resource* m_freeCountUploadBuffer;   

    public:
        ParticleManager(int maxParticles = MAX_PARTICLES);
        ~ParticleManager();

        void Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

        void Update(ID3D12GraphicsCommandList* cmdList);
        void FlushSpawns(ID3D12GraphicsCommandList* cmdList);

        void SpawnParticle(const CBufferParticleCompute& data);

        ID3D12Resource* GetParticleResource() const;
        int GetMaxParticles() const;

        void CreateViews(
            ID3D12Device* device,
            ID3D12DescriptorHeap* heap,
            UINT srvOffset,
            UINT uavOffset,
            UINT freeListOffset,
            UINT freeCountOffset,
            UINT spawnListSRVOffset,
            UINT descriptorSize
        );
    };
}