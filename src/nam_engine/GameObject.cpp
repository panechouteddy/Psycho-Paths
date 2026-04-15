#include "pch.h"
#include "GameObject.h"

using namespace DirectX;

namespace nam
{
	GameObject::GameObject()
	{
	}

	void GameObject::Init(Scene* scene, Entity entity)
	{
		mp_scene = scene;
		m_entity = entity;

		TransformComponent transform = TransformComponent();
		transform.SetWorldPosition(XMFLOAT3(0.f, 0.f, 0.f));
		AddComponent(transform);

		mp_transform = &GetComponent<TransformComponent>();

		OnInit();
	}

	void GameObject::SetActive(bool active)
	{
		Entity* e = GetEntity();

		ECS.SetEntityActive(*e, active);

		m_isActive = active;
	}

	void GameObject::Destroy()
	{
		OnDestroy();
		mp_scene->DestroyGameObject(this);
	}

	void GameObject::SetBehavior()
	{
		SetFunctionUpdate(this, &GameObject::OnUpdate);
	}
	void GameObject::AddBoxCollider(int collisionTag)
	{
		SetBoxCollider(this, &GameObject::OnCollision, collisionTag);
	}
	void GameObject::AddSphereCollider(int collisionTag)
	{
		SetSphereCollider(this, &GameObject::OnCollision, collisionTag);
	}

	Entity* GameObject::GetEntity()
	{
		return &m_entity;
	}

	Scene* GameObject::GetScene()
	{
		return mp_scene;
	}

	void GameObject::SetTag(int tag)
	{
		m_tag = tag;
	}

	bool GameObject::IsTag(int tag) const
	{
		return m_tag == tag;
	}

	int GameObject::GetTag() const
	{
		return m_tag;
	}
}