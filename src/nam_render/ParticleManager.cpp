#include "pch.h"
#include "ParticleManager.h"
#include "RenderWrapper.h"
#include "CommandManager.h"

namespace nam
{
    ParticleManager::ParticleManager(int maxParticles) :
        m_maxParticles(maxParticles),
        m_particleUploadBuffer(nullptr),
        m_particleBufferResource(nullptr),
        m_spawnUploadBuffer(nullptr),
        m_spawnBufferResource(nullptr),
        m_freeListBuffer(nullptr),
        m_freeListUploadBuffer(nullptr),
        m_freeCountBuffer(nullptr),
        m_freeCountUploadBuffer(nullptr),
        m_particleSpawnCountBuffer(nullptr),
        m_particleSpawnCountCBResource(nullptr)
    {
    }

    ParticleManager::~ParticleManager()
    {
        if (m_particleUploadBuffer)
        {
            delete m_particleUploadBuffer;
            m_particleUploadBuffer = nullptr;
        }

        if (m_spawnUploadBuffer)
        {
            delete m_spawnUploadBuffer;
            m_spawnUploadBuffer = nullptr;
        }

        if (m_particleSpawnCountBuffer)
        {
            delete m_particleSpawnCountBuffer;
            m_particleSpawnCountBuffer = nullptr;
        }

        if (m_particleBufferResource)
        {
            m_particleBufferResource->Release();
            m_particleBufferResource = nullptr;
        }

        if (m_spawnBufferResource)
        {
            m_spawnBufferResource->Release();
            m_spawnBufferResource = nullptr;
        }

        if (m_freeListBuffer)
        {
            m_freeListBuffer->Release();
            m_freeListBuffer = nullptr;
        }

        if (m_freeListUploadBuffer)
        {
            m_freeListUploadBuffer->Release();
            m_freeListUploadBuffer = nullptr;
        }

        if (m_freeCountBuffer)
        {
            m_freeCountBuffer->Release();
            m_freeCountBuffer = nullptr;
        }

        if (m_freeCountUploadBuffer)
        {
            m_freeCountUploadBuffer->Release();
            m_freeCountUploadBuffer = nullptr;
        }

        if (m_particleSpawnCountCBResource)
        {
            m_particleSpawnCountCBResource->Release();
            m_particleSpawnCountCBResource = nullptr;
        }
    }

    void ParticleManager::Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
    {
        m_particleUploadBuffer = new UploadBuffer<CBufferParticleCompute>(device, m_maxParticles, false);

        D3D12_RESOURCE_DESC bufDesc = {};
        bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufDesc.Width = m_maxParticles * sizeof(CBufferParticleCompute);
        bufDesc.Height = 1;
        bufDesc.DepthOrArraySize = 1;
        bufDesc.MipLevels = 1;
        bufDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufDesc.SampleDesc.Count = 1;
        bufDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        D3D12_HEAP_PROPERTIES heapPropsDefault = {};
        heapPropsDefault.Type = D3D12_HEAP_TYPE_DEFAULT;

        ThrowIfFailed(device->CreateCommittedResource(
            &heapPropsDefault,
            D3D12_HEAP_FLAG_NONE,
            &bufDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_particleBufferResource)
        ));

        m_spawnUploadBuffer = new UploadBuffer<CBufferParticleCompute>(device, MAX_PARTICLES_SPAWNS_PER_FRAME, false);

        D3D12_RESOURCE_DESC spawnBufDesc = {};
        spawnBufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        spawnBufDesc.Width = MAX_PARTICLES_SPAWNS_PER_FRAME * sizeof(CBufferParticleCompute);
        spawnBufDesc.Height = 1;
        spawnBufDesc.DepthOrArraySize = 1;
        spawnBufDesc.MipLevels = 1;
        spawnBufDesc.Format = DXGI_FORMAT_UNKNOWN;
        spawnBufDesc.SampleDesc.Count = 1;
        spawnBufDesc.Flags = D3D12_RESOURCE_FLAG_NONE;  
        spawnBufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ThrowIfFailed(device->CreateCommittedResource(
            &heapPropsDefault,
            D3D12_HEAP_FLAG_NONE,
            &spawnBufDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,  
            nullptr,
            IID_PPV_ARGS(&m_spawnBufferResource)
        ));

