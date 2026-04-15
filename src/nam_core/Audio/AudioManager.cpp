#include "pch.h"
#include "AudioManager.h"

namespace nam
{
	ma_engine AudioManager::m_engine = {};
	UnMap<std::string, SoundData> AudioManager::m_sounds = {};
	UnMap<std::string, MusicData> AudioManager::m_musics = {};

	bool AudioManager::m_musicLooped = false;
	ma_uint64 AudioManager::m_currentMusicPausedFrame = 0;
	ma_sound* AudioManager::m_currentMusic = nullptr;

	Vector<ma_sound*> AudioManager::m_activeSounds = {};

	float AudioManager::m_globalVolume = 1.f;
	float AudioManager::m_sfxVolume = 1.f;
	float AudioManager::m_musicVolume = 1.f;

	void AudioManager::Init()
	{
		if (ma_engine_init(nullptr, &m_engine) != MA_SUCCESS)
		{
			_ASSERT(false && "Error to Init ma_engine from AudioManager");
			return;
		}

		m_activeSounds.reserve(MAX_ACTIVE_SOUNDS);
	}

	void AudioManager::UpdateSoundsPool()
	{
		for (auto it = m_activeSounds.begin(); it != m_activeSounds.end();)
		{
			if (!ma_sound_is_playing(*it))
			{
				ma_sound_uninit(*it);

				delete* it;

				it = m_activeSounds.erase(it);
			}
			else
			{
				++it;
			}
		}

	}

	void AudioManager::UnInit()
	{
		for (auto& sound : m_activeSounds)
		{
			ma_sound_stop(sound);
			ma_sound_uninit(sound);

			delete sound;
		}

		for (auto& [name, sound] : m_sounds)
		{
			ma_sound_stop(&sound.m_soundRef);
			ma_sound_uninit(&sound.m_soundRef);
		}

		for (auto& [name, music] : m_musics)
		{
			ma_sound_stop(&music.m_musicRef);
			ma_sound_uninit(&music.m_musicRef);
		}

		m_activeSounds.clear();
		m_sounds.clear();
		m_musics.clear();

		ma_engine_uninit(&m_engine);
	}

	void AudioManager::LoadSound(std::string name, std::string path, float defaultVolume)
	{
		auto it = m_sounds.find(name);

		if (it != m_sounds.end())
		{
			return;
		}

		m_sounds[name] = {};
		SoundData& sfx = m_sounds[name];
		sfx.m_path = path;
		sfx.m_refVolume = defaultVolume;

		if (ma_sound_init_from_file(&m_engine, path.c_str(), MA_SOUND_FLAG_DECODE, nullptr, nullptr, &sfx.m_soundRef) != MA_SUCCESS)
		{
			_ASSERT(false && "LoadSound() -> Failed to Load");
		}

		ma_sound_set_spatialization_enabled(&sfx.m_soundRef, MA_FALSE);
	}

	void AudioManager::LoadMusic(std::string name, std::string path, float defaultVolume)
	{
		auto it = m_musics.find(name);

		if (it != m_musics.end())
		{
			return;
		}

		m_musics[name] = {};
		MusicData& music = m_musics[name];
		music.m_path = path;
		music.m_refVolume = defaultVolume;

		if (ma_sound_init_from_file(&m_engine, path.c_str(), MA_SOUND_FLAG_STREAM, nullptr, nullptr, &music.m_musicRef) != MA_SUCCESS)
		{
			_ASSERT(false && "LoadMusic() -> Failed to Load");
			return;
		}

		ma_sound_set_spatialization_enabled(&music.m_musicRef, MA_FALSE);
	}

