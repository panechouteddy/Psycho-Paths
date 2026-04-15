#pragma once
#include <GameObject.h>

class Monster : public nam::GameObject
{
public:
	std::priority_queue<PursuitPoint, Vector<PursuitPoint>, Compare> m_pursuitPoints = {};

	PathFindingComponent* mp_pathComp = nullptr;
	StatesManager m_stateManager;

	enum States
	{
		_IDLE,
		_PATROL,
		_FOLLOW,
		_PURSUIT,
		Count
	};
private:
	
	Mesh* mp_mesh = nullptr;

	//Stats
	float m_height = 2.f;
	float m_speed = 2.f;
	float m_fov = XM_PIDIV4;
	float m_visionRange = 7.5f;

	//Seeking
	Timer m_afkTimer; 
	Timer m_dontSeePlayerTimer;
	bool m_seek = false;

	//Animation
	bool m_isPlayerCaught = false;
	Timer m_animTimer;

public:
	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;
	
	void CheckStates();
	void OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo) override;
	void OnDestroy() override;

	void ActivatePursuit();

	void SetSeek(bool state);

	void MonsterIdle();

	void MonsterPatrol();

	void MonsterFollow();
	void TriggerFollow();

	void MonsterPursuit();

	void ToPatrol();

	bool PursitIsArrived();

	void AddPursuitPoints(Vector<PursuitPoint>& pursuitpoints);
private: 
	bool IsSeeingPlayer(XMFLOAT3 m, XMFLOAT3 p, GameObject* p_player, float visionRange);

	bool IsPlayerInCone(XMFLOAT3 m, XMFLOAT3 p, GameObject* p_player);

	void HandleJumpscareAnimation();

};

