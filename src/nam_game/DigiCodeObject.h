#pragma once
class DigiCodeObject : public GameObject
{
	UIObject* mp_codeInterface;

	Door* m_door = nullptr;

	std::string m_CodeValues;
	std::string m_Code;

	UnMap<int, GameObject* > m_buttons;
public:
	bool m_digiCodeOnX = false ;
	bool m_isActiveCode = false;

	void OnInit() override;
	void OnStart() override;
	void OnUpdate() override;
	void InitClosestDoor();
	void InitCode(std::string code);
};

