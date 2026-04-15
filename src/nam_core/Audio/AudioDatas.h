#pragma once
#include "miniaudio.h"

struct SoundData
{
    ma_sound m_soundRef = {};
    std::string m_path = "";
    float m_refVolume = 1.f;
};

struct MusicData
{
    ma_sound m_musicRef = {};
    std::string m_path = "";
    float m_refVolume = 1.f;
};

