#include "pch.h"
#include "LightManager.h"
#include "MaterialManager.h"

namespace nam
{
	Queue<u8> LightManager::m_freeIDs = {};

	LightManager::LightManager()
	{
		for (size_t i = 0; i < MAX_LIGHTS; i++)
		{
			m_freeIDs.push(static_cast<u8>(i));
		}

		m_lights.reserve(MAX_LIGHTS);
	}

	LightManager::~LightManager()
	{
		for (auto& [id, light] : m_lights)
		{
			delete light;
		}

		m_lights.clear();
		m_taggedLights.clear();
	}

	Light* LightManager::CreateLight()
	{
		if (m_lights.size() >= MAX_LIGHTS)
		{
			_ASSERT(false && "Too many Lights !");
			return nullptr;
		}

		Light* newLight = new Light();

		u8 id = m_freeIDs.front();
		m_freeIDs.pop();

		newLight->m_id = id;
		m_lights[id] = newLight;

		return newLight;
	}

	bool LightManager::RemoveLight(u8 lightID)
	{
		auto it = m_lights.find(lightID);

		if (it == m_lights.end())
		{
			_ASSERT(false && "Light not found ! Can't Erase !");
			return false;
		}

		m_freeIDs.push(it->second->m_id);

		delete it->second;
		m_lights.erase(it);

		return true;
	}

	bool LightManager::RemoveLight(Light*& p_light)
	{
		if (p_light == nullptr)
			return false;

		if (RemoveLight(p_light->m_id) == false)
			return false;

		p_light = nullptr;

		return true;
	}

	void LightManager::FillBufferLight(Light* bufferTab)
	{
		size count = m_lights.size();

		Vector<Light*> lights;

		for (auto& [key, light] : m_lights)
		{
			lights.push_back(light);
		}

		for (size i = 0; i < count; ++i)
		{
			bufferTab[i] = *lights[i]; // Copy the light
		}
	}

	void LightManager::SetRenderItemTag(Light* p_light, size tag)
	{
		m_taggedLights[tag] = p_light;
	}

	Light* LightManager::GetLight(u8 lightID)
	{
		auto it = m_lights.find(lightID);

		if (it == m_lights.end())
		{
			_ASSERT(false && "Light not found ! Can't Get !");
			return nullptr; 
		}

		return it->second;
	}

	Light* LightManager::GetLight(size tag)
	{
		auto it = m_taggedLights.find(tag);

		if (it == m_taggedLights.end())
		{
			_ASSERT(false && "Light Tag not found ! Can't Get !");
			return nullptr;
		}

		return it->second;
	}

	u32 LightManager::GetLightsCount() const
	{
		return static_cast<u32>(m_lights.size());
	}
}
