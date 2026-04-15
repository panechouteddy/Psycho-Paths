#include "pch.h"
#include "ObjParser.h"
#include "Vertex.h"

#include "Mesh.h"

#include <fstream>
#include <regex>

#include <json.hpp>

using namespace DirectX;

namespace nam
{
	bool ObjParser::LoadObj(Mesh* p_target, std::wstring path)
	{
        std::ifstream file(path);

        if (!file.is_open())
            return false;

        Vector<XMFLOAT3> positions;
        Vector<XMFLOAT3> normals;
        Vector<XMFLOAT2> uvs;
        XMFLOAT4 meshColor = p_target->m_color;

        std::string line;
        while (std::getline(file, line)) //Read until no line is found
        {
            std::istringstream ss(line);
            std::string prefix;

            ss >> prefix; //Read the following word and fill the variable

            if (prefix == "v") // position
            {
                float x, y, z;
                ss >> x >> y >> z;
                positions.push_back({ x, y, z });
            }
            else if (prefix == "vt") // uv
            {
                float u, v;
                ss >> u >> v;
                uvs.push_back({ u, 1.f - v });
            }
            else if (prefix == "vn") // normal
            {
                float x, y, z;
                ss >> x >> y >> z;
                normals.push_back({ x, y, z });
            }
            else if (prefix == "f")
            {
                Vector<std::string> tokens;
                std::string token; // vi/vti/ni -> indices in the tabs Positions/UV/Normals

                while (ss >> token)
                    tokens.push_back(token);

                auto parseFace = [&](const std::string& token) -> Vertex
                    {
                        int vertIndex = 0, vertTextureIndex = 0, normalIndex = 0;
                        size firstSlash = token.find('/');
                        size secondSlash = token.find('/', firstSlash + 1);

                        if (firstSlash != std::string::npos)
                        {
                            vertIndex = std::stoi(token.substr(0, firstSlash)) - 1;

                            if (secondSlash != std::string::npos)
                            {
                                std::string uvStr = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
                                if (!uvStr.empty())
                                    vertTextureIndex = std::stoi(uvStr) - 1;

                                normalIndex = std::stoi(token.substr(secondSlash + 1)) - 1;
                            }
                        }

                        Vertex vert = {};
                        vert.m_pos = positions[vertIndex];
                        vert.m_uv = (uvs.empty()) ? XMFLOAT2(0, 0) : uvs[vertTextureIndex];
                        vert.m_normal = (normals.empty()) ? XMFLOAT3(0, 0, 0) : normals[normalIndex];
                        vert.m_color = meshColor;
                        return vert;
                    };

                if (tokens.size() == 3)
                {
                    u32 start = (u32)p_target->m_vertices.size();
                    p_target->m_vertices.push_back(parseFace(tokens[0]));
                    p_target->m_vertices.push_back(parseFace(tokens[1]));
                    p_target->m_vertices.push_back(parseFace(tokens[2]));

                    p_target->m_indices.push_back(start);
                    p_target->m_indices.push_back(start + 1);
                    p_target->m_indices.push_back(start + 2);
                }
                else if (tokens.size() == 4) // quad
                {
                    u32 start = (u32)p_target->m_vertices.size();

                    p_target->m_vertices.push_back(parseFace(tokens[0]));
                    p_target->m_vertices.push_back(parseFace(tokens[1]));
                    p_target->m_vertices.push_back(parseFace(tokens[2]));
                    p_target->m_vertices.push_back(parseFace(tokens[3]));

                    //tri 1
                    p_target->m_indices.push_back(start);
                    p_target->m_indices.push_back(start + 1);
                    p_target->m_indices.push_back(start + 2);

                    //tri 2
                    p_target->m_indices.push_back(start);
                    p_target->m_indices.push_back(start + 2);
                    p_target->m_indices.push_back(start + 3);
                }
            }
        }

        return true;
	}
}