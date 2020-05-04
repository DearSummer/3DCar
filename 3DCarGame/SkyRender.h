#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include "SkyCubeEffect.h"
#include "BasicCamera.h"

struct ID3D11Device;

class SkyRender
{
public:

	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	SkyRender() = default;
	~SkyRender() = default;
	// 不允许拷贝，允许移动
	SkyRender(const SkyRender&) = delete;
	SkyRender& operator=(const SkyRender&) = delete;
	SkyRender(SkyRender&&) = default;
	SkyRender& operator=(SkyRender&&) = default;


	HRESULT Init(ID3D11Device *device,
		ID3D11DeviceContext *context,
		const std::vector<std::wstring>& cubemapFilename,
		float skySphereRadius);


	ID3D11ShaderResourceView *GetTexture() const;
	void Draw(ID3D11DeviceContext *context,SkyCubeEffect& effect,BasicCamera& camera);

private:
	HRESULT InitResource(ID3D11Device* device, float skyRadius);

	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;

	UINT m_indexCount;
	ComPtr<ID3D11ShaderResourceView> m_pTextureCube;
};

