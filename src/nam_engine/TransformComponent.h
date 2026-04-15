#pragma once
namespace nam
{
	enum LocalAxis
	{
		_Forward,
		_Up,
		_Right,
	};

	struct TransformComponent
	{
	private:
		DirectX::XMFLOAT3 m_localPosition = { 0, 0, 0 };
		DirectX::XMFLOAT3 m_localScale = { 1, 1, 1 };
		DirectX::XMFLOAT4 m_localRotation = { 0, 0, 0, 1 };

		DirectX::XMFLOAT3 m_worldPosition = { 0, 0, 0 };
		DirectX::XMFLOAT3 m_worldScale = { 1, 1, 1 };
		DirectX::XMFLOAT4 m_worldRotation = { 0, 0, 0, 1 };

		DirectX::XMFLOAT3 m_forward = { 0, 0, 1 };
		DirectX::XMFLOAT3 m_right = { 1, 0, 0 };
		DirectX::XMFLOAT3 m_up = { 0, 1, 0 };

		DirectX::XMFLOAT4X4 m_localMatrix;
		DirectX::XMFLOAT4X4 m_worldMatrix;

		TransformComponent* m_parent = nullptr;
		Vector<TransformComponent*> m_children;

		bool m_localDirty = true;
		bool m_worldDirty = true;
		bool m_vectorsDirty = true;

	public:
		TransformComponent();
		~TransformComponent();

		void Reset();

		void SetLocalPosition(const DirectX::XMFLOAT3& pos);
		void TranslateLocal(const DirectX::XMFLOAT3& delta);
		DirectX::XMFLOAT3 GetLocalPosition() const;

		void SetWorldPosition(const DirectX::XMFLOAT3& pos);
		void TranslateWorld(const DirectX::XMFLOAT3& delta);
		DirectX::XMFLOAT3 GetWorldPosition();

		void SetLocalScale(const DirectX::XMFLOAT3& scale);
		void ScaleLocal(const DirectX::XMFLOAT3& factor);
		DirectX::XMFLOAT3 GetLocalScale() const;

		void SetWorldScale(const DirectX::XMFLOAT3& scale);
		void ScaleWorld(const DirectX::XMFLOAT3& factor);
		DirectX::XMFLOAT3 GetWorldScale();

		void SetLocalRotation(const DirectX::XMFLOAT4& quat);
		void RotateLocal(const DirectX::XMFLOAT4& quat);
		void RotateLocalAroundAxis(const DirectX::XMFLOAT3& axis, float angle);
		DirectX::XMFLOAT4 GetLocalRotation() const;

		void SetWorldRotation(const DirectX::XMFLOAT4& quat);
		void RotateWorld(const DirectX::XMFLOAT4& quat);
		void RotateWorldAroundAxis(const DirectX::XMFLOAT3& axis, float angle);
		DirectX::XMFLOAT4 GetWorldRotation();

		void SetLocalYPR(float yaw, float pitch, float roll);
		void RotateLocalYPR(float yaw, float pitch, float roll);

		void SetLocalYPR(const DirectX::XMFLOAT3& ypr);
		void RotateLocalYPR(const DirectX::XMFLOAT3& ypr);
		DirectX::XMFLOAT3 GetLocalYPR() const;

		void SetWorldYPR(float yaw, float pitch, float roll);
		void RotateWorldYPR(float yaw, float pitch, float roll);

		void SetWorldYPR(const DirectX::XMFLOAT3& ypr);
		void RotateWorldYPR(const DirectX::XMFLOAT3& ypr);
		DirectX::XMFLOAT3 GetWorldYPR() const;

		void MoveLocal(float distance, LocalAxis axis, Array<bool, 3> applyAxis = Array<bool, 3>{ true, true, true });
		void MoveWorld(float distance, LocalAxis axis, Array<bool, 3> applyAxis = Array<bool, 3>{ true, true, true });

		void MoveLocalTowards(const DirectX::XMFLOAT3& target, float distance);
		void MoveWorldTowards(const DirectX::XMFLOAT3& target, float distance);

		void LookAtLocal(const DirectX::XMFLOAT3& target);
		void LookAtLocal(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
		void LookAtWorld(const DirectX::XMFLOAT3& target);
		void LookAtWorld(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

		void LookToLocal(const DirectX::XMFLOAT3& direction);
		void LookToLocal(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up);
		void LookToWorld(const DirectX::XMFLOAT3& direction);
		void LookToWorld(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up);

		void RotateAroundLocal(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& axis, float angle);
		void RotateAroundWorld(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& axis, float angle);

		//Get Forward, Up or Right
		const DirectX::XMFLOAT3& GetLocalAxis(LocalAxis axis);

		const DirectX::XMFLOAT4X4& GetLocalMatrix();
		const DirectX::XMFLOAT4X4& GetWorldMatrix();

		const DirectX::XMFLOAT4X4 GetWorldMatrixTranspose();
		const DirectX::XMFLOAT4X4 GetInverseWorldMatrixTranspose();

		DirectX::XMFLOAT3 TransformPoint(const DirectX::XMFLOAT3& point);

		void SetParent(TransformComponent* parent);
		TransformComponent* GetParent() const;
		void AddChild(TransformComponent* child);
		void RemoveChild(TransformComponent* child);
		const Vector<TransformComponent*>& GetChildren() const;

		void UpdateMatrices();
		void UpdateWorldData();
		void UpdateDirectionVectors();

	private:
		void SetLocalDirty();
		void SetWorldDirty();
		void SetVectorsDirty();

		static DirectX::XMFLOAT4 QuaternionFromYPR(float yaw, float pitch, float roll);
		static void YPRFromQuaternion(const DirectX::XMFLOAT4& quat, float& yaw, float& pitch, float& roll);
		static DirectX::XMFLOAT4 QuaternionMultiply(const DirectX::XMFLOAT4& q1, const DirectX::XMFLOAT4& q2);
		static DirectX::XMFLOAT3 QuaternionRotateVector(const DirectX::XMFLOAT4& quat, const DirectX::XMFLOAT3& vec);
	};
}