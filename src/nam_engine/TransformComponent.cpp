#include "pch.h"
#include "TransformComponent.h"

using namespace DirectX;

namespace nam
{
    TransformComponent::TransformComponent() { Reset(); };
    TransformComponent::~TransformComponent() {};

    void TransformComponent::Reset()
    {
        m_localPosition = { 0.f, 0.f, 0.f };
        m_localScale = { 1.f, 1.f, 1.f };
        m_localRotation = { 0.f, 0.f, 0.f, 1.f };

        m_worldPosition = { 0.f, 0.f, 0.f };
        m_worldScale = { 1.f, 1.f, 1.f };
        m_worldRotation = { 0.f, 0.f, 0.f, 1.f };

        m_forward = { 0.f, 0.f, 1.f };

        m_parent = nullptr;
        m_children.clear();

        XMStoreFloat4x4(&m_localMatrix, XMMatrixIdentity());
        XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());

        m_localDirty = true;
        m_worldDirty = true;
        m_vectorsDirty = true;
    }

    void TransformComponent::SetLocalPosition(const XMFLOAT3& pos)
    {
        m_localPosition = pos;
        SetLocalDirty();
    }

    void TransformComponent::TranslateLocal(const XMFLOAT3& delta)
    {
        m_localPosition.x += delta.x;
        m_localPosition.y += delta.y;
        m_localPosition.z += delta.z;
        SetLocalDirty();
    }

    XMFLOAT3 TransformComponent::GetLocalPosition() const
    {
        return m_localPosition;
    }

    void TransformComponent::SetWorldPosition(const XMFLOAT3& pos)
    {
        if (m_parent)
        {
            XMMATRIX invParent = XMLoadFloat4x4(&m_parent->GetWorldMatrix());
            invParent = XMMatrixInverse(nullptr, invParent);
            XMVECTOR worldPos = XMLoadFloat3(&pos);
            XMVECTOR localPos = XMVector3Transform(worldPos, invParent);
            XMStoreFloat3(&m_localPosition, localPos);
            SetLocalDirty();
        }
        else
        {
            m_localPosition = pos;
            SetLocalDirty();
        }
    }

    void TransformComponent::TranslateWorld(const XMFLOAT3& delta)
    {
        XMVECTOR worldDelta = XMLoadFloat3(&delta);

        if (m_parent)
        {
            XMMATRIX invParent = XMLoadFloat4x4(&m_parent->GetWorldMatrix());
            invParent = XMMatrixInverse(nullptr, invParent);
            XMVECTOR localDelta = XMVector3Transform(worldDelta, invParent);
            XMFLOAT3 localDelta3;
            XMStoreFloat3(&localDelta3, localDelta);
            TranslateLocal(localDelta3);
        }
        else
        {
            TranslateLocal(delta);
        }
    }

    XMFLOAT3 TransformComponent::GetWorldPosition()
    {
        UpdateWorldData();
        return m_worldPosition;
    }

    void TransformComponent::SetLocalScale(const XMFLOAT3& scale)
    {
        m_localScale = scale;
        SetLocalDirty();
    }

    void TransformComponent::ScaleLocal(const XMFLOAT3& factor)
    {
        m_localScale.x *= factor.x;
        m_localScale.y *= factor.y;
        m_localScale.z *= factor.z;
        SetLocalDirty();
    }

    XMFLOAT3 TransformComponent::GetLocalScale() const
    {
        return m_localScale;
    }

    void TransformComponent::SetWorldScale(const XMFLOAT3& scale)
    {
        if (m_parent)
        {
            XMFLOAT3 parentScale = m_parent->GetWorldScale();
            m_localScale.x = scale.x / parentScale.x;
            m_localScale.y = scale.y / parentScale.y;
            m_localScale.z = scale.z / parentScale.z;
            SetLocalDirty();
        }
        else
        {
            m_localScale = scale;
            SetLocalDirty();
        }
    }

    void TransformComponent::ScaleWorld(const XMFLOAT3& factor)
    {
        m_worldScale.x *= factor.x;
        m_worldScale.y *= factor.y;
        m_worldScale.z *= factor.z;
        SetWorldDirty();
    }

    XMFLOAT3 TransformComponent::GetWorldScale()
    {
        UpdateWorldData();
        return m_worldScale;
    }

    void TransformComponent::SetLocalRotation(const XMFLOAT4& quat)
    {
        m_localRotation = quat;
        SetLocalDirty();
        SetVectorsDirty();
    }

    void TransformComponent::RotateLocal(const XMFLOAT4& quat)
    {
        m_localRotation = QuaternionMultiply(m_localRotation, quat);
        SetLocalDirty();
        SetVectorsDirty();
    }

    void TransformComponent::RotateLocalAroundAxis(const XMFLOAT3& axis, float angle)
    {
        XMVECTOR vAxis = XMLoadFloat3(&axis);
        vAxis = XMVector3Normalize(vAxis);
        XMVECTOR quat = XMQuaternionRotationAxis(vAxis, angle);
        XMFLOAT4 q;
        XMStoreFloat4(&q, quat);
        RotateLocal(q);
    }

    XMFLOAT4 TransformComponent::GetLocalRotation() const
    {
        return m_localRotation;
    }

    void TransformComponent::SetWorldRotation(const XMFLOAT4& quat)
    {
        if (m_parent)
        {
            XMFLOAT4 parentRot = m_parent->GetWorldRotation();
            XMFLOAT4 parentRotInv = { -parentRot.x, -parentRot.y, -parentRot.z, parentRot.w };
            m_localRotation = QuaternionMultiply(parentRotInv, quat);
            SetLocalDirty();
            SetVectorsDirty();
        }
        else
        {
            m_localRotation = quat;
            SetLocalDirty();
            SetVectorsDirty();
        }
    }

    void TransformComponent::RotateWorld(const XMFLOAT4& quat)
    {
        if (m_parent)
        {
            XMFLOAT4 worldRot = GetWorldRotation();
            worldRot = QuaternionMultiply(worldRot, quat);
            SetWorldRotation(worldRot);
        }
        else
        {
            RotateLocal(quat);
        }
    }

    void TransformComponent::RotateWorldAroundAxis(const XMFLOAT3& axis, float angle)
    {
        XMVECTOR vAxis = XMLoadFloat3(&axis);
        vAxis = XMVector3Normalize(vAxis);
        XMVECTOR quat = XMQuaternionRotationAxis(vAxis, angle);
        XMFLOAT4 q;
        XMStoreFloat4(&q, quat);
        RotateWorld(q);
    }

    XMFLOAT4 TransformComponent::GetWorldRotation()
    {
        UpdateWorldData();
        return m_worldRotation;
    }

    void TransformComponent::SetLocalYPR(float yaw, float pitch, float roll)
    {
        m_localRotation = QuaternionFromYPR(yaw, pitch, roll);
        SetLocalDirty();
        SetVectorsDirty();
    }

    void TransformComponent::RotateLocalYPR(float yaw, float pitch, float roll)
    {
        XMFLOAT4 q = QuaternionFromYPR(yaw, pitch, roll);
        RotateLocal(q);
    }

    void TransformComponent::SetLocalYPR(const DirectX::XMFLOAT3& ypr)
    {
        SetLocalYPR(ypr.x, ypr.y, ypr.z);
    }

    void TransformComponent::RotateLocalYPR(const DirectX::XMFLOAT3& ypr)
    {
        RotateLocalYPR(ypr.x, ypr.y, ypr.z);
    }

    DirectX::XMFLOAT3 TransformComponent::GetLocalYPR() const
    {
        XMFLOAT3 ypr = {};

        YPRFromQuaternion(m_localRotation, ypr.x, ypr.y, ypr.z);

        return ypr;
    }

    void TransformComponent::SetWorldYPR(float yaw, float pitch, float roll)
    {
        XMFLOAT4 q = QuaternionFromYPR(yaw, pitch, roll);
        SetWorldRotation(q);
    }

    void TransformComponent::RotateWorldYPR(float yaw, float pitch, float roll)
    {
        XMFLOAT4 q = QuaternionFromYPR(yaw, pitch, roll);
        RotateWorld(q);
    }

    void TransformComponent::SetWorldYPR(const DirectX::XMFLOAT3& ypr)
    {
        SetWorldYPR(ypr.x, ypr.y, ypr.z);
    }

    void TransformComponent::RotateWorldYPR(const DirectX::XMFLOAT3& ypr)
    {
        RotateWorldYPR(ypr.x, ypr.y, ypr.z);
    }

    DirectX::XMFLOAT3 TransformComponent::GetWorldYPR() const
    {
        XMFLOAT3 ypr = {};

        YPRFromQuaternion(m_worldRotation, ypr.x, ypr.y, ypr.z);

        return ypr;
    }

    void TransformComponent::MoveLocal(float distance, LocalAxis axis, Array<bool, 3> applyAxis)
    {
        UpdateDirectionVectors();
        XMFLOAT3 dir = GetLocalAxis(axis);
        XMFLOAT3 t = {
            dir.x * distance * applyAxis[0],
            dir.y * distance * applyAxis[1],
            dir.z * distance * applyAxis[2]
        };

        TranslateLocal(t);
    }

    void TransformComponent::MoveWorld(float distance, LocalAxis axis, Array<bool, 3> applyAxis)
    {
        UpdateDirectionVectors();

        XMFLOAT3 dir = GetLocalAxis(axis);
        XMFLOAT3 t = {
            dir.x * distance * applyAxis[0],
            dir.y * distance * applyAxis[1],
            dir.z * distance * applyAxis[2]
        };
        TranslateWorld(t);
    }

    void TransformComponent::MoveLocalTowards(const XMFLOAT3& target, float distance)
    {
        XMFLOAT3 direction;
        direction.x = target.x - m_localPosition.x;
        direction.y = target.y - m_localPosition.y;
        direction.z = target.z - m_localPosition.z;

        float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        if (length > 0.0001f)
        {
            direction.x /= length;
            direction.y /= length;
            direction.z /= length;

            direction.x *= distance;
            direction.y *= distance;
            direction.z *= distance;

            TranslateLocal(direction);
        }
    }

    void TransformComponent::MoveWorldTowards(const XMFLOAT3& target, float distance)
    {
        UpdateWorldData();
        XMFLOAT3 direction;
        direction.x = target.x - m_worldPosition.x;
        direction.y = target.y - m_worldPosition.y;
        direction.z = target.z - m_worldPosition.z;

        float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        if (length > 0.0001f)
        {
            direction.x /= length;
            direction.y /= length;
            direction.z /= length;

            direction.x *= distance;
            direction.y *= distance;
            direction.z *= distance;

            TranslateWorld(direction);
        }
    }

    void TransformComponent::LookAtLocal(const XMFLOAT3& target)
    {
        LookAtLocal(target, { 0, 1, 0 });
    }

    void TransformComponent::LookAtLocal(const XMFLOAT3& target, const XMFLOAT3& up)
    {
        XMVECTOR pos = XMLoadFloat3(&m_localPosition);
        XMVECTOR t = XMLoadFloat3(&target);
        XMVECTOR u = XMLoadFloat3(&up);

        XMMATRIX view = XMMatrixLookAtLH(pos, t, u);
        XMMATRIX rot = XMMatrixInverse(nullptr, view);

        XMVECTOR quat = XMQuaternionRotationMatrix(rot);
        XMStoreFloat4(&m_localRotation, quat);

        SetLocalDirty();
        SetVectorsDirty();
    }

    void TransformComponent::LookAtWorld(const XMFLOAT3& target)
    {
        LookAtWorld(target, { 0, 1, 0 });
    }

    void TransformComponent::LookAtWorld(const XMFLOAT3& target, const XMFLOAT3& up)
    {
        UpdateWorldData();

        XMVECTOR pos = XMLoadFloat3(&m_worldPosition);
        XMVECTOR t = XMLoadFloat3(&target);
        XMVECTOR u = XMLoadFloat3(&up);

        XMMATRIX view = XMMatrixLookAtLH(pos, t, u);
        XMMATRIX rot = XMMatrixInverse(nullptr, view);

        XMVECTOR quat = XMQuaternionRotationMatrix(rot);
        XMFLOAT4 worldQuat;
        XMStoreFloat4(&worldQuat, quat);

        SetWorldRotation(worldQuat);
    }

    void TransformComponent::LookToLocal(const XMFLOAT3& direction)
    {
        LookToLocal(direction, { 0, 1, 0 });
    }

    void TransformComponent::LookToLocal(const XMFLOAT3& direction, const XMFLOAT3& up)
    {
        XMVECTOR dir = XMLoadFloat3(&direction);
        XMVECTOR u = XMLoadFloat3(&up);

        dir = XMVector3Normalize(dir);
        XMVECTOR right = XMVector3Cross(u, dir);
        right = XMVector3Normalize(right);
        XMVECTOR newUp = XMVector3Cross(dir, right);

        XMMATRIX rotMatrix;
        rotMatrix.r[0] = right;
        rotMatrix.r[1] = newUp;
        rotMatrix.r[2] = dir;
        rotMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

        XMVECTOR quat = XMQuaternionRotationMatrix(rotMatrix);
        XMStoreFloat4(&m_localRotation, quat);

        SetLocalDirty();
        SetVectorsDirty();
    }

    void TransformComponent::LookToWorld(const XMFLOAT3& direction)
    {
        LookToWorld(direction, { 0, 1, 0 });
    }

    void TransformComponent::LookToWorld(const XMFLOAT3& direction, const XMFLOAT3& up)
    {
        XMFLOAT4 worldQuat;
        XMVECTOR dir = XMLoadFloat3(&direction);
        XMVECTOR u = XMLoadFloat3(&up);

        dir = XMVector3Normalize(dir);
        XMVECTOR right = XMVector3Cross(u, dir);
        right = XMVector3Normalize(right);
        XMVECTOR newUp = XMVector3Cross(dir, right);

        XMMATRIX rotMatrix;
        rotMatrix.r[0] = right;
        rotMatrix.r[1] = newUp;
        rotMatrix.r[2] = dir;
        rotMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

        XMVECTOR quat = XMQuaternionRotationMatrix(rotMatrix);
        XMStoreFloat4(&worldQuat, quat);

        SetWorldRotation(worldQuat);
    }

    void TransformComponent::RotateAroundLocal(const XMFLOAT3& point, const XMFLOAT3& axis, float angle)
    {
        XMVECTOR p = XMLoadFloat3(&point);
        XMVECTOR a = XMLoadFloat3(&axis);
        XMVECTOR pos = XMLoadFloat3(&m_localPosition);

        XMVECTOR toPoint = XMVectorSubtract(pos, p);

        XMVECTOR quat = XMQuaternionRotationAxis(a, angle);
        XMVECTOR rotated = XMVector3Rotate(toPoint, quat);

        XMVECTOR newPos = XMVectorAdd(p, rotated);
        XMStoreFloat3(&m_localPosition, newPos);

        RotateLocalAroundAxis(axis, angle);
    }

    void TransformComponent::RotateAroundWorld(const XMFLOAT3& point, const XMFLOAT3& axis, float angle)
    {
        UpdateWorldData();

        XMVECTOR p = XMLoadFloat3(&point);
        XMVECTOR a = XMLoadFloat3(&axis);
        XMVECTOR pos = XMLoadFloat3(&m_worldPosition);

        XMVECTOR toPoint = XMVectorSubtract(pos, p);

        XMVECTOR quat = XMQuaternionRotationAxis(a, angle);
        XMVECTOR rotated = XMVector3Rotate(toPoint, quat);

        XMVECTOR newPos = XMVectorAdd(p, rotated);

        if (m_parent)
        {
            XMMATRIX invParent = XMLoadFloat4x4(&m_parent->GetWorldMatrix());
            invParent = XMMatrixInverse(nullptr, invParent);
            newPos = XMVector3Transform(newPos, invParent);
        }

        XMStoreFloat3(&m_localPosition, newPos);
        RotateWorldAroundAxis(axis, angle);
    }

    // ==================== VECTEURS DE DIRECTION ====================

    const XMFLOAT3& TransformComponent::GetLocalAxis(LocalAxis axis)
    {
        UpdateDirectionVectors();

        switch (axis)
        {
        case _Forward:
            return m_forward;
        case _Up:
            return m_up;
        case _Right:
            return m_right;

        default:
            return m_forward;
        }
    }

    const XMFLOAT4X4& TransformComponent::GetLocalMatrix()
    {
        UpdateMatrices();
        return m_localMatrix;
    }

    const XMFLOAT4X4& TransformComponent::GetWorldMatrix()
    {
        UpdateMatrices();
        UpdateWorldData();
        return m_worldMatrix;
    }

    const DirectX::XMFLOAT4X4 TransformComponent::GetWorldMatrixTranspose()
    {
        DirectX::XMFLOAT4X4 transpose;
        DirectX::XMMATRIX mat = XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));
        XMStoreFloat4x4(&transpose, mat);


        return transpose;
    }

    const DirectX::XMFLOAT4X4 TransformComponent::GetInverseWorldMatrixTranspose()
    {
        DirectX::XMFLOAT4X4 invTranspose;
        DirectX::XMMATRIX mat = XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_worldMatrix)));
        XMStoreFloat4x4(&invTranspose, mat);

        return invTranspose;
    }

    XMFLOAT3 TransformComponent::TransformPoint(const XMFLOAT3& point)
    {
        XMMATRIX world = XMLoadFloat4x4(&GetWorldMatrix());
        XMVECTOR p = XMLoadFloat3(&point);
        XMVECTOR result = XMVector3Transform(p, world);
        XMFLOAT3 out;
        XMStoreFloat3(&out, result);
        return out;
    }

    void TransformComponent::SetParent(TransformComponent* parent)
    {
        if (m_parent == parent)
            return;

        if (m_parent)
            m_parent->RemoveChild(this);

        m_parent = parent;

        if (m_parent)
            m_parent->AddChild(this);

        SetWorldDirty();
    }

    TransformComponent* TransformComponent::GetParent() const
    {
        return m_parent;
    }

    void TransformComponent::AddChild(TransformComponent* child)
    {
        if (child && std::find(m_children.begin(), m_children.end(), child) == m_children.end())
        {
            m_children.push_back(child);
        }
    }

    void TransformComponent::RemoveChild(TransformComponent* child)
    {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end())
        {
            m_children.erase(it);
        }
    }

    const Vector<TransformComponent*>& TransformComponent::GetChildren() const
    {
        return m_children;
    }

    void TransformComponent::UpdateMatrices()
    {
        if (!m_localDirty && !m_worldDirty)
            return;

        if (m_localDirty)
        {
            XMVECTOR pos = XMLoadFloat3(&m_localPosition);
            XMVECTOR scale = XMLoadFloat3(&m_localScale);
            XMVECTOR rot = XMLoadFloat4(&m_localRotation);

            XMMATRIX mat = XMMatrixScalingFromVector(scale) *
                XMMatrixRotationQuaternion(rot) *
                XMMatrixTranslationFromVector(pos);

            XMStoreFloat4x4(&m_localMatrix, mat);
            m_localDirty = false;
            m_worldDirty = true;
        }

        if (m_worldDirty)
        {
            if (m_parent)
            {
                XMMATRIX local = XMLoadFloat4x4(&m_localMatrix);
                XMMATRIX parentWorld = XMLoadFloat4x4(&m_parent->GetWorldMatrix());
                XMMATRIX world = XMMatrixMultiply(local, parentWorld);
                XMStoreFloat4x4(&m_worldMatrix, world);
            }
            else
            {
                m_worldMatrix = m_localMatrix;
            }
            m_worldDirty = false;
        }
    }

    void TransformComponent::UpdateWorldData()
    {
        if (!m_worldDirty)
            return;

        UpdateMatrices();

        XMMATRIX world = XMLoadFloat4x4(&m_worldMatrix);

        XMVECTOR scale, rot, pos;
        XMMatrixDecompose(&scale, &rot, &pos, world);

        XMStoreFloat3(&m_worldPosition, pos);
        XMStoreFloat3(&m_worldScale, scale);
        XMStoreFloat4(&m_worldRotation, rot);
    }

    void TransformComponent::UpdateDirectionVectors()
    {
        if (!m_vectorsDirty && !m_worldDirty)
            return;

        UpdateWorldData();

        XMVECTOR quat = XMLoadFloat4(&m_worldRotation);
        XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
        forward = XMVector3Normalize(XMVector3Rotate(forward, quat));
        XMStoreFloat3(&m_forward, forward);

        XMVECTOR refUp = XMVectorSet(0, 1, 0, 0);
        XMVECTOR right = XMVectorSet(1, 0, 0, 0);
        right = XMVector3Normalize(XMVector3Cross(refUp, forward));
        XMStoreFloat3(&m_right, right);

        XMVECTOR up = XMVector3Cross(forward, right);
        XMStoreFloat3(&m_up, up);
    }

    void TransformComponent::SetLocalDirty()
    {
        m_localDirty = true;
        m_worldDirty = true;
        m_vectorsDirty = true;

        for (auto* child : m_children)
        {
            child->SetWorldDirty();
        }
    }

    void TransformComponent::SetWorldDirty()
    {
        m_worldDirty = true;
        m_vectorsDirty = true;

        for (auto* child : m_children)
        {
            child->SetWorldDirty();
        }
    }

    void TransformComponent::SetVectorsDirty()
    {
        m_vectorsDirty = true;
    }

    XMFLOAT4 TransformComponent::QuaternionFromYPR(float yaw, float pitch, float roll)
    {
        XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
        XMFLOAT4 result;
        XMStoreFloat4(&result, q);
        return result;
    }

    void TransformComponent::YPRFromQuaternion(const XMFLOAT4& quat, float& yaw, float& pitch, float& roll)
    {
        float x = quat.x;
        float y = quat.y;
        float z = quat.z;
        float w = quat.w;

        float sinPitch = 2.0f * (w * x + y * z);
        float cosPitch = 1.0f - 2.0f * (x * x + y * y);
        pitch = atan2f(sinPitch, cosPitch);

        float sinYaw = 2.0f * (w * y - z * x);
        float cosYaw = 1.0f - 2.0f * (y * y + z * z);
        yaw = atan2f(sinYaw, cosYaw);

        float sinRoll = 2.0f * (w * z + x * y);
        float cosRoll = 1.0f - 2.0f * (z * z + x * x);
        roll = atan2f(sinRoll, cosRoll);
    }

    XMFLOAT4 TransformComponent::QuaternionMultiply(const XMFLOAT4& q1, const XMFLOAT4& q2)
    {
        XMVECTOR a = XMLoadFloat4(&q1);
        XMVECTOR b = XMLoadFloat4(&q2);
        XMVECTOR result = XMQuaternionMultiply(a, b);
        XMFLOAT4 r;
        XMStoreFloat4(&r, result);
        return r;
    }

    XMFLOAT3 TransformComponent::QuaternionRotateVector(const XMFLOAT4& quat, const XMFLOAT3& vec)
    {
        XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR v = XMLoadFloat3(&vec);
        XMVECTOR result = XMVector3Rotate(v, q);
        XMFLOAT3 r;
        XMStoreFloat3(&r, result);
        return r;
    }
}
