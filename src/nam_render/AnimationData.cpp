#include "pch.h"
#include "AnimationData.h"

namespace nam
{
	float Bone::GetStartTime() const
	{
		// Keyframes are sorted by time, so first keyframe gives start time.
		return m_keyframes.front().m_timePos;
	}
	float Bone::GetEndTime() const
	{
		// Keyframes are sorted by time, so last keyframe gives end time.
		return  m_keyframes.back().m_timePos;
	}
	void Bone::Interpolate(float t, DirectX::XMFLOAT4X4& M) const
	{
		if (t <= m_keyframes.front().m_timePos)
		{
			XMVECTOR S = XMLoadFloat3(&m_keyframes.front().m_sca);
			XMVECTOR P = XMLoadFloat3(&m_keyframes.front().m_transla);
			XMVECTOR Q = XMLoadFloat4(&m_keyframes.front().m_rotQuat);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else if (t >= m_keyframes.back().m_timePos)
		{
			XMVECTOR S = XMLoadFloat3(&m_keyframes.back().m_sca);
			XMVECTOR P = XMLoadFloat3(&m_keyframes.back().m_transla);
			XMVECTOR Q = XMLoadFloat4(&m_keyframes.back().m_rotQuat);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else
		{
			for (UINT i = 0; i < m_keyframes.size() - 1; ++i)
			{
				if (t >= m_keyframes[i].m_timePos && t <= m_keyframes[i + 1].m_timePos)
				{
					float lerpPercent = (t - m_keyframes[i].m_timePos) / (m_keyframes[i + 1].m_timePos - m_keyframes[i].m_timePos);

					XMVECTOR s0 = XMLoadFloat3(&m_keyframes[i].m_sca);
					XMVECTOR s1 = XMLoadFloat3(&m_keyframes[i + 1].m_sca);

					XMVECTOR p0 = XMLoadFloat3(&m_keyframes[i].m_transla);
					XMVECTOR p1 = XMLoadFloat3(&m_keyframes[i + 1].m_transla);

					XMVECTOR q0 = XMLoadFloat4(&m_keyframes[i].m_rotQuat);
					XMVECTOR q1 = XMLoadFloat4(&m_keyframes[i + 1].m_rotQuat);

					XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
					XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
					XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

					XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
					XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

					break;
				}
			}
		}
	}

	////////////////

	float Animation::GetClipStartTime() const
	{
		// Find smallest start time over all bones in this clip.
		float t = MathHelper::Infinity;
		for (UINT i = 0; i < m_bones.size(); ++i)
		{
			t = MathHelper::Min(t, m_bones[i].GetStartTime());
		}

		return t;
	}

	float Animation::GetClipEndTime() const
	{	
		// Find largest end time over all bones in this clip.
		float t = 0.f;
		for (UINT i = 0; i < m_bones.size(); ++i)
		{
			t = MathHelper::Max(t, m_bones[i].GetEndTime());
		}

		return t;
	}

	void Animation::Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms) const
	{
		for (UINT i = 0; i < m_bones.size(); ++i)
		{
			m_bones[i].Interpolate(t, boneTransforms[i]);
		}
	}

	///////////////

	UINT MeshAnimationData::BoneCount() const
	{
		return (UINT)m_boneHierarchy.size();
	}

	float MeshAnimationData::GetClipStartTime(const std::string& clipName) const
	{
		auto clip = m_animations.find(clipName);
		return clip->second.GetClipStartTime();
	}

	float MeshAnimationData::GetClipEndTime(const std::string& clipName) const
	{
		auto clip = m_animations.find(clipName);
		return clip->second.GetClipEndTime();
	}

	void MeshAnimationData::Set(std::vector<int>& boneHierarchy, std::vector<DirectX::XMFLOAT4X4>& boneOffsets, std::unordered_map<std::string, Animation>& animations)
	{
		m_boneHierarchy = boneHierarchy;
		m_boneOffsets = boneOffsets;
		m_animations = animations;
	}

	void MeshAnimationData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<DirectX::XMFLOAT4X4>& finalTransforms) const
	{

		size numBones = m_boneOffsets.size();
		std::vector<XMFLOAT4X4> toParentTransforms(numBones);

		// Interpolate all the bones of this clip at the given time instance.
		auto clip = m_animations.find(clipName);
		clip->second.Interpolate(timePos, toParentTransforms);


		//
		// Traverse the hierarchy and transform all the bones to the root space.
		//

		std::vector<XMFLOAT4X4> toRootTransforms(numBones);

		// The root bone has index 0.  The root bone has no parent, so its toRootTransform
		// is just its local bone transform.
		toRootTransforms[0] = toParentTransforms[0];

		// Now find the toRootTransform of the children.
		for (UINT i = 1; i < numBones; ++i)
		{
			XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

			int parentIndex = m_boneHierarchy[i];
			XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

			XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

			XMStoreFloat4x4(&toRootTransforms[i], toRoot);
		}

		// Premultiply by the bone offset transform to get the final transform.
		for (UINT i = 0; i < numBones; ++i)
		{
			XMMATRIX offset = XMLoadFloat4x4(&m_boneOffsets[i]);
			XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
			XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
			XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
		}
	}
}