        D3D12_RESOURCE_DESC freeListDesc = {};
        freeListDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        freeListDesc.Width = m_maxParticles * sizeof(UINT);
        freeListDesc.Height = 1;
        freeListDesc.DepthOrArraySize = 1;
        freeListDesc.MipLevels = 1;
        freeListDesc.Format = DXGI_FORMAT_UNKNOWN;
        freeListDesc.SampleDesc.Count = 1;
        freeListDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        freeListDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ThrowIfFailed(device->CreateCommittedResource(
            &heapPropsDefault,
            D3D12_HEAP_FLAG_NONE,
            &freeListDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_freeListBuffer)
        ));

        D3D12_RESOURCE_DESC freeCountDesc = {};
        freeCountDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        freeCountDesc.Width = sizeof(UINT);
        freeCountDesc.Height = 1;
        freeCountDesc.DepthOrArraySize = 1;
        freeCountDesc.MipLevels = 1;
        freeCountDesc.Format = DXGI_FORMAT_UNKNOWN;
        freeCountDesc.SampleDesc.Count = 1;
        freeCountDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        freeCountDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ThrowIfFailed(device->CreateCommittedResource(
            &heapPropsDefault,
            D3D12_HEAP_FLAG_NONE,
            &freeCountDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_freeCountBuffer)
        ));

        D3D12_HEAP_PROPERTIES heapPropsUpload = {};
        heapPropsUpload.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC uploadDesc = {};
        uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadDesc.Width = m_maxParticles * sizeof(UINT);
        uploadDesc.Height = 1;
        uploadDesc.DepthOrArraySize = 1;
        uploadDesc.MipLevels = 1;
        uploadDesc.Format = DXGI_FORMAT_UNKNOWN;
        uploadDesc.SampleDesc.Count = 1;
        uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ThrowIfFailed(device->CreateCommittedResource(
            &heapPropsUpload,
            D3D12_HEAP_FLAG_NONE,
            &uploadDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_freeListUploadBuffer)
        ));

        uploadDesc.Width = sizeof(UINT);
        ThrowIfFailed(device->CreateCommittedResource(
            &heapPropsUpload,
            D3D12_HEAP_FLAG_NONE,
            &uploadDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_freeCountUploadBuffer)
        ));

        UINT* freeListData;
        m_freeListUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&freeListData));
        for (UINT i = 0; i < static_cast<UINT>(m_maxParticles); i++)
        {
            freeListData[i] = i;
        }
        m_freeListUploadBuffer->Unmap(0, nullptr);

        UINT* freeCountData;
        m_freeCountUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&freeCountData));
        *freeCountData = m_maxParticles;
        m_freeCountUploadBuffer->Unmap(0, nullptr);

        {
            D3D12_RESOURCE_BARRIER barriers[] = {
                CD3DX12_RESOURCE_BARRIER::Transition(m_particleBufferResource,
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
                CD3DX12_RESOURCE_BARRIER::Transition(m_freeListBuffer,
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
                CD3DX12_RESOURCE_BARRIER::Transition(m_freeCountBuffer,
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)
            };
            cmdList->ResourceBarrier(3, barriers);
        }

        cmdList->CopyResource(m_particleBufferResource, m_particleUploadBuffer->Resource());
        cmdList->CopyResource(m_freeListBuffer, m_freeListUploadBuffer);
        cmdList->CopyResource(m_freeCountBuffer, m_freeCountUploadBuffer);

        {
            D3D12_RESOURCE_BARRIER barriers[] = {
                CD3DX12_RESOURCE_BARRIER::Transition(m_particleBufferResource,
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
                CD3DX12_RESOURCE_BARRIER::Transition(m_freeListBuffer,
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
                CD3DX12_RESOURCE_BARRIER::Transition(m_freeCountBuffer,
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
            };
            cmdList->ResourceBarrier(3, barriers);
        }

        m_particleSpawnCountBuffer = new UploadBuffer<CBufferSpawn>(device, 1, true);
        m_particleSpawnCountCBResource = m_particleSpawnCountBuffer->Resource();
    }

    void ParticleManager::Update(ID3D12GraphicsCommandList* cmdList)
    {
        auto uavBarrier1 = CD3DX12_RESOURCE_BARRIER::UAV(m_particleBufferResource);
        cmdList->ResourceBarrier(1, &uavBarrier1);

        cmdList->SetPipelineState(RENDER_MANAGER->GetPsoManager()->GetPSO(PsoType::ParticleUpdate));
        RENDER_MANAGER->SetComputeRootSignature(RootSignatureName::_RootUpdateParticle);

        cmdList->SetComputeRootConstantBufferView(
            0,
            RENDER_MANAGER->GetConstantBuffersDatas()->GetGPUVirtualAdressOf(CBufferType::_ViewProj)
        );

        D3D12_GPU_DESCRIPTOR_HANDLE uavHandles[3];
        uavHandles[0] = RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetUAVHandle(PARTICLE_UAV_SLOT);
        uavHandles[1] = RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetUAVHandle(PARTICLE_FREELIST_SLOT);
        uavHandles[2] = RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetUAVHandle(PARTICLE_FREECOUNT_SLOT);

        cmdList->SetComputeRootDescriptorTable(1, uavHandles[0]);
        cmdList->SetComputeRootDescriptorTable(2, uavHandles[1]);
        cmdList->SetComputeRootDescriptorTable(3, uavHandles[2]);

        int numGroups = (m_maxParticles + 255) / 256;
        cmdList->Dispatch(numGroups, 1, 1);

        CD3DX12_RESOURCE_BARRIER uavBarrier2 = CD3DX12_RESOURCE_BARRIER::UAV(m_particleBufferResource);
        cmdList->ResourceBarrier(1, &uavBarrier2);
    }

    void ParticleManager::FlushSpawns(ID3D12GraphicsCommandList* cmdList)
    {
        if (m_pendingSpawns.empty())
            return;

        UINT spawnCount = (UINT)m_pendingSpawns.size();

        CBufferSpawn spawnData;
        spawnData.spawnCount = spawnCount;
        m_particleSpawnCountBuffer->CopyData(0, spawnData);

        for (UINT i = 0; i < spawnCount; i++)
        {
            m_spawnUploadBuffer->CopyData(i, m_pendingSpawns[i]);
        }

        cmdList->CopyResource(m_spawnBufferResource, m_spawnUploadBuffer->Resource());

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_spawnBufferResource,
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
        );
        cmdList->ResourceBarrier(1, &barrier);

        cmdList->SetPipelineState(RENDER_MANAGER->GetPsoManager()->GetPSO(PsoType::ParticleSpawn));
        RENDER_MANAGER->SetComputeRootSignature(RootSignatureName::_RootSpawnParticle);

        cmdList->SetComputeRootConstantBufferView(0, m_particleSpawnCountCBResource->GetGPUVirtualAddress());

        D3D12_GPU_DESCRIPTOR_HANDLE spawnSRV =
            RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetSRVHandle(PARTICLE_SPAWN_SLOT);
        cmdList->SetComputeRootDescriptorTable(1, spawnSRV);

        D3D12_GPU_DESCRIPTOR_HANDLE particleUAV =
            RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetUAVHandle(PARTICLE_UAV_SLOT);
        cmdList->SetComputeRootDescriptorTable(2, particleUAV);

        D3D12_GPU_DESCRIPTOR_HANDLE freeListUAV =
            RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetUAVHandle(PARTICLE_FREELIST_SLOT);
        cmdList->SetComputeRootDescriptorTable(3, freeListUAV);

        D3D12_GPU_DESCRIPTOR_HANDLE freeCountUAV =
            RENDER_MANAGER->GetDescriptorHeapsWrapper()->GetUAVHandle(PARTICLE_FREECOUNT_SLOT);
        cmdList->SetComputeRootDescriptorTable(4, freeCountUAV);

        UINT groups = (spawnCount + 63) / 64;
        cmdList->Dispatch(groups, 1, 1);

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_spawnBufferResource,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_COPY_DEST
        );
        cmdList->ResourceBarrier(1, &barrier);

        CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_particleBufferResource);
        cmdList->ResourceBarrier(1, &uavBarrier);

        m_pendingSpawns.clear();
    }

    void ParticleManager::SpawnParticle(const CBufferParticleCompute& data)
    {
        if (m_pendingSpawns.size() >= MAX_PARTICLES_SPAWNS_PER_FRAME)
            return;
        m_pendingSpawns.push_back(data);
    }

    ID3D12Resource* ParticleManager::GetParticleResource() const
    {
        return m_particleBufferResource;
    }

    int ParticleManager::GetMaxParticles() const
    {
        return m_maxParticles;
    }

    void ParticleManager::CreateViews(
        ID3D12Device* device,
        ID3D12DescriptorHeap* heap,
        UINT srvOffset,
        UINT uavOffset,
        UINT freeListOffset,
        UINT freeCountOffset,
        UINT spawnListSRVOffset,
        UINT descriptorSize
    )
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = m_maxParticles;
        srvDesc.Buffer.StructureByteStride = sizeof(CBufferParticleCompute);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(heap->GetCPUDescriptorHandleForHeapStart(), srvOffset, descriptorSize);
        device->CreateShaderResourceView(m_particleBufferResource, &srvDesc, srvHandle);

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = m_maxParticles;
        uavDesc.Buffer.StructureByteStride = sizeof(CBufferParticleCompute);
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE uavHandle(heap->GetCPUDescriptorHandleForHeapStart(), uavOffset, descriptorSize);
        device->CreateUnorderedAccessView(m_particleBufferResource, nullptr, &uavDesc, uavHandle);

        D3D12_UNORDERED_ACCESS_VIEW_DESC freeListUAVDesc = {};
        freeListUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        freeListUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        freeListUAVDesc.Buffer.FirstElement = 0;
        freeListUAVDesc.Buffer.NumElements = m_maxParticles;
        freeListUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE freeListHandle(heap->GetCPUDescriptorHandleForHeapStart(), freeListOffset, descriptorSize);
        device->CreateUnorderedAccessView(m_freeListBuffer, nullptr, &freeListUAVDesc, freeListHandle);

        D3D12_UNORDERED_ACCESS_VIEW_DESC freeCountUAVDesc = {};
        freeCountUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        freeCountUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        freeCountUAVDesc.Buffer.FirstElement = 0;
        freeCountUAVDesc.Buffer.NumElements = 1;
        freeCountUAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE freeCountHandle(heap->GetCPUDescriptorHandleForHeapStart(), freeCountOffset, descriptorSize);
        device->CreateUnorderedAccessView(m_freeCountBuffer, nullptr, &freeCountUAVDesc, freeCountHandle);

        D3D12_SHADER_RESOURCE_VIEW_DESC spawnSRVDesc = {};
        spawnSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        spawnSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        spawnSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        spawnSRVDesc.Buffer.FirstElement = 0;
        spawnSRVDesc.Buffer.NumElements = MAX_PARTICLES_SPAWNS_PER_FRAME;
        spawnSRVDesc.Buffer.StructureByteStride = sizeof(CBufferParticleCompute);
        spawnSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        CD3DX12_CPU_DESCRIPTOR_HANDLE spawnSRVHandle(heap->GetCPUDescriptorHandleForHeapStart(),
            spawnListSRVOffset, descriptorSize);
        device->CreateShaderResourceView(m_spawnBufferResource, &spawnSRVDesc, spawnSRVHandle);
    }
}