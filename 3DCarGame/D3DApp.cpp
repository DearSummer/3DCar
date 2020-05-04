#include "D3DApp.h"
#include <sstream>
#include <cassert>
#include <minwinbase.h>
#include "DXTrace.h"
#include "Mouse.h"
#include "RenderState.h"


namespace
{
	D3DApp *g_pD3DApp = nullptr;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pD3DApp->MsgProc(hwnd, uMsg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
	:m_instance(hInstance), 
	m_mainWnd(nullptr), 
	m_appPause(false), 
	m_minmized(false), 
	m_maxmized(false), 
	m_resizing(false), 
	m_enable4xMsaa(true), 
	m_4xMsaaQuality(0), 
	m_pDevice(nullptr), 
	m_pImmediateContext(nullptr),
	m_pSwapChain(nullptr), 
	m_pDepthStencilBuffer(nullptr), 
	m_pRenderTargetView(nullptr), 
	m_pDepthStencilView(nullptr), 
	m_mainWndCaption(L"DirectX11 Car Game"), 
	m_clientWidth(800), m_clientHeight(600)
{
	ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
	g_pD3DApp = this;
}

D3DApp::~D3DApp()
{
	if (m_pImmediateContext)
		m_pImmediateContext->ClearState();
}


HINSTANCE D3DApp::AppInstance() const
{
	return m_instance;
}

float D3DApp::AspectRatio() const
{
	return static_cast<float>(m_clientWidth) / m_clientHeight;
}

HWND D3DApp::MainWnd() const
{
	return m_mainWnd;
}


int D3DApp::Run()
{
	MSG msg = { };

	m_timer.Reset();
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick();
			if(!m_appPause)
			{
				CalcluateFrameStats();
				Update(m_timer.DetlaTime());
				DrawScene();			
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return static_cast<int>(msg.wParam);
}


bool D3DApp::Init()
{

	m_pKeyboard = std::make_unique<DirectX::Keyboard>();
	m_pMouse = std::make_unique<DirectX::Mouse>();
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	m_pMouse->SetWindow(m_mainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	return true;
}



bool D3DApp::InitMainWindow()
{

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"WindowClass";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	RECT r = { 0,0,m_clientWidth,m_clientHeight };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
	const int width = r.right - r.left;
	const int height = r.bottom - r.top;

	m_mainWnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		m_mainWndCaption.c_str(),                 // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,

		nullptr,       // Parent window    
		nullptr,       // Menu
		m_instance,     // Instance handle
		nullptr        // Additional application data
	);


	if (!m_mainWnd)
		return false;

	ShowWindow(m_mainWnd, SW_SHOW);
	UpdateWindow(m_mainWnd);

	return true;
}


LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		m_clientWidth = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);
		if (m_pDevice)
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
				m_appPause = true;
				m_minmized = true;
				m_maxmized = false;
				break;
			case SIZE_MAXIMIZED:
				m_appPause = false;
				m_minmized = false;
				m_maxmized = true;
				break;
			case SIZE_RESTORED:

				if (m_minmized)
				{
					m_appPause = false;
					m_minmized = false;
					Resize();
				}
				else if (m_maxmized)
				{
					m_appPause = false;
					m_maxmized = false;
					Resize();
				}
				else if (m_resizing) {}
				else
				{
					Resize();
				}
				break;
			default: break;
			}

		}
		return 0;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_appPause = true;
			m_timer.Stop();
		}
		else
		{
			m_appPause = false;
			m_timer.Start();
		}
		return 0;
	case WM_ENTERSIZEMOVE:
		m_appPause = true;
		m_resizing = true;
		m_timer.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		m_appPause = false;
		m_resizing = false;
		m_timer.Start();
		Resize();
		return 0;
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		m_pMouse->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_ACTIVATEAPP:
		m_pMouse->ProcessMessage(msg, wParam, lParam);
		m_pKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_pKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;
	default:
		break;

	}


	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool D3DApp::InitDirect3D()
{
	HRESULT hResult = S_OK;
	UINT createDeviceFlags = 0;

#if defined (DEBUG) || defined (_DEBUG) 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//d3d驱动类型
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_REFERENCE
	};

	//目标的d3d等级
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1
	};
	const UINT numFeatureLevel = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;
	for (auto driverType : driverTypes)
	{
		hResult = D3D11CreateDevice(
			nullptr,driverType, nullptr, createDeviceFlags, 
			featureLevels, numFeatureLevel, 
			D3D11_SDK_VERSION, 
			&m_pDevice, 
			&featureLevel, 
			&m_pImmediateContext);

		if (SUCCEEDED(hResult))
			break;
	}

	if (FAILED(hResult))
		return false;

	m_pDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;

	//buff的大小
	swapChainDesc.BufferDesc.Width = m_clientWidth;
	swapChainDesc.BufferDesc.Height = m_clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//刷新率60帧
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	//输出的设备
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_mainWnd;

	swapChainDesc.Windowed = true;

	if(m_enable4xMsaa)
	{
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	ComPtr<IDXGIFactory> dxgiFactory;
	ComPtr<IDXGIAdapter> dxgiAdapter;
	ComPtr<IDXGIDevice> dxgiDevice;

	DX_DEBUG_TRACE(m_pDevice.As(&dxgiDevice)) ;
	DX_DEBUG_TRACE(dxgiDevice->GetAdapter(&dxgiAdapter));
	DX_DEBUG_TRACE(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), &dxgiFactory));

	DX_DEBUG_TRACE(dxgiFactory->CreateSwapChain(m_pDevice.Get(), &swapChainDesc, &m_pSwapChain));
	

	Resize();
	return true;
}

void D3DApp::Resize()
{
	assert(m_pDevice);
	assert(m_pImmediateContext);
	assert(m_pSwapChain);

	m_pRenderTargetView.Reset();
	m_pDepthStencilBuffer.Reset();
	m_pDepthStencilView.Reset();

	ComPtr<ID3D11Texture2D> backBuffer;
	DX_DEBUG_TRACE(m_pSwapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	DX_DEBUG_TRACE(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));
	DX_DEBUG_TRACE(m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_pRenderTargetView));

	backBuffer.Reset();


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof desc);
	desc.Width = m_clientWidth;
	desc.Height = m_clientHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (m_enable4xMsaa)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	DX_DEBUG_TRACE(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pDepthStencilBuffer));
	DX_DEBUG_TRACE(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, &m_pDepthStencilView));

	m_pImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width = static_cast<float>(m_clientWidth);
	m_screenViewport.Height = static_cast<float>(m_clientHeight);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &m_screenViewport);



}


void D3DApp::CalcluateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;
	if((m_timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		const float fps = static_cast<float>(frameCnt);
		const float mspf = 1000.0f / fps;

		std::wostringstream outputStream;
		outputStream.precision(6);

		outputStream << m_mainWndCaption << L"  " 
			<< L"FPS: " << fps << L"  " 
			<< L"frame time: " << mspf << "ms";

		SetWindowText(m_mainWnd, outputStream.str().c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
