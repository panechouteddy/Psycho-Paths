#pragma once
#include "miniaudio.h"
#include "../BaseTypes.h"
#include "../ContainerAssociative.h"
#include "../ContainerSequential.h"

#include "AudioDatas.h"

#include <DirectXMath.h>

#undef PlaySound

namespace nam
{
	constexpr int MAX_ACTIVE_SOUNDS = 128;

	class AudioManager
	{
	public:
		static ma_engine m_engine;

	private:
		static UnMap<std::string, SoundData> m_sounds;
		static UnMap<std::string, MusicData> m_musics;

		static bool m_musicLooped;
		static ma_uint64 m_currentMusicPausedFrame;
		static ma_sound* m_currentMusic;

		static Vector<ma_sound*> m_activeSounds;

		static float m_globalVolume;

		static float m_sfxVolume;
		static float m_musicVolume;

	public:
		static void Init();
		static void UpdateSoundsPool();
		static void UnInit();

		static void LoadSound(std::string name, std::string path, float defaultVolume = 1.f);
		static void LoadMusic(std::string name, std::string path, float defaultVolume = 1.f);

		static void PlaySound(std::string name, float volumeScale = 1.f, DirectX::XMFLOAT3 pos = { 0, 0, 0 }, bool spatialized = false);

		static void PlayMusic(std::string name, float volumeScale, bool looped, bool resetMusicIfSame);

		static void PauseCurrentMusic();
		static void ResumeCurrentMusic();
		static void StopCurrentMusic();
		static void SetCurrentMusicVolume(float volume);

		//Only if 3D Spatialisation
		static void SetListenerData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up);

		static void SetGlobalVolume(float val);
		static void SetMusicVolume(float val);
		static void SetSfxVolume(float val);
	};
}