	void AudioManager::PlaySound(std::string name, float volumeScale, DirectX::XMFLOAT3 pos, bool spatialized)
	{
		if (m_activeSounds.size() >= MAX_ACTIVE_SOUNDS)
			return;

		auto it = m_sounds.find(name);

		if (it == m_sounds.end())
		{
			_ASSERT(false && "PlaySound() -> Can't find Sound");
			return;
		}

		//Else
		m_activeSounds.emplace_back(new ma_sound);
		ma_sound* newSound = m_activeSounds.back();

		ma_sound_init_copy(&m_engine, &it->second.m_soundRef, 0, nullptr, newSound);
		ma_sound_set_volume(newSound, m_globalVolume * it->second.m_refVolume * volumeScale * m_sfxVolume);

		if (spatialized)
		{
			ma_sound_set_position(newSound, pos.x, pos.y, pos.z);
			ma_sound_set_spatialization_enabled(newSound, MA_TRUE);
		}
		else
		{
			ma_sound_set_spatialization_enabled(newSound, MA_FALSE); // <- important
		}

		ma_sound_start(newSound);
	}
	void AudioManager::PlayMusic(std::string name, float volumeScale, bool looped, bool resetMusicIfSame)
	{
		auto it = m_musics.find(name);

		if (it == m_musics.end())
		{
			_ASSERT(false && "PlayMusic() -> Can't find Music");
			return;
		}

		if (resetMusicIfSame == false && m_currentMusic == &it->second.m_musicRef)
			return;

		m_musicLooped = looped;

		StopCurrentMusic();

		m_currentMusic = &it->second.m_musicRef;
	
		SetCurrentMusicVolume(m_globalVolume * it->second.m_refVolume * volumeScale * m_musicVolume);
		ma_sound_set_looping(m_currentMusic, (ma_bool32)looped);
		ResumeCurrentMusic();
	}

	void AudioManager::PauseCurrentMusic()
	{
		if (m_currentMusic == nullptr)
			return;

		ma_uint64 frame = 0;

		ma_sound_get_cursor_in_pcm_frames(m_currentMusic, &frame);
		m_currentMusicPausedFrame = frame;
		ma_sound_stop(m_currentMusic);
	}
	void AudioManager::ResumeCurrentMusic()
	{
		if (m_currentMusic == nullptr)
			return;

		ma_sound_seek_to_pcm_frame(m_currentMusic, m_currentMusicPausedFrame);
		ma_sound_start(m_currentMusic);
	}
	void AudioManager::StopCurrentMusic()
	{
		if (m_currentMusic == nullptr)
			return;

		ma_sound_stop(m_currentMusic);
		m_currentMusicPausedFrame = 0;

		m_currentMusic = nullptr;
	}
	void AudioManager::SetCurrentMusicVolume(float volume)
	{
		if (m_currentMusic == nullptr)
			return;

		ma_sound_set_volume(m_currentMusic, volume);
	}

	void AudioManager::SetListenerData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up)
	{
		ma_engine_listener_set_position(&m_engine, 0, pos.x, pos.y, pos.z);
		ma_engine_listener_set_direction(&m_engine, 0, forward.x, forward.y, forward.z); // forward vers +Z
		ma_engine_listener_set_world_up(&m_engine, 0, up.x, up.y, up.z);   // up = +Y ou +Z selon ton axe vertical
	}

	void AudioManager::SetGlobalVolume(float val)
	{
		m_globalVolume = val;

		for (auto& [key, sound] : m_sounds)
		{
			ma_sound_set_volume(&sound.m_soundRef, m_globalVolume * sound.m_refVolume * m_sfxVolume);
		}

		for (auto& [key, music] : m_musics)
		{
			ma_sound_set_volume(&music.m_musicRef, m_globalVolume * music.m_refVolume * m_musicVolume);
		}
	}

	void AudioManager::SetMusicVolume(float val)
	{
		m_musicVolume = std::clamp(val, 0.f, 1.f);

		for (auto& [key, music] : m_musics)
		{
			ma_sound_set_volume(&music.m_musicRef, m_globalVolume * music.m_refVolume * m_musicVolume);
		}
	}

	void AudioManager::SetSfxVolume(float val)
	{
		m_sfxVolume = std::clamp(val, 0.f, 1.f);

		for (auto& [key, sound] : m_sounds)
		{
			ma_sound_set_volume(&sound.m_soundRef, m_globalVolume * sound.m_refVolume * m_sfxVolume);
		}
	}
}