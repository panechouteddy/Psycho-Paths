#include "pch.h"
#include "BlenderSceneCustom.h"

#include <fstream>
#include <regex>

#include "GameObject.h"

#include "App.h"

#include "Chunk.h"
#include "Room.h"

#include "SavePoint.h"
#include "Door.h"
#include "Battery.h"
#include "Locker.h"
#include "FlashlightCollectible.h"
#include "EndscreenTrigger.h"
#include "PursuitTrigger.h"

#include "DigiCodeObject.h"

#include "InteractComponent.h"

#include "GameScene.h"

#include "InventoryScene.h"

static int TexturesLoaded = 0;
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

inline XMFLOAT3 GetBoundingBox(Vector<float> vertices, Vector<u32> indices)
{
    size_t vertexCount = vertices.size() / 8;

    Mesh tmpMesh;
    Vector<Vertex>& meshVertices = tmpMesh.m_vertices;
    Vector<u32>& meshIndices = tmpMesh.m_indices;

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

    tmpMesh.UpdateMinMaxBounds();
    tmpMesh.UpdateCenterExtend();

    return { tmpMesh.m_extend.x * 2.f, tmpMesh.m_extend.y * 2.f, tmpMesh.m_extend.z * 2.f };
}

std::vector<GameObject*> BlenderSceneCustom::Load(std::wstring jsonPath, size tagScene, std::wstring textureFolder, Chunk* p_chunk)
{
    App* p_app = APP;
    Scene* p_scene = SCENE_MANAGER.GetScene(tagScene);

    GameScene* p_test = dynamic_cast<GameScene*>(p_scene);
    Grid* p_grid = nullptr;

    if (p_test != nullptr)
        p_grid = p_test->GetGrid();

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

        GameObject* p_go = HandleGameObjectType(p_scene, name);

        HandleTransform(obj, p_go);

        XMFLOAT3 boundingBox = {};
        Mesh* p_mesh = HandleMesh(obj, name, p_go, boundingBox);
        HandleMaterial(name, p_mesh);
        HandleTexture(obj, p_mesh, textureFolder);
        HandleCollider(name, p_go, boundingBox);
    
        HandleLight(name, p_go);

        HandleTagFloor(name, p_go);
        HandleTagWall(name, p_go);

        HandlePathCheckpoint(p_test, name, p_go);
        HandlePursuitPoint(p_test, name, p_go);

        HandleChunkRoom(p_chunk, p_scene, name, p_go);

        allCreatedObj.push_back(p_go);
    }

    p_chunk->SetActive(true);

    return allCreatedObj;
}

