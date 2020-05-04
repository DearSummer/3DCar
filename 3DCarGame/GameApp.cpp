#include "GameApp.h"
#include <cassert>
#include "DXTrace.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "WICTextureLoader.h"
#include "Buffer.h"
#include "FPCamera.h"
#include "TPCamera.h"
#include "RenderState.h"

GameApp::GameApp(HINSTANCE hinstance)
	:D3DApp(hinstance),m_cameraMode(CameraMode::FirstPerson)
{
}


GameApp::~GameApp()
= default;

void GameApp::DrawScene()
{
	float blue[4] = { 0.0f,0.0f,0.0f,1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//镜面反射
	m_basicEffect.SetWriteStencilOnly(m_pImmediateContext.Get(), 1);
	m_ice.Draw(m_pImmediateContext.Get(),m_basicEffect);
	
	//不透明物体
	m_basicEffect.SetReflectionState(true);
	m_basicEffect.SetRenderDefaultWithStencil(m_pImmediateContext.Get(), 1);


	m_walls[2].Draw(m_pImmediateContext.Get(),m_basicEffect);
	m_walls[3].Draw(m_pImmediateContext.Get(),m_basicEffect);
	m_walls[4].Draw(m_pImmediateContext.Get(),m_basicEffect);
	m_floor.Draw(m_pImmediateContext.Get(),m_basicEffect);
	m_car.Draw(m_pImmediateContext.Get(), m_basicEffect);


	m_skyCubeEffect.SetRenderDefault(m_pImmediateContext.Get());
	m_pSkyRender->Draw(m_pImmediateContext.Get(), m_skyCubeEffect, *m_pCamera);

	//阴影
	m_car.SetCarMaterial(m_shadowMat);
	m_basicEffect.SetShadowState(true);
	m_basicEffect.SetRenderNoDoubleBlend(m_pImmediateContext.Get(), 1);

	m_car.Draw(m_pImmediateContext.Get(), m_basicEffect);


	m_basicEffect.SetShadowState(false);
	m_car.SetCarMaterial(m_defaultMat);



	//透明物体
	m_basicEffect.SetReflectionState(false);
	m_basicEffect.SetRenderAlphaBlendWithStencil(m_pImmediateContext.Get(), 1);
	m_ice.Draw(m_pImmediateContext.Get(), m_basicEffect);

	//不透明又不反射
	m_basicEffect.SetRenderDefault(m_pImmediateContext.Get());

	for(auto& wall:m_walls)
	{
		wall.Draw(m_pImmediateContext.Get(),m_basicEffect);
	}
	m_floor.Draw(m_pImmediateContext.Get(),m_basicEffect);
	m_car.Draw(m_pImmediateContext.Get(), m_basicEffect);

	//阴影
	m_car.SetCarMaterial(m_shadowMat);

	m_basicEffect.SetShadowState(true);
	m_basicEffect.SetRenderNoDoubleBlend(m_pImmediateContext.Get(), 0);
	m_car.Draw(m_pImmediateContext.Get(), m_basicEffect);

	
	m_basicEffect.SetShadowState(false);
	m_car.SetCarMaterial(m_defaultMat);


	m_skyCubeEffect.SetRenderDefault(m_pImmediateContext.Get());
	m_pSkyRender->Draw(m_pImmediateContext.Get(), m_skyCubeEffect, *m_pCamera);


	DX_DEBUG_TRACE(m_pSwapChain->Present(0, 0));
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	RenderState::Init(m_pDevice.Get());

	if (!m_basicEffect.Init(m_pDevice.Get()))
		return false;

	if (!m_skyCubeEffect.Init(m_pDevice.Get()))
		return false;

	if (!InitResource())
		return false;

	return true;
}

void GameApp::Resize()
{
	D3DApp::Resize();

	if(m_pCamera != nullptr)
	{
		m_pCamera->SetFrustum(DirectX::XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
		m_pCamera->SetViewPort(0.0f, 0.0f, static_cast<float>(m_clientWidth), static_cast<float>(m_clientHeight));
		m_basicEffect.SetProjMatrix(m_pCamera->GetProjXM());
	}
}


void GameApp::InitGameObject()
{
	//初始化游戏对象

	//材质
	Material mat{};
	mat.ambient = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	m_defaultMat = mat;
	m_shadowMat.ambient = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_shadowMat.diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	m_shadowMat.specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	//car
	ComPtr<ID3D11ShaderResourceView> pTexture;
	DX_DEBUG_TRACE(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), L"Texture\\awesomeface.png", nullptr, &pTexture));
	m_car.SetBuffer(m_pDevice.Get(), Geometry::CreateBox(2.0f,1.0f,3.0f));
	m_car.SetTexture(pTexture.Get());
	m_car.SetPosition({ 0.0f, 0.0f, 0.0f });
	m_car.SetCarMaterial(m_defaultMat);

	//wheel
	m_car.Init(m_pDevice.Get());
	
	

	//初始化地板
	DX_DEBUG_TRACE(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), L"Texture\\floor.png", nullptr, &pTexture));
	m_floor.SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(200.0f, 200.0f), DirectX::XMFLOAT2(50.0f, 50.0f)));
	m_floor.SetTexture(pTexture.Get());
	m_floor.SetPosition({ 0.0f, -1.0f, -90.0f });
	m_floor.SetMaterial(m_defaultMat);

	//初始化墙
	m_walls.resize(5);
	DX_DEBUG_TRACE(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), L"Texture\\wall.jpg", nullptr, &pTexture));
	for (int i = 0; i < 5; i++)
	{
		m_walls[i].SetTexture(pTexture.Get());
		m_walls[i].SetMaterial(m_defaultMat);
	}

	m_walls[0].SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(96.0f, 8.0f), DirectX::XMFLOAT2(28.0f, 2.0f)));
	m_walls[1].SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(96.0f, 8.0f), DirectX::XMFLOAT2(28.0f, 2.0f)));
	m_walls[2].SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(200.0f, 8.0f), DirectX::XMFLOAT2(50.0f, 2.0f)));
	m_walls[3].SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(200.0f, 8.0f), DirectX::XMFLOAT2(50.0f, 2.0f)));
	m_walls[4].SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(200.0f, 8.0f), DirectX::XMFLOAT2(50.0f, 2.0f)));

	m_walls[0].SetRotation({ -DirectX::XM_PIDIV2 ,0,0 });
	m_walls[0].SetPosition({ -52.0f,3.0f,10.0f });

	m_walls[1].SetRotation({ -DirectX::XM_PIDIV2 ,0,0 });
	m_walls[1].SetPosition({ 52.0f,3.0f,10.0f });

	m_walls[2].SetRotation({ -DirectX::XM_PIDIV2, 0, DirectX::XM_PIDIV2});
	m_walls[2].SetPosition({ 100.0f,3.0f,-90.0f });

	m_walls[3].SetRotation({ -DirectX::XM_PIDIV2 ,0,-DirectX::XM_PIDIV2 });
	m_walls[3].SetPosition({ -100.0f,3.0f,-90.0f });

	m_walls[4].SetRotation({ -DirectX::XM_PIDIV2,0,DirectX::XM_PI });
	m_walls[4].SetPosition({ 0.0f,3.0f,-190.0f });

	//初始化冰面
	mat.ambient = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mat.specular = DirectX::XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mat.reflect = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	DX_DEBUG_TRACE(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), L"Texture\\ice.jpg", nullptr, &pTexture));
	m_ice.SetBuffer(m_pDevice.Get(), Geometry::CreatePlane(DirectX::XMFLOAT2(8.0f, 8.0f), DirectX::XMFLOAT2(1.0f, 1.0f)));
	m_ice.SetTexture(pTexture.Get());
	m_ice.SetRotation({ -DirectX::XM_PIDIV2, 0, 0 });
	m_ice.SetPosition({ 0.0f, 3.0f, 10.0f });
	m_ice.SetMaterial(mat);

	//摄像机
	m_cameraMode = CameraMode::FirstPerson;
	auto camera = std::make_shared<FPCamera>();
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, static_cast<float>(m_clientWidth), static_cast<float>(m_clientHeight));
	camera->LookAt(DirectX::XMFLOAT3(), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	camera->SetFrustum(DirectX::XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	m_basicEffect.SetProjMatrix(m_pCamera->GetProjXM());
}


void GameApp::InitScene()
{
	//天空盒
	m_pSkyRender = std::make_unique<SkyRender>();
	DX_DEBUG_TRACE(m_pSkyRender->Init(m_pDevice.Get(),m_pImmediateContext.Get(),
		std::vector<std::wstring>{
		L"Texture\\sunset_posX.jpg", L"Texture\\sunset_negX.jpg",
			L"Texture\\sunset_posY.jpg", L"Texture\\sunset_negY.jpg",
			L"Texture\\sunset_posZ.jpg", L"Texture\\sunset_negZ.jpg" },
		5000.0f));

	m_basicEffect.SetTextureCube(m_pSkyRender->GetTexture());
	//反射
	m_basicEffect.SetReflectionMatrix(DirectX::XMMatrixReflect(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 10.0f)));

	// 稍微高一点位置以显示阴影
	m_basicEffect.SetShadowMatrix(DirectX::XMMatrixShadow(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), DirectX::XMVectorSet(0.0f, 100.0f, -100.0f, 1.0f)));
	m_basicEffect.SetRefShadowMatrix(DirectX::XMMatrixShadow(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), DirectX::XMVectorSet(0.0f, 100.0f, 100.0f, 1.0f)));

	// 环境光
	DirectionalLight dirLight{};
	dirLight.ambient = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.specular = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_basicEffect.SetDirLight(dirLight);
}

