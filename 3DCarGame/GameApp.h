#pragma once
#include "D3DApp.h"
#include "Light.h"
#include "Geometry.h"
#include "BasicCamera.h"
#include "GameObject.h"
#include "Car.h"
#include "BasicEffect.h"
#include "SkyCubeEffect.h"
#include "SkyRender.h"


class GameApp : public D3DApp
{


	enum CameraMode
	{
		FirstPerson,
		ThirdPerson
	};



public:
	explicit GameApp(HINSTANCE hinstance);
	~GameApp();

	void Update(float dt) override;
	void DrawScene() override;
	bool Init() override;
	void Resize() override;

private:

	bool InitResource();
	void InitGameObject();
	void InitScene();

	Car m_car;
	GameObject m_floor;
	GameObject m_ice;

	std::vector<GameObject> m_walls;

	Material m_shadowMat{};
	Material m_defaultMat{};

	BasicEffect m_basicEffect;
	SkyCubeEffect m_skyCubeEffect;
	std::unique_ptr<SkyRender> m_pSkyRender;

	std::shared_ptr<BasicCamera> m_pCamera;
	CameraMode m_cameraMode;
};

