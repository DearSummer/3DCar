#pragma once
#include "Effect.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <memory>


#define BUFFER_DRAWING 0
#define BUFFER_DRAWING_STATE 1
#define BUFFER_FRAME_CHANGE 2
#define BUFFER_RESIZE 3
#define BUFFER_ENVIRONMENT 4

struct Material;
struct DirectionalLight;

class BasicEffect :public IEffect
{
public:
	BasicEffect();
	virtual ~BasicEffect() override;

	BasicEffect(BasicEffect&& moveFrom) noexcept;
	BasicEffect& operator=(BasicEffect&& moveFrom) noexcept;

	static BasicEffect& Instance();

	bool Init(ID3D11Device *device);

//
// 渲染模式的变更
//


private:
	
	void SetInputLayout(ID3D11DeviceContext* deviceContext) const;
	void SetPSSampler(ID3D11DeviceContext* deviceContext,ID3D11SamplerState *const* sampler) const;
public:
	// 默认状态来绘制
	void SetRenderDefault(ID3D11DeviceContext * deviceContext) const;
	// Alpha混合绘制
	void SetRenderAlphaBlend(ID3D11DeviceContext * deviceContext) const;
	// 无二次混合
	void SetRenderNoDoubleBlend(ID3D11DeviceContext * deviceContext, UINT stencilRef) const;
	// 仅写入模板值
	void SetWriteStencilOnly(ID3D11DeviceContext * deviceContext, UINT stencilRef) const;
	// 对指定模板值的区域进行绘制，采用默认状态
	void SetRenderDefaultWithStencil(ID3D11DeviceContext * deviceContext, UINT stencilRef) const;
	// 对指定模板值的区域进行绘制，采用Alpha混合
	void SetRenderAlphaBlendWithStencil(ID3D11DeviceContext * deviceContext, UINT stencilRef) const;

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W) const;
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V) const;
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P) const;

	void XM_CALLCONV SetReflectionMatrix(DirectX::FXMMATRIX R) const;
	void XM_CALLCONV SetShadowMatrix(DirectX::FXMMATRIX S) const;
	void XM_CALLCONV SetRefShadowMatrix(DirectX::FXMMATRIX RefS) const;

	//
	// 光照、材质和纹理相关设置
	//

	void SetDirLight(const DirectionalLight& dirLight) const;
	void SetMaterial(const Material& material) const;
	void SetTexture(ID3D11ShaderResourceView * texture) const;
	void XM_CALLCONV SetEyePos(DirectX::FXMVECTOR eyePos) const;

	void SetTextureCube(ID3D11ShaderResourceView *textureCube) const;

	//
	// 状态开关设置
	//
	void SetReflectionState(bool isOn) const;
	void SetShadowState(bool isOn) const;


	// 应用常量缓冲区和纹理资源的变更
	void Apply(ID3D11DeviceContext * deviceContext) override;

private :
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

