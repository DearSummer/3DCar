#pragma once
#include "Effect.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

class SkyCubeEffect :public IEffect
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	SkyCubeEffect();
	virtual ~SkyCubeEffect() override;

	SkyCubeEffect(SkyCubeEffect&& moveFrom) noexcept;
	SkyCubeEffect& operator=(SkyCubeEffect&& moveFrom) noexcept;

	static SkyCubeEffect& Instance();

	bool Init(ID3D11Device *device);

	void SetRenderDefault(ID3D11DeviceContext *deviceContext) const;

	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P) const;
	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP) const;

	void SetTextureCube(ID3D11ShaderResourceView * textureCube) const;

	void Apply(ID3D11DeviceContext* deviceContext) override;

private:
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

