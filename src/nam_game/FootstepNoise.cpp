#include "pch.h"
#include "FootstepNoise.h"

#include "Player.h"

void FootstepNoise::OnInit()
{
	NoiseComponent nc;
	nc.m_soundName = "Walk";
	nc.m_soundRange = 0.f;
	nc.m_soundScale = 1.f;
	AddComponent(nc);

	m_frequencyTimer.Init(0.f);
}

void FootstepNoise::OnStart()
{
	m_frequencyTimer.ResetProgress();
}

void FootstepNoise::OnUpdate()
{
}

void FootstepNoise::HandleNoise(Player* p_player)
{
	if (p_player == nullptr)
		return;

	mp_transform->SetWorldPosition(p_player->mp_transform->GetWorldPosition());

	StatesManager& sm = p_player->m_stateManager;
	float dt = CHRONO.GetScaledDeltaTime();

	NoiseComponent& nc = GetComponent<NoiseComponent>();

	if (sm.IsActiveState(Player::States::_WALK))
	{
		if (m_previousState == Player::States::_WALK)
		{
			//Make noise
			if (m_frequencyTimer.GetProgress() <= 0.f)
				MakeNoise(nc, "Walk", 0);

			if (m_frequencyTimer.IsTargetReached())
			{
				m_frequencyTimer.ResetProgress();
				return;
			}

			m_frequencyTimer.Update(dt);
		}
		else
		{
			m_frequencyTimer.SetTargetTime(0.9f);
			m_previousState = Player::States::_WALK;
		}	
	}
	else if (sm.IsActiveState(Player::States::_RUN))
	{
		if (m_previousState == Player::States::_RUN)
		{
			//Make noise
			if (m_frequencyTimer.GetProgress() <= 0.f)
				MakeNoise(nc, "Run", 5);

			if (m_frequencyTimer.IsTargetReached())
			{
				m_frequencyTimer.ResetProgress();
				return;
			}

			m_frequencyTimer.Update(dt);
		}
		else
		{
			m_frequencyTimer.SetTargetTime(0.3f);
			m_previousState = Player::States::_RUN;
		}
	}
}

void FootstepNoise::MakeNoise(NoiseComponent& nc, std::string name, float range)
{
	SetActive(true);
	nc.m_activated = true;
	nc.m_soundName = name;
	nc.m_soundRange = range;
	nc.m_soundScale = 1.f;
}
