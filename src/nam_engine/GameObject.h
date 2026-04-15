#pragma once

namespace nam
{
	class App;
	class Scene;

	class GameObject
	{
	public:
		TransformComponent* mp_transform = nullptr;

		//Only useful when debugging
		std::string m_name = "Undefined";

	protected:
		Entity m_entity = {};
		Scene* mp_scene = nullptr;
		int m_tag = -1;
		bool m_isActive = false;

	public:
		GameObject();

		void Init(Scene* scene, Entity entity);
	
		virtual void OnInit() {};
		virtual void OnStart() {};
		virtual void OnUpdate() {};
		virtual void OnEnd() {};

		virtual void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) {};
		virtual void OnDestroy() {};

		virtual void SetActive(bool active);
		void Destroy();

		template<typename Component>
		void AddComponent(const Component& data);
		template<typename Component>
		const bool HasComponent();
		template<typename Component>
		Component& GetComponent();

		void SetBehavior();

		//Collision tag is apart from the Object Tag, it's useful to not Detect Collisions with some combinations of Collision Tags
		void AddBoxCollider(int collisionTag = -1);

		//Collision tag is apart from the Object Tag, it's useful to not Detect Collisions with some combinations of Collision Tags
		void AddSphereCollider(int collisionTag = -1);

		//Don't use if you don't use UIObject or Button
		virtual void SetUiPosition(XMFLOAT2 pos) {};
		virtual void TranslateUiPosition(XMFLOAT2 t) {};

		Entity* GetEntity();
		Scene* GetScene();

		void SetTag(int tag);
		bool IsTag(int tag) const;
		int GetTag() const;
		
	private:
		template<typename T>
		void SetFunctionUpdate(T* owner, void (T::* Update)());
		template<typename T>
		void SetBoxCollider(T* owner, void(T::* Collide)(u32 self, u32 other, const CollisionInfo& collisionInfo), int collisionTag);
		template<typename T>
		void SetSphereCollider(T* owner, void(T::* Collide)(u32 self, u32 other, const CollisionInfo& collisionInfo), int collisionTag);

		friend class App;
		friend class Scene;
	};
}

#include "GameObject.inl"