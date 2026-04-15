 #include "pch.h"
#include "App.h"

#include "Systems.h"
#include "Components.h"

#include "Window.h"

#include <Mesh.h>
#include <Sprite.h>
#include <Text.h>

#include "LoadingScreen.h"

using namespace DirectX;

namespace nam
{
	LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return App::Get()->MsgProc(hwnd, msg, wParam, lParam);
	}

	LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msp_instance == nullptr)
			return DefWindowProc(hwnd, msg, wParam, lParam);

		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			m_window.m_clientWidth = LOWORD(lParam);
			m_window.m_clientHeight = HIWORD(lParam);

			if (RENDER_MANAGER->GetDevice())
			{
				if (wParam == SIZE_MINIMIZED)
				{
					m_minimized = true;
					m_maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					m_minimized = false;
					m_maximized = true;
					Resize();
				}
				else if (wParam == SIZE_RESTORED)
				{
					if (m_minimized)
					{
						m_minimized = false;
						Resize();
					}
					else if (m_maximized)
					{
						m_maximized = false;
						Resize();
					}
					else if (m_resizing)
					{
					}
					else
					{
						Resize();
					}
				}
			}
			return 0;

		case WM_MOUSEWHEEL:
		{
			Input::m_scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			return 0;
		}
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	App* App::msp_instance = nullptr;

	App::App(HINSTANCE hInst, int startWindowWidth, int startWindowHeight) : m_window(hInst, L"Test")
	{
		ToggleDebugConsole(true);

		m_window.Create(startWindowWidth, startWindowHeight, MainWndProc);

		RenderManager::Init(&m_window);
	}

	App::~App()
	{
		mp_camera->Destroy();
		delete mp_camera;
		m_sceneManager.Destroy();

		RenderManager::UnInit();
		Input::UnInit();
	}

	void App::Destroy()
	{
		if (msp_instance)
		{
			delete msp_instance;
		}
	}

	App* App::Get(HINSTANCE hInst, int startWindowWidth, int startWindowHeight)
	{
		if (msp_instance == nullptr && hInst != 0)
		{
			msp_instance = new App(hInst, startWindowWidth, startWindowHeight);
			msp_instance->Init();
		}

		return msp_instance;
	}

	void App::Run()
	{
		Start();
		MainLoop();

		AudioManager::UnInit();
	}

	void App::LoadTexture(std::wstring path, size uniqueTag, bool usingTextureFolder)
	{
		m_texturesDataToLoad.push_back(TextureData{ path, uniqueTag, usingTextureFolder });
	}

	void App::ToggleDebugConsole(bool state)
	{
		if (state == true)
		{
			AllocConsole();
			FILE* fp;

			freopen_s(&fp, "CONOUT$", "w", stdout);
			freopen_s(&fp, "CONOUT$", "w", stderr);
			freopen_s(&fp, "CONIN$", "r", stdin);
		}
		else
		{
			FreeConsole();
		}
	}

	void App::SetAmbientLightColor(DirectX::XMFLOAT3 color)
	{
		m_ambientLightColor = color;
	}

	const DirectX::XMFLOAT3& App::GetAmbientLightColor() const
	{
		return m_ambientLightColor;
	}

	Window& App::GetWindow()
	{
		return m_window;
	}

	GameObject* App::GetCamera()
	{
		return mp_camera;
	}

	Ecs& App::GetEcs()
	{
		return m_ecs;
	}

	SceneManager& App::GetSceneManager()
	{
		return m_sceneManager;
	}

	AppChrono& App::GetChrono()
	{
		return m_chrono;
	}

	//Private

	void App::Init()
	{
		AudioManager::Init();

		m_chrono.SetPauseState(true);

		RENDER_MANAGER->InitDirectX3D();
		Resize();

		m_ecs.AddSystem<RenderSystem>();
		m_ecs.AddSystem<PhysicSystem>();
		m_ecs.AddSystem<CameraManagerSystem>();
		m_ecs.AddSystem<ParticleSystem>();
		m_ecs.AddSystem<UISystem>();
		m_ecs.AddSystem<BehaviorSystem>();
		m_ecs.AddSystem<LightManagerSystem>();
		m_ecs.AddSystem<StateMachineSystem>();

		CreateCamera();

		//Loading Screen first frame
		LoadTexture(L"../../src/nam_render/Font Sheets/pusab.dds", _FontPusab, false);
		FontManager::LoadFnt("Pusab", "../../src/nam_render/Font Sheets/pusab.fnt");

		Scene* p_loadingScene = m_sceneManager.CreateScene(size(-1));

		mp_loadingScreen = p_loadingScene->CreateGameObject<LoadingScreen>();

		p_loadingScene->StartGameObjects();
		SCENE_MANAGER.SetActiveScene(p_loadingScene->GetTag(), true);

		RENDER_MANAGER->BuildMinimal();

		RenderFirstFrame();
	}

	void App::Start()
	{
		RENDER_MANAGER->BuildOthers();
		LoadTextures();
		RENDER_MANAGER->EndInit();

		CameraDefaultSettings();
	}

	int App::MainLoop()
	{
		m_chrono.SetPauseState(false);
		MSG msg = { 0 };

		while (msg.message != WM_QUIT && m_isRunning)
		{
			// If there are Window messages then process them.
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Otherwise, do animation/game stuff.
			else
			{
				Update();

				Input::m_scrollDelta = 0.f;

				RENDER_MANAGER->BeginFrame();
				RENDER_MANAGER->UpdateCompute();
				RENDER_MANAGER->Draw();
				RENDER_MANAGER->EndFrame();
			}
		}

		return (int)msg.wParam;
	}

	void App::Update()
	{
		m_chrono.Update();
		AudioManager::UpdateSoundsPool();
		Input::Update();
		m_ecs.Update();

		m_sceneManager.Update();
		m_ecs.CleanUpdate();
	}

	void App::Resize()
	{
		if (mp_camera)
		{
			CameraComponent& cam = mp_camera->GetComponent<CameraComponent>();
			cam.UpdateAspectRatio(m_window.m_clientWidth, m_window.m_clientHeight);
		}

		RENDER_MANAGER->OnResize();
	}

	void App::CreateCamera()
	{
		if (mp_camera != nullptr)
		{
			delete mp_camera;
		}

		mp_camera = new GameObject();
		mp_camera->m_entity = m_ecs.CreateEntity();
		mp_camera->mp_scene = nullptr;

		mp_camera->AddComponent(TransformComponent());
		mp_camera->mp_transform = &mp_camera->GetComponent<TransformComponent>();

		mp_camera->AddComponent(CameraComponent());

		mp_camera->OnStart();
	}

	void App::CameraDefaultSettings()
	{
		if (mp_camera == nullptr)
			return;

		//si n'a aucun components
		if (mp_camera->HasComponent<TransformComponent>() == false)
		{
			//creer tout les components
			TransformComponent transform = TransformComponent();
			transform.SetWorldPosition(XMFLOAT3(0.f, 0.f, 0.f));
			mp_camera->AddComponent(transform);

			//save le component
			//mp_camera->mp_transform = &(mp_camera->GetComponent<TransformComponent>());
			mp_camera->AddComponent(CameraComponent());
		}

		TransformComponent& camTransform = mp_camera->GetComponent<TransformComponent>();

		XMFLOAT3 pos = { 0.f, 5.f, 0.f };

		camTransform.SetWorldPosition(pos);
		camTransform.SetWorldYPR(0.f, 0.f, 0.f);

	}

	void App::RenderFirstFrame()
	{
		//A first Update to push the RenderItem in the RenderSystem Draw
		Update();

		RENDER_MANAGER->BeginFrame();
		RENDER_MANAGER->Draw();
		RENDER_MANAGER->EndFirstFrame();
	}

	void App::LoadTextures()
	{
		for (TextureData& td : m_texturesDataToLoad)
		{
			RENDER_MANAGER->LoadTexture(td.m_path, td.m_uniqueTag, td.m_usingTextureFolder);
		}
	}
}
