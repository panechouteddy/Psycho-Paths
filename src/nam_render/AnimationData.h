#pragma once

//Source : DX12 Book-master Chapter 23 Character Animation

namespace nam
{
	struct Keyframe
	{
		float m_timePos = 0.f;

		DirectX::XMFLOAT3 m_transla = { 0, 0, 0 };
		DirectX::XMFLOAT3 m_sca = { 1, 1, 1 };
		DirectX::XMFLOAT4 m_rotQuat = { 0, 0, 0, 1 };
	};

	struct Bone
	{
		float GetStartTime()const;
		float GetEndTime()const;

		void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

		Vector<Keyframe> m_keyframes;
	};

	struct Animation
	{
		float GetClipStartTime()const;
		float GetClipEndTime()const;

		void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

		std::vector<Bone> m_bones;
	};

	struct MeshAnimationData
	{
	public:
		UINT BoneCount()const;

		float GetClipStartTime(const std::string& clipName)const;
		float GetClipEndTime(const std::string& clipName)const;

		void Set(
			std::vector<int>& boneHierarchy,
			std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
			std::unordered_map<std::string, Animation>& animations
			);

		// In a real project, you'd want to cache the result if there was a chance
		// that you were calling this several times with the same clipName at 
		// the same timePos.
		void GetFinalTransforms(const std::string& clipName, float timePos,
			std::vector<DirectX::XMFLOAT4X4>& finalTransforms)const;

	private:
		// Gives parentIndex of ith bone.
		std::vector<int> m_boneHierarchy;
		std::vector<DirectX::XMFLOAT4X4> m_boneOffsets;
		std::unordered_map<std::string, Animation> m_animations;
	};

	struct MeshAnimationModel
	{
		MeshAnimationData* mp_meshAnimData = nullptr;
		std::vector<DirectX::XMFLOAT4X4> m_finalTransforms;
		std::string m_clipName;
		float m_timePos = 0.0f;

		// Called every frame and increments the time position, interpolates the 
		// animations for each bone based on the current animation clip, and 
		// generates the final transforms which are ultimately set to the effect
		// for processing in the vertex shader.
		void ResizeTransformsVector()
		{
			m_finalTransforms.resize(mp_meshAnimData->BoneCount());
		}

		void UpdateAnimation(float dt)
		{
			m_timePos += dt;

			// Loop animation
			if (m_timePos > mp_meshAnimData->GetClipEndTime(m_clipName))
				m_timePos = 0.0f;

			// Compute the final transforms for this time position.
			mp_meshAnimData->GetFinalTransforms(m_clipName, m_timePos, m_finalTransforms);
		}
	};
}
