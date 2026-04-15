#pragma once
#include "Ecs.h"
#include "SceneManager.h"
#include "AppChrono.h"
#include "Entity.h"
#include "Window.h"

#include <TextureManager.h>
#include <MaterialManager.h>
#include <RenderManager.h>

#include <Audio/AudioManager.h>
#include <StatesManager.h>

//Qol (Quality Of Life)
#define DEGREE_TO_RADIAN XM_PI / 180.f
#ifdef	RENDER_MANAGER
#define RENDER_ITEM_MANAGER RENDER_MANAGER->GetRenderItemManager()
#define LIGHT_MANAGER RENDER_MANAGER->GetLightManager()
#define MATERIAL_MANAGER RENDER_MANAGER->GetMaterialManager()
#define WINDOW RENDER_MANAGER->GetWindow()
#endif

#define APP App::Get()

#ifdef APP
#define CHRONO APP->GetChrono()
#define SCENE_MANAGER APP->GetSceneManager()
#define ECS APP->GetEcs()
#endif

namespace nam
{
	class RenderManager;

	class GameObject;
	class LoadingScreen;

	class App final
	{
	public:
		bool m_isRunning = true;

	private:
		static App* msp_instance;
		Window m_window;
		Ecs m_ecs;
		SceneManager m_sceneManager;

		GameObject* mp_camera = nullptr;
		AppChrono m_chrono;
		RenderManager* mp_renderManager = nullptr;

		bool m_minimized = false;
		bool m_maximized = false;
		bool m_resizing = false;
		bool m_fullscreen = false;

		struct TextureData
		{
			std::wstring m_path = L"";
			size m_uniqueTag = 0;
			bool m_usingTextureFolder = false;
		};

		Vector<TextureData> m_texturesDataToLoad;

		LoadingScreen* mp_loadingScreen = nullptr;
	
		DirectX::XMFLOAT3 m_ambientLightColor = { 0, 0, 0 };
	public:
		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		~App();

		// Pass the Window Settings the first time to Init the App, next times, just call Get() without anything in parameters !
		static App* Get(HINSTANCE hInst = 0, int startWindowWidth = -1, int startWindowHeight = -1);
		static void Destroy();
	
		void Run();

		// To call before Run() | if usingTextureFolder true -> the path will remains in the Textures Folder
		void LoadTexture(std::wstring path, size uniqueTag, bool usingTextureFolder = false);

		void ToggleDebugConsole(bool state);
		
		//The minimal light color you'll see
		void SetAmbientLightColor(DirectX::XMFLOAT3 color);
		const DirectX::XMFLOAT3& GetAmbientLightColor() const;

		Ecs& GetEcs();
		SceneManager& GetSceneManager();
    
		Window& GetWindow();

		AppChrono& GetChrono();
		GameObject* GetCamera();

	private:
		App(HINSTANCE hInst, int startWindowWidth, int startWindowHeight);

		void Init();
		void Start();

		void RenderFirstFrame();
		void LoadTextures();

		int MainLoop();
		void Update();
		void Resize();

		void CreateCamera();
		void CameraDefaultSettings();

		friend class GameObject;
	};
}

#include "App.inl"
