#pragma once
#include "GameTimer.h"
#include "Mouse.h"


#include <wrl/client.h>
#include <string>
#include <d3d11.h>
#include "Keyboard.h"


class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInstance() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	int Run();

	virtual bool Init();
	virtual void Resize();
	virtual void Update(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalcluateFrameStats();

protected:

	HINSTANCE m_instance;
	HWND m_mainWnd;
	bool m_appPause;
	bool m_minmized;
	bool m_maxmized;
	bool m_resizing;
	bool m_enable4xMsaa;
	UINT m_4xMsaaQuality;

	GameTimer m_timer;

	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;

	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	D3D11_VIEWPORT m_screenViewport;

	std::wstring m_mainWndCaption;
	int m_clientWidth;
	int m_clientHeight;

	//键鼠输入
	std::unique_ptr<DirectX::Mouse> m_pMouse;
	DirectX::Mouse::ButtonStateTracker m_mouseTracker;
	std::unique_ptr<DirectX::Keyboard> m_pKeyboard;
	DirectX::Keyboard::KeyboardStateTracker m_keyboardTracker;

};
