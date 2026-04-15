#include "pch.h"
#include "BlenderSceneParser.h"

#include <fstream>
#include <regex>

#include "GameObject.h"

#include "App.h"

static int TexturesLoaded = 0;

namespace nam
{
    inline void MakeCustomGeometry(
        Vector<float> vertices,
        Vector<u32> indices,
        Mesh* p_mesh
    )
    {
        size_t vertexCount = vertices.size() / 8;

        Vector<Vertex>& meshVertices = p_mesh->m_vertices;
        Vector<u32>& meshIndices = p_mesh->m_indices;

        meshVertices.resize(vertexCount);

        for (size_t i = 0; i < vertexCount; i++)
        {
            size_t vIndex = i * 8;
            Vertex v;
            v.m_pos = {
                vertices[vIndex],
                vertices[vIndex + 2],
                vertices[vIndex + 1]
            };

            v.m_uv = {
                vertices[vIndex + 3],
                1.f - vertices[vIndex + 4]
            };

            v.m_normal = {
                vertices[vIndex + 5],
                vertices[vIndex + 7],
                vertices[vIndex + 6],
            };

            meshVertices[i] = v;
        }

        meshIndices.resize(indices.size());
        size_t reverseIndice = indices.size() - 1;
        for (size_t i = 0; i < indices.size(); ++i)
            meshIndices[reverseIndice - i] = indices[i];

        p_mesh->UpdateMinMaxBounds();
        p_mesh->UpdateCenterExtend();
    }

    std::vector<GameObject*> BlenderSceneParser::Load(std::wstring jsonPath, size tagScene, std::wstring textureFolder)
    {
        App* p_app = APP;
        Scene* p_scene = SCENE_MANAGER.GetScene(tagScene);

        if (p_scene == nullptr)
            return {};

        std::ifstream f(jsonPath);
        nlohmann::json data;

        try {
            data = nlohmann::json::parse(f);
        }
        catch (const std::exception& e) {
            Print("Error JSON: ", e.what());
            return {};
        }

        Vector<GameObject*> allCreatedObj;

        for (auto& obj : data)
        {
            std::string type = obj.value("t", "NONE");
            if (type != "MESH")
                continue;

            std::string name = obj.value("n", "Unnamed");
            GameObject* p_go = p_scene->CreateGameObject<GameObject>();
            p_go->m_name = name;

            HandleTransform(obj, p_go);
            Mesh* p_mesh = HandleMesh(obj, p_go);
            HandleMaterial(name, p_mesh);
            HandleTexture(obj, p_mesh, textureFolder);
            HandleCollider(name, p_go);

            allCreatedObj.push_back(p_go);
        }

        return allCreatedObj;
    }

    void BlenderSceneParser::HandleTransform(nlohmann::json::value_type& obj, GameObject* p_go)
    {
        std::string baseColorTex = "";
        XMFLOAT3 position = {};
        XMFLOAT4 rotation = {};
        XMFLOAT3 scale = {};

        if (obj.contains("p"))
        {
            position.x = obj["p"][0];
            position.y = obj["p"][2];
            position.z = obj["p"][1];
        }

        if (obj.contains("r"))
            rotation = { obj["r"][0], obj["r"][2], obj["r"][1], obj["r"][3] };
        else
            rotation = { 0.f, 0.f, 0.f, 0.f };

        if (obj.contains("s"))
            scale = { obj["s"][0], obj["s"][2], obj["s"][1] };
        else
            scale = { 1.f, 1.f, 1.f };


        p_go->mp_transform->SetWorldPosition(position);
        p_go->mp_transform->SetWorldRotation(rotation);
        p_go->mp_transform->SetWorldScale(scale);
    }

    void BlenderSceneParser::HandleMaterial(std::string name, Mesh* p_mesh)
    {
        std::smatch matMatch;
        std::regex patternMat("M([0-9]+)");

        if (std::regex_search(name, matMatch, patternMat))
        {
            std::string numberStr = matMatch[1];
            int number = std::stoi(numberStr);

            p_mesh->SetMaterialTag(number);
        }
    }
    void BlenderSceneParser::HandleTexture(nlohmann::json::value_type& obj, Mesh* p_mesh, std::wstring textureFolder)
    {
        std::string baseColorTex = {};

        if (obj.contains("tx"))
        {
            auto& tex = obj["tx"];

            if (tex.contains("Base Color"))
                baseColorTex = tex["Base Color"].get<std::string>();
        }

        if (!baseColorTex.empty())
        {
            size index = baseColorTex.size();
            baseColorTex = baseColorTex.substr(0, index - 3);
            baseColorTex += std::string("dds");

            std::wstring pathColor(baseColorTex.begin(), baseColorTex.end());

            int textureOffset = (MAX_TEXTURES - 3) - TexturesLoaded;
            TexturesLoaded++;

            APP->LoadTexture(textureFolder + pathColor, textureOffset, false);
            p_mesh->SetTexture(textureOffset);
        }
    }

    Mesh* BlenderSceneParser::HandleMesh(nlohmann::json::value_type& obj, GameObject* p_go)
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        if (obj.contains("m"))
        {
            auto& mesh = obj["m"];

            if (mesh.contains("v"))
                vertices = mesh["v"].get<Vector<float>>();

            if (mesh.contains("i"))
                indices = mesh["i"].get<Vector<u32>>();
        }

        p_go->AddComponent(MeshRendererComponent());
        MeshRendererComponent& meshComp = p_go->GetComponent<MeshRendererComponent>();
        Mesh* p_mesh = meshComp.CreateMeshInstance();
        MakeCustomGeometry(vertices, indices, p_mesh);

        return p_mesh;
    }

    void BlenderSceneParser::HandleCollider(std::string name, GameObject* p_go)
    {
        std::regex patternCollider("P");

        if (std::regex_search(name, patternCollider))
        {
            p_go->AddBoxCollider(0);

            PhysicComponent pc;
            pc.m_bounciness = 0.f;
            pc.m_isKinematic = true;
            pc.m_useGravity = false;

            p_go->AddComponent(pc);
        }
    }
}
