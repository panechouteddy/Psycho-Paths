#include "pch.h"
#include "SavePoint.h"

#include "SaveManager.h"

void SavePoint::OnInit()
{
	AddBoxCollider((int)TagCollision::_Decor);
}

void SavePoint::OnStart()
{

}

void SavePoint::OnUpdate()
{

}

void SavePoint::OnEnd()
{

}

void SavePoint::OnCollision(u32 self, u32 other, const CollisionInfo& collisionInfo)
{
	SetActive(false);

	SaveManager::SaveDatasInFile();
	SaveManager::SavePlaytimeInFile();
}

void SavePoint::SetColliderSize(XMFLOAT3 size)
{
	GetComponent<BoxColliderComponent>().CreateManualBox(size, false, { 0, 0, 0 });
}
