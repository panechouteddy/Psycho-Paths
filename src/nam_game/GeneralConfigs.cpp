#include "pch.h"
#include "GeneralConfigs.h"
constexpr const char* PATH_GENCONFIG = "../../res/Save/generalconfigs.txt";

#include <Audio/AudioManager.h>

GeneralData GeneralConfigs::m_data = {};

const GeneralData& GeneralConfigs::GetDatas()
{
	return m_data;
}

void GeneralConfigs::InitFromFile()
{
	std::ifstream file(PATH_GENCONFIG);

	if (!file.is_open())
	{
		SaveGeneralInFile();
	}
	else
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream ss(line);
			std::string prefix;

			ss >> prefix;

			if (prefix == "SensivityScale")
			{
				ss >> m_data.m_sensivityScale;
			}
			else if (prefix == "GlobalVolume")
			{
				ss >> m_data.m_globalVolume;
			}
			else if (prefix == "MusicVolume")
			{
				ss >> m_data.m_musicVolume;
			}
			else if (prefix == "SfxVolume")
			{
				ss >> m_data.m_sfxVolume;
			}
		}
	}

	UpdateAudio();
}

void GeneralConfigs::SaveGeneralInFile()
{
	std::ofstream fileOut(PATH_GENCONFIG, std::ios::trunc);

	if (!fileOut.is_open())
		return;

	fileOut << "SensivityScale " << m_data.m_sensivityScale << "\n";
	fileOut << "GlobalVolume " << m_data.m_globalVolume << "\n";
	fileOut << "MusicVolume " << m_data.m_musicVolume << "\n";
	fileOut << "SfxVolume " << m_data.m_sfxVolume << "\n";
	fileOut.close();

	UpdateAudio();
	Print("General Config saved !");
}

void GeneralConfigs::AddSensivityScale(float val)
{
	m_data.m_sensivityScale = std::clamp(m_data.m_sensivityScale + val, 0.1f, 10.f);
}

void GeneralConfigs::AddGlobalVolume(float val)
{
	m_data.m_globalVolume = std::clamp(m_data.m_globalVolume + val, 0.f, 2.f);
	AudioManager::SetGlobalVolume(m_data.m_globalVolume);
}

void GeneralConfigs::AddMusicVolume(float val)
{
	m_data.m_musicVolume = std::clamp(m_data.m_musicVolume + val, 0.f, 1.f);
	AudioManager::SetMusicVolume(m_data.m_musicVolume);
}

void GeneralConfigs::AddSfxVolume(float val)
{
	m_data.m_sfxVolume = std::clamp(m_data.m_sfxVolume + val, 0.f, 1.f);
	AudioManager::SetSfxVolume(m_data.m_sfxVolume);
}

void GeneralConfigs::UpdateAudio()
{
	AudioManager::SetGlobalVolume(m_data.m_globalVolume);
	AudioManager::SetMusicVolume(m_data.m_musicVolume);
	AudioManager::SetSfxVolume(m_data.m_sfxVolume);
}
