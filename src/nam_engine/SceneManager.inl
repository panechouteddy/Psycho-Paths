
namespace nam
{
	template<typename T>
	T* SceneManager::CreateScene(size sceneTag)
	{
		static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene in CreateScene<T>");

		auto it = m_scenes.find(sceneTag);

		if (it != m_scenes.end())
		{
			_ASSERT(false && "SceneManager has already a Scene with this tag !");
			return nullptr;
		}

		T* t = new T();
		t->m_tag = sceneTag;
		m_scenes[sceneTag] = t;

		t->Init();

		return t;
	}
}
