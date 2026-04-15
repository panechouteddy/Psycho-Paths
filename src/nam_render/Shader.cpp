#include "pch.h"
#include "Shader.h"

namespace nam
{
	Shader::~Shader()
	{
	}

	void Shader::Compile(const std::string& name, std::wstring path, bool isCS)
	{
		m_name = name;

		if (isCS)
		{
			mp_csByteCode = d3dUtil::CompileShader(path, nullptr, "CS", "cs_5_0");
			return;
		}

		mp_vsByteCode = d3dUtil::CompileShader(path, nullptr, "VS", "vs_5_0");
		mp_psByteCode = d3dUtil::CompileShader(path, nullptr, "PS", "ps_5_0");

	}

	Shader* Shader::Create(const std::string& name, std::wstring path, bool isCS)
	{
		Shader* p_shader = new Shader();
		p_shader->Compile(name, path, isCS);

		return p_shader;
	}
}