GameObject* BlenderSceneCustom::HandleGameObjectType(Scene* s, std::string name)
{
    GameObject* p_ref = nullptr;

    auto hasName = [&](std::string txt, std::string n) -> bool
        {
            std::regex pattern(txt);
            return std::regex_search(name, pattern);
        };

    if (hasName(Regexword::FIRST_KEY, name))
    {
        Collectible* c = s->CreateGameObject<Collectible>();
        c->SetProperties(ItemListName::FirstKey, ItemListDesc::FirstKey, false);

        p_ref = c;
    }
    else if (hasName(Regexword::FLASHLIGHT, name))
    {
        FlashlightCollectible* f = s->CreateGameObject<FlashlightCollectible>();
        p_ref = f;
    }
    else if (hasName(Regexword::EQUATION_SHEET, name))
    {
        Collectible* c = s->CreateGameObject<Collectible>();
        c->SetProperties(ItemListName::EquationSheet, ItemListDesc::EquationSheet, false);

        p_ref = c;
    }
    else if (hasName(Regexword::LOCKER, name))
    {
        Locker* l = s->CreateGameObject<Locker>();
        p_ref = l;
    }
    else if (hasName(Regexword::SAVE, name))
    {
        SavePoint* sp = s->CreateGameObject<SavePoint>();
        p_ref = sp;
    }
    else if (hasName(Regexword::DOOR, name))
    {
        Door* p_door = s->CreateGameObject<Door>();

        bool right = false;
        if (hasName("rPivot", name))
            right = true;
        
        bool locked = false;
        if (hasName("locked", name))
            locked = true;

        p_door->SetProperties({ 0, 0, 0 }, right, false, XM_PIDIV2, 0.75f, !locked);

        InteractComponent& ic = p_door->GetComponent<InteractComponent>();
        ic.InitInteract([p_door, &ic]()
    	    {
    		    p_door->ToggleDoor();
    	    });

        p_ref = p_door;
        p_ref->m_name = name;
    }
    else if (hasName(Regexword::TRIGGER_PURSUIT, name))
    {
        PursuitTrigger* p = s->CreateGameObject<PursuitTrigger>();
        p_ref = p;
    }
    else if (hasName(Regexword::DIGICODE, name))
    {
        std::smatch match;
        std::regex pattern(Regexword::DIGICODE);
        if (std::regex_search(name, match, pattern))
        {
            DigiCodeObject* dc = s->CreateGameObject<DigiCodeObject>();
            
            if (std::regex_search(name, std::regex("right")))
            {
                dc->m_digiCodeOnX = true;
            }
            else
            {
                dc->m_digiCodeOnX = false;
            }

            dc->InitCode(match[1]);
            dc->m_isActiveCode = true;

            p_ref = dc;
        }
    }
    else if (hasName(Regexword::BATTERY, name))
    {
        Battery* p_battery = s->CreateGameObject<Battery>();
        p_ref = p_battery;
    }
    else if (hasName(Regexword::ENDSCREEN, name))
    {
        EndscreenTrigger* t = s->CreateGameObject<EndscreenTrigger>();
        p_ref = t;
    }
    else
    {
        p_ref = s->CreateGameObject<GameObject>();
        p_ref->m_name = name;
    }

    return p_ref;
}

void BlenderSceneCustom::HandleTransform(nlohmann::json::value_type& obj, GameObject* p_go)
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

void BlenderSceneCustom::HandleMaterial(std::string name, Mesh* p_mesh)
{
    if (p_mesh == nullptr)
        return;

    std::smatch matMatch;
    std::regex patternMat(Regexword::MATERIAL);

    if (std::regex_search(name, matMatch, patternMat))
    {
        std::string numberStr = matMatch[1];
        int number = std::stoi(numberStr);

        p_mesh->SetMaterialTag(number);
    }
}
void BlenderSceneCustom::HandleTexture(nlohmann::json::value_type& obj, Mesh* p_mesh, std::wstring textureFolder)
{
    if (p_mesh == nullptr)
        return;

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

        size_t pos = baseColorTex.find('.');
        if (pos != std::string::npos)
        {
            baseColorTex.erase(pos);
        }

        baseColorTex += std::string(".dds");

        std::wstring pathColor(baseColorTex.begin(), baseColorTex.end());

        int textureOffset = (MAX_TEXTURES - 3) - TexturesLoaded;
        TexturesLoaded++;

        APP->LoadTexture(textureFolder + pathColor, textureOffset, false);
        p_mesh->SetTexture(textureOffset);
    }
}

Mesh* BlenderSceneCustom::HandleMesh(nlohmann::json::value_type& obj, std::string name, GameObject* p_go, XMFLOAT3& boundingBox)
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

    boundingBox = GetBoundingBox(vertices, indices);

    std::regex noMesh(Regexword::NO_MESH);
    if (std::regex_search(name, noMesh))
        return nullptr;

    p_go->AddComponent(MeshRendererComponent());
    MeshRendererComponent& meshComp = p_go->GetComponent<MeshRendererComponent>();
    Mesh* p_mesh = meshComp.CreateMeshInstance();
    MakeCustomGeometry(vertices, indices, p_mesh);

    return p_mesh;
}

