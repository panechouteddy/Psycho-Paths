#pragma once
#include "ARenderItem.h"
#include "CubeFace.h"

namespace nam
{
    struct VoxelGrid;

    struct Mesh : public ARenderItem
    {
        DirectX::XMFLOAT3 m_minBounds = {};
        DirectX::XMFLOAT3 m_maxBounds = {};
        DirectX::XMFLOAT3 m_center = {};
        DirectX::XMFLOAT3 m_extend = {};

        float m_radius = -1;
    private:
        int m_materialTag = MAX_MATERIALS - 1; //Default Material white albedo, with 1 roughness, 0 metalness

    public:
        Mesh();
        ~Mesh();

        void ComputeNormals(bool smoothShading = true);

        void BuildCube(float size, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 }, DirectX::XMFLOAT3 ypr = {0, 0, 0});
        void BuildBox(DirectX::XMFLOAT3 size, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 }, DirectX::XMFLOAT3 ypr = { 0, 0, 0 });

        void BuildCylinder(float radius, int circleStacks, float height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 }, DirectX::XMFLOAT3 ypr = { 0, 0, 0 });
        void BuildCone(float baseRadius, int baseStacks, float height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 }, DirectX::XMFLOAT3 ypr = { 0, 0, 0 });

        //With Texture, it's better to use it, but without, IcoSphere has a better topology for the lighting.
        void BuildUvSphere(float radius, int stacks, int slices, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 });

        //With Texture, it could be weird looking so UvSphere is better, but without it's better to use this though
        void BuildIcoSphere(float radius, int subdivision, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 });

        void BuildCapsule(float radius, int stacks, int slices, float height, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 offset = { 0, 0, 0 }, DirectX::XMFLOAT3 ypr = { 0, 0, 0 });
        void BuildPlane(DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, bool doubleFaced = false, DirectX::XMFLOAT3 offset = { 0, 0, 0 }, DirectX::XMFLOAT3 ypr = { 0, 0, 0 });

        void BuildFromVoxels(VoxelGrid& grid, DirectX::XMFLOAT4 color);
        void AddCubeFace(const DirectX::XMFLOAT3& center, float halfVoxel, CubeFace face, const DirectX::XMFLOAT4& color);

        void MakeVerticesColorFromNormals(DirectX::XMFLOAT4 flatColor, DirectX::XMFLOAT4 slopeColor);

        void MakeNoise(int octaves = 1, float frequency = 1.f, float amplitude = 1.f);

        void AddTriangle(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3, DirectX::XMFLOAT4 color, bool doubleFaced = false);

        void LoadObj(const std::wstring path, DirectX::XMFLOAT3 color = {1, 1, 1});
        void LoadGlb(const std::wstring path);

        void SetMaterialTag(int tag);
        const int GetMaterialTag() const override;

        void SetAnimData(MeshAnimationModel* m);

        //Doesn't copy GPU datas
        //The target mesh will receive This mesh data
        //Can be risky to use 
        void CopyDataTo(Mesh* p_target);

        //Doesn't copy GPU datas
        //This mesh will receive the source data
        //Can be risky to use 
        void CopyDataFrom(Mesh* p_source);

        void UpdateMinMaxBounds();
        void UpdateCenterExtend();

        //Get the radius of the smallest Sphere which contains your mesh
        float GetBoundingSphereRadius();
    private:
    
        DirectX::XMVECTOR ComputeNormal(DirectX::FXMVECTOR p0, DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2);
    };
}