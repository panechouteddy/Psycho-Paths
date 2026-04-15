#pragma once

class Player;

class FootstepNoise : public GameObject
{
public:
	int m_previousState = -1;

private:
	//Delay between each signal
	Timer m_frequencyTimer;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;

	void HandleNoise(Player* p_player);

private:
	void MakeNoise(NoiseComponent& nc, std::string name, float range);
};