void BlenderSceneCustom::HandleCollider(std::string name, GameObject* p_go, const XMFLOAT3& boundingBox)
{
    std::regex patternCollider(Regexword::COLLIDER);
    if (std::regex_search(name, patternCollider))
    {
        p_go->AddBoxCollider((int)TagCollision::_Decor);

        if (p_go->HasComponent<MeshRendererComponent>() == false)
        {
            p_go->GetComponent<BoxColliderComponent>().CreateManualBox(boundingBox, false);
        }
        else
        {
            std::regex patternNoPhysic(Regexword::NOREPULSE);

            if (std::regex_search(name, patternNoPhysic))
                return;

            PhysicComponent pc;
            pc.m_bounciness = 0.f;
            pc.m_isKinematic = true;
            pc.m_useGravity = false;

            p_go->AddComponent(pc);
        }
    }
}
void BlenderSceneCustom::HandleTagFloor(std::string name, GameObject* p_go)
{
    std::regex patternTagFloor(Regexword::FLOOR);

    if (std::regex_search(name, patternTagFloor))
    {
        p_go->SetTag((int)TagObject::_Floor);
    }
}

void BlenderSceneCustom::HandleTagWall(std::string name, GameObject* p_go)
{
    std::regex patternTagWall(Regexword::WALL);

    if (std::regex_search(name, patternTagWall))
    {
        p_go->SetTag((int)TagObject::_WallPaper);
    }
}

void BlenderSceneCustom::HandleLight(std::string name, GameObject* p_go)
{
    std::regex patternLight(Regexword::LIGHT);

    if (std::regex_search(name, patternLight))
    {
        LightComponent lc;
        Light* p_light = lc.CreateLightInstance();
        p_light->SetToPointLight(1.f, { 0, 0, 0 }, 5.f, { 1, 1, 1 });
        p_go->AddComponent(lc);
    }
}

void BlenderSceneCustom::HandlePathCheckpoint(GameScene* gs, std::string name, GameObject*& p_go)
{
    if (gs == nullptr)
        return;

    std::regex patternCP(Regexword::CHECKPOINT);
    if (std::regex_search(name, patternCP) == false)
        return;

    gs->m_pathCPpositions.push_back(p_go->mp_transform->GetWorldPosition());
    p_go->Destroy();
    p_go = nullptr;
}

void BlenderSceneCustom::HandlePursuitPoint(GameScene* gs, std::string name, GameObject* p_go)
{
    if (gs == nullptr)
        return;

    std::smatch match;
    std::regex patternPP(Regexword::PURSUITPOINT);
    if (std::regex_search(name, match, patternPP) == false)
        return;

    int nb = std::stoi(match[1]);

    gs->m_pursuitPoints.push_back(PursuitPoint{ p_go->mp_transform->GetWorldPosition(), nb});
    p_go->Destroy();
    p_go = nullptr;
}

void BlenderSceneCustom::HandleChunkRoom(Chunk* p_chunk, Scene* p_scene, std::string name, GameObject* p_go)
{
    if (p_go == nullptr)
        return;

    std::regex patternFirstDoor("locked0");
    if (std::regex_search(name, patternFirstDoor))
    {
        Door* p_door = (Door*)p_go;

        p_door->GetComponent<InteractComponent>().InitInteract([p_door]
            {
                if (InventoryScene* is = (InventoryScene*)SCENE_MANAGER.GetScene((int)TagScene::_Inventory))
                {
                    if (is->HasItem(ItemListName::FirstKey))
                    {
                        AudioManager::PlaySound("DoorOpenLocked");
                        is->UseItem(ItemListName::FirstKey);
                        p_door->CanOpen(true);
                    }
   
                    p_door->ToggleDoor();
                }
            
            });
    }

    std::smatch roomMatch;
    std::regex patternRoom(Regexword::ROOM);

    if (std::regex_search(name, roomMatch, patternRoom))
    {
        std::string numberStr = roomMatch[1];
        int number = std::stoi(numberStr);

        Room* p_room = p_chunk->GetRoom(number);

        if (p_room == nullptr)
            p_room = p_chunk->AddRoom(number, new Room(p_scene));

        if (p_go->GetTag() == (int)TagObject::_Floor)
            p_chunk->SetChunkIndex(p_go);

        p_room->AddGameObject(p_go);
    }
}
