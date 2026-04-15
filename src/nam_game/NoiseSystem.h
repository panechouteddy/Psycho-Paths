#pragma once
#include <ISystem.h>

//System used to make React the Monster to Sounds

struct NoiseSystem : public ISystem
{
	int m_monsterTag = -1;

public:
	void SetMonsterTag(int tag);
	void Update(Ecs& ecs) override;

private:
	bool IsMonsterHearingSound(Scene* p_scene, XMFLOAT3 monsterPos, XMFLOAT3 soundPos, float soundRange);
};

