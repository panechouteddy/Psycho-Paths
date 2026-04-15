#include "pch.h"
#include "GlbParser.h"
#include "Mesh.h"

#include "AnimationData.h"

namespace nam
{
	bool GlbParser::LoadGlb(Mesh* p_target, std::wstring path)
	{
		std::ifstream file(path, std::ios::binary);

		if (!file.is_open())
			return false;

		GLBHeader header = {};
		file.read((char*)&header, sizeof(header));

		if (header.magic != 0x46546C67 || header.version != 2)
		{
			_ASSERT(false && "Not a Glb !");
			return false;
		}

		ChunkHeader jsonChunk = {};
		file.read((char*)&jsonChunk, 8);

		if (jsonChunk.type != 0x4E4F534A)
			return false;

		std::vector<char> json(jsonChunk.length);
		file.read(json.data(), jsonChunk.length);

		ChunkHeader binChunk = {};
		file.read((char*)&binChunk, 8);
		std::vector<u8> bin(binChunk.length);
		file.read((char*)bin.data(), binChunk.length);

		std::string jsonStr(json.begin(), json.end());
		nlohmann::json doc;

		try {
			doc = nlohmann::json::parse(jsonStr);
		}
		catch (const std::exception& e) {
			Print("Error JSON: ", e.what());
			return false;
		}

		HandleMesh(doc, bin, p_target);
		HandleAnimations(doc, bin, p_target);
		return true;
	}

	void GlbParser::HandleMesh(nlohmann::json& doc, std::vector<u8> bin, Mesh* p_target)
	{
		auto& mesh = doc["meshes"][0];
		auto& primitive = mesh["primitives"][0];

		//indices
		int idxAccessorIndex = primitive["indices"].get<int>();
		auto& idxAccessor = doc["accessors"][idxAccessorIndex];
		int idxBVIndex = idxAccessor["bufferView"].get<int>();
		auto& idxBV = doc["bufferViews"][idxBVIndex];
		size idxOffset = idxBV.value("byteOffset", 0) + idxAccessor.value("byteOffset", 0);
		size idxCount = idxAccessor["count"];

		int compType = idxAccessor["componentType"];
		if (compType == 5123) // uint16
		{
			const u8* data = bin.data() + idxOffset;

			for (size_t i = 0; i < idxCount; i++)
			{
				u16 v;
				memcpy(&v, data + i * 2, 2);
				p_target->m_indices.push_back((u32)v);
			}
		}
		else if (compType == 5125) // uint32
		{
			const u8* data = bin.data() + idxOffset;

			for (size_t i = 0; i < idxCount; i++)
			{
				u32 v;
				memcpy(&v, data + i * 2, 2);
				p_target->m_indices.push_back(v);
			}
		}

		auto& posAccessor = doc["accessors"][0];
		size posOffset = doc["bufferViews"][posAccessor["bufferView"].get<int>()]["byteOffset"]
			+ posAccessor.value("byteOffset", 0);

		auto& normalAccessor = doc["accessors"][1];
		size normOffset = doc["bufferViews"][normalAccessor["bufferView"].get<int>()]["byteOffset"]
			+ normalAccessor.value("byteOffset", 0);

		auto& uvAccessor = doc["accessors"][2];
		size uvOffset = doc["bufferViews"][uvAccessor["bufferView"].get<int>()]["byteOffset"]
			+ uvAccessor.value("byteOffset", 0);

		auto& jointsAccessor = doc["accessors"][3];
		size jointsOffset = doc["bufferViews"][jointsAccessor["bufferView"].get<int>()]["byteOffset"]
			+ jointsAccessor.value("byteOffset", 0);

		auto& weightsAccessor = doc["accessors"][4];
		size weightOffset = doc["bufferViews"][weightsAccessor["bufferView"].get<int>()]["byteOffset"]
			+ weightsAccessor.value("byteOffset", 0);

		const float* pos = (float*)(bin.data() + posOffset);
		const float* normal = (float*)(bin.data() + normOffset);
		const float* uv = (float*)(bin.data() + uvOffset);
		const u16* joints = (u16*)(bin.data() + jointsOffset);
		const float* weights = (float*)(bin.data() + weightOffset);

		for (size i = 0; i < posAccessor["count"]; i++)
		{
			XMFLOAT3 p
			(
				pos[i * 3 + 0],
				pos[i * 3 + 1],
				pos[i * 3 + 2]
			);

			XMFLOAT3 n
			(
				normal[i * 3 + 0],
				normal[i * 3 + 1],
				normal[i * 3 + 2]
			);

			XMFLOAT2 t
			(
				uv[i * 2 + 0],
				uv[i * 2 + 1]
			);

			XMINT4 j
			(
				joints[i * 4 + 0],
				joints[i * 4 + 1],
				joints[i * 4 + 2],
				joints[i * 4 + 3]
			);

			XMFLOAT4 w
			(
				weights[i * 4 + 0],
				weights[i * 4 + 1],
				weights[i * 4 + 2],
				weights[i * 4 + 3]
			);

			p_target->m_vertices.push_back(Vertex{ p, {1, 1, 1, 1}, t, n, j, w});
		}
	}

