#pragma once
struct GeneralData
{
	float m_playerSensivity = 0.002f;
	float m_sensivityScale = 1.f;

	float m_globalVolume = 1.f;
	float m_musicVolume = 1.f;
	float m_sfxVolume = 1.f;
};

struct GeneralConfigs
{
	static GeneralData m_data;

public:
	static const GeneralData& GetDatas();

	static void InitFromFile();
	static void SaveGeneralInFile();

	static void AddSensivityScale(float val);
	static void AddGlobalVolume(float val);
	static void AddMusicVolume(float val);
	static void AddSfxVolume(float val);

	static void UpdateAudio();
};

