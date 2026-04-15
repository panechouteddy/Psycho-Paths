#include "pch.h"
#include "FontManager.h"

static std::string GetValue(const std::string& token)
{
	auto pos = token.find('=');
	if (pos == std::string::npos) return "";

	return token.substr(pos + 1);
}

namespace nam
{
    UnMap<std::string, Font> FontManager::m_fonts = {};

	void FontManager::LoadFnt(std::string name, std::string path)
	{
        if (path.ends_with(".fnt") == false)
        {
            _ASSERT(false && "You need to read a .fnt file !");
            return;
        }

        Font font;

        std::ifstream file(path);

        if (file.is_open() == false)
        {
            _ASSERT(false && ".fnt found !");
            return;
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            std::istringstream ss(line);
            std::string type;
            ss >> type;

            if (type == "common")
            {
                std::string token;

                while (ss >> token)
                {
                    if (token.find("lineHeight=") == 0)
                        font.m_lineHeight = std::stof(GetValue(token));

                    if (token.find("scaleW=") == 0)
                        font.m_scaleW = std::stof(GetValue(token));

                    if (token.find("scaleH=") == 0)
                        font.m_scaleH = std::stof(GetValue(token));
                }
            }

            if (type == "char")
            {
                Glyph g = {};
                int id = 0;

                std::string token;

                while (ss >> token)
                {
                    if (token.find("id=") == 0)
                        id = std::stoi(GetValue(token));

                    else if (token.find("x=") == 0)
                        g.u0 = std::stof(GetValue(token));

                    else if (token.find("y=") == 0)
                        g.v0 = std::stof(GetValue(token));

                    else if (token.find("width=") == 0)
                        g.width = std::stof(GetValue(token));

                    else if (token.find("height=") == 0)
                        g.height = std::stof(GetValue(token));

                    else if (token.find("xoffset=") == 0)
                        g.xOffset = std::stof(GetValue(token));

                    else if (token.find("yoffset=") == 0)
                        g.yOffset = std::stof(GetValue(token));

                    else if (token.find("xadvance=") == 0)
                        g.xAdvance = std::stof(GetValue(token));
                }

                g.u1 = g.u0 + g.width;
                g.v1 = g.v0 + g.height;

                //0 to 1
                g.u0 /= font.m_scaleW;
                g.u1 /= font.m_scaleW;

                g.v0 /= font.m_scaleH;
                g.v1 /= font.m_scaleH;

                font.m_glyphs[id] = g;
            }
        }

        m_fonts[name] = font;
	}

	Font* FontManager::GetFont(std::string name)
	{
        auto it = m_fonts.find(name);

        if (it == m_fonts.end())
        {
            _ASSERT(false && "Font not found !");
            return nullptr;
        }
 
        return &it->second;
	}
}
