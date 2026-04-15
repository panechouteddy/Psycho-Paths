#pragma once
#include <json.hpp>

namespace nam
{
	struct Mesh;

	struct GLBHeader
	{
		u32 magic;
		u32 version;
		u32 length;
	};

	struct ChunkHeader
	{
		u32 length;
		u32 type;
	};

	struct SamplerData
	{
		std::vector<float> times;
		std::vector<float> values;
		int stride = 0; // 1, 3 or 4
	};

	struct Channel
	{
		int translationSampler = -1;
		int rotationSampler = -1;
		int scaleSampler = -1;
	};

	struct TempBoneAnim
	{
		SamplerData translation;
		SamplerData rotation;
		SamplerData scale;
	};

	class GlbParser
	{
	public:
		static bool LoadGlb(Mesh* p_target, std::wstring path);

	private:
		static void HandleMesh(nlohmann::json& doc, std::vector<u8> bin, Mesh* p_target);
		static void HandleAnimations(nlohmann::json& doc, std::vector<u8> bin, Mesh* p_target);
	};
}