	void GlbParser::HandleAnimations(nlohmann::json& doc, std::vector<u8> bin, Mesh* p_target)
	{
		if (!doc.contains("animations"))
			return;

		MeshAnimationModel* p_final = new MeshAnimationModel();
		MeshAnimationData* p_animData = new MeshAnimationData(); // TO complete after

		auto& animations = doc["animations"];

		//Skinning
		auto& skin = doc["skins"][0];
		auto& joints = skin["joints"];

		const size boneCount = joints.size();
		UnMap<int, int> nodeToBone;

		for (int i = 0; i < joints.size(); i++)
		{
			int nodeIndex = joints[i];
			nodeToBone[nodeIndex] = i;
		}

		Vector<int> boneHierarchy;
		boneHierarchy.resize(joints.size(), -1);
		auto& nodes = doc["nodes"];

		for (int i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i].contains("children"))
				continue;

			for (auto& child : nodes[i]["children"])
			{
				int childNode = child.get<int>();

				if (nodeToBone.count(childNode))
				{
					int childBone = nodeToBone[childNode];
					int parentBone = nodeToBone[i];

					boneHierarchy[childBone] = parentBone;
				}
			}
		}

		UnMap<std::string, Animation> animationsMap;

		//Animations
		for (auto& anim : doc["animations"])
		{
			auto& samplers = anim["samplers"];
			Vector<SamplerData> samplersData;
			UnMap<int, Channel> channelsPerBone;

			Animation animation;
			animation.m_bones.resize(boneCount);

			//samplers
			samplersData.resize(samplers.size());
			for (int i = 0; i < samplers.size(); i++)
			{
				auto& sampler = samplers[i];

				//Input
				auto& inputAccessor = doc["accessors"][sampler["input"].get<int>()];
				int keyCount = inputAccessor["count"].get<int>();
				size_t inputOffset =
					doc["bufferViews"][inputAccessor["bufferView"].get<int>()]["byteOffset"]
					+ inputAccessor.value("byteOffset", 0);

				const float* times = (float*)(bin.data() + inputOffset);
				samplersData[i].times.assign(times, times + keyCount);

				//Output
				auto& outputAccessor = doc["accessors"][sampler["output"].get<int>()];
				std::string type = outputAccessor["type"];
				size_t outputOffset =
					doc["bufferViews"][outputAccessor["bufferView"].get<int>()]["byteOffset"]
					+ outputAccessor.value("byteOffset", 0);

				int comp = (type == "SCALAR") ? 1 : (type == "VEC3") ? 3 : 4;
				samplersData[i].stride = comp;

				const float* values = (float*)(bin.data() + outputOffset);
				samplersData[i].values.assign(values, values + keyCount * comp);
			}


			for (auto& channel : anim["channels"])
			{
				int node = channel["target"]["node"];
				std::string path = channel["target"]["path"];
				int sampler = channel["sampler"];

				int bone = nodeToBone[node];
				auto& c = channelsPerBone[bone];

				if (path == "translation")
					c.translationSampler = sampler;
				else if (path == "rotation")
					c.rotationSampler = sampler;
				else if (path == "scale")
					c.scaleSampler = sampler;
			}

			std::vector<TempBoneAnim> tempBones;
			tempBones.resize(boneCount);

			for (auto& [boneIndex, channel] : channelsPerBone)
			{
				TempBoneAnim& b = tempBones[boneIndex];

				if (channel.translationSampler != -1)
					b.translation = samplersData[channel.translationSampler];

				if (channel.rotationSampler != -1)
					b.rotation = samplersData[channel.rotationSampler];

				if (channel.scaleSampler != -1)
					b.scale = samplersData[channel.scaleSampler];
			}

			for (int b = 0; b < boneCount; b++)
			{
				TempBoneAnim& tb = tempBones[b];
				Bone& bone = animation.m_bones[b];

				std::set<float> times;
				for (float t : tb.translation.times) times.insert(t);
				for (float t : tb.rotation.times) times.insert(t);
				for (float t : tb.scale.times) times.insert(t);

				for (float t : times)
				{
					Keyframe k;
					k.m_timePos = t;

					// Translation
					if (!tb.translation.times.empty())
					{
						auto it = std::lower_bound(tb.translation.times.begin(), tb.translation.times.end(), t);
						if (it != tb.translation.times.end() && *it == t)
						{
							size_t idx = it - tb.translation.times.begin();
							k.m_transla.x = tb.translation.values[idx * 3 + 0];
							k.m_transla.y = tb.translation.values[idx * 3 + 1];
							k.m_transla.z = tb.translation.values[idx * 3 + 2];
						}
					}

					// Rotation (quaternion)
					if (!tb.rotation.times.empty())
					{
						auto it = std::lower_bound(tb.rotation.times.begin(), tb.rotation.times.end(), t);
						if (it != tb.rotation.times.end() && *it == t)
						{
							size_t idx = it - tb.rotation.times.begin();
							k.m_rotQuat.x = tb.rotation.values[idx * 4 + 0];
							k.m_rotQuat.y = tb.rotation.values[idx * 4 + 1];
							k.m_rotQuat.z = tb.rotation.values[idx * 4 + 2];
							k.m_rotQuat.w = tb.rotation.values[idx * 4 + 3];
						}
					}

					// Scale
					if (!tb.scale.times.empty())
					{
						auto it = std::lower_bound(tb.scale.times.begin(), tb.scale.times.end(), t);
						if (it != tb.scale.times.end() && *it == t)
						{
							size_t idx = it - tb.scale.times.begin();
							k.m_sca.x = tb.scale.values[idx * 3 + 0];
							k.m_sca.y = tb.scale.values[idx * 3 + 1];
							k.m_sca.z = tb.scale.values[idx * 3 + 2];
						}
					}

					bone.m_keyframes.push_back(k);
				}

				std::sort(bone.m_keyframes.begin(), bone.m_keyframes.end(),
					[](const Keyframe& a, const Keyframe& b) { return a.m_timePos < b.m_timePos; });
			}

			animationsMap[anim["name"].get<std::string>()] = animation;
		}

		auto& invBindAccessor = doc["accessors"][skin["inverseBindMatrices"].get<int>()];
		size_t invBindOffset = doc["bufferViews"][invBindAccessor["bufferView"].get<int>()]["byteOffset"]
			+ invBindAccessor.value("byteOffset", 0);

		int invBindCount = invBindAccessor["count"].get<int>();
		const float* invBindData = (float*)(bin.data() + invBindOffset);

		std::vector<DirectX::XMFLOAT4X4> boneOffsets(invBindCount);
		for (int i = 0; i < invBindCount; ++i)
		{
			DirectX::XMFLOAT4X4 mat;
			memcpy(&mat, invBindData + i * 16, sizeof(float) * 16);
			boneOffsets[i] = mat;
		}

		p_animData->Set(boneHierarchy, boneOffsets, animationsMap);

		p_final->mp_meshAnimData = p_animData;
		p_final->ResizeTransformsVector();

		p_target->SetAnimData(p_final);
	}
}