bool GameApp::InitResource()
{

	InitGameObject();
	InitScene();

	return true;
}




void GameApp::Update(float dt)
{
	DirectX::Mouse::State mouseState = m_pMouse->GetState();
	DirectX::Mouse::State lastMouseState = m_mouseTracker.GetLastState();

	DirectX::Keyboard::State keyState = m_pKeyboard->GetState();
	DirectX::Keyboard::State lastKeyState = m_keyboardTracker.GetLastState();
	m_keyboardTracker.Update(keyState);

	auto cam1st = std::dynamic_pointer_cast<FPCamera>(m_pCamera);
	auto cam3rd = std::dynamic_pointer_cast<TPCamera>(m_pCamera);

	if(m_cameraMode == CameraMode::FirstPerson)
	{
		 m_car.SetDirection(Car::Direction::STOP);
		 m_car.SetTurn(Car::Turn::NONE);
		 if (keyState.IsKeyDown(DirectX::Keyboard::W) && lastKeyState.IsKeyDown(DirectX::Keyboard::W))
		 {
		 	m_car.SetDirection(Car::Direction::FORWARD);
		 }
		 if (keyState.IsKeyDown(DirectX::Keyboard::S) && lastKeyState.IsKeyDown(DirectX::Keyboard::S))
		 {
		 	m_car.SetDirection(Car::Direction::BACK);
		 }
		 if (keyState.IsKeyDown(DirectX::Keyboard::A) && lastKeyState.IsKeyDown(DirectX::Keyboard::A))
		 {
		 	m_car.SetTurn(Car::Turn::LEFT);
		 }
		 if (keyState.IsKeyDown(DirectX::Keyboard::D) && lastKeyState.IsKeyDown(DirectX::Keyboard::D))
		 {
		 	m_car.SetTurn(Car::Turn::RIGHT);
		 }
  
		 m_car.Move();


		cam1st->LookTo(m_car.GetPosition(),
			m_car.GetForward(),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	}
	else if (m_cameraMode == CameraMode::ThirdPerson)
	{
		m_car.SetDirection(Car::Direction::STOP);
		m_car.SetTurn(Car::Turn::NONE);
		if (keyState.IsKeyDown(DirectX::Keyboard::W) && lastKeyState.IsKeyDown(DirectX::Keyboard::W))
		{
			m_car.SetDirection(Car::Direction::FORWARD);
		}
		if (keyState.IsKeyDown(DirectX::Keyboard::S) && lastKeyState.IsKeyDown(DirectX::Keyboard::S))
		{
			m_car.SetDirection(Car::Direction::BACK);
		}
		if (keyState.IsKeyDown(DirectX::Keyboard::A) && lastKeyState.IsKeyDown(DirectX::Keyboard::A))
		{
			m_car.SetTurn(Car::Turn::LEFT);
		}
		if (keyState.IsKeyDown(DirectX::Keyboard::D) && lastKeyState.IsKeyDown(DirectX::Keyboard::D))
		{
			m_car.SetTurn(Car::Turn::RIGHT);
		}

		m_car.Move();

		cam3rd->SetTarget(m_car.GetPosition());
		cam3rd->RotateX(mouseState.y  * 0.1f);
		cam3rd->RotateY(mouseState.x  * 0.1f);
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}


	m_pCamera->UpdateViewMatrix();
	m_basicEffect.SetViewMatrix(m_pCamera->GetViewXM());
	m_basicEffect.SetEyePos(m_pCamera->GetPositionXM());

	m_pMouse->ResetScrollWheelValue();

	if(m_keyboardTracker.IsKeyPressed(DirectX::Keyboard::D1) && m_cameraMode != CameraMode::FirstPerson)
	{
		if(!cam1st)
		{
			cam1st.reset(new FPCamera());
			cam1st->SetFrustum(DirectX::XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cam1st;
		}

		cam1st->LookTo(m_car.GetPosition(),
			m_car.GetForward(),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

		m_cameraMode = CameraMode::FirstPerson;
	}

	else if(m_keyboardTracker.IsKeyPressed(DirectX::Keyboard::D2) && m_cameraMode != CameraMode::ThirdPerson)
	{
		if(!cam3rd)
		{
			cam3rd.reset(new TPCamera());
			cam3rd->SetFrustum(DirectX::XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cam3rd;
		}

		DirectX::XMFLOAT3 target = m_car.GetPosition();
		cam3rd->SetTarget(target);
		cam3rd->SetDistance(8.0f);
		cam3rd->SetDistanceMinMax(3.0f, 20.0f);

		m_pCamera = cam3rd;
		m_cameraMode = CameraMode::ThirdPerson;
	}

	if (keyState.IsKeyDown(DirectX::Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);


}
