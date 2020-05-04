#include "BasicEffect.h"
#include "EffectHelper.h"
#include "Light.h"
#include <vector>
#include "RenderState.h"
#include "DXTrace.h"
#include "DXUtil.h"
#include "Vertex.h"

class BasicEffect::Impl : public Aligned<Impl>
{
	struct OnDrawingBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
		Material mat;
	};

	struct OnDrawStateBuffer
	{
		int isReflection;
		int isShadow;
		DirectX::XMFLOAT2 pad;
	};

	struct OnFrameChangeBuffer
	{
		DirectX::XMMATRIX view;
		DirectX::XMVECTOR eyePos;
	};

	struct OnResizeBuffer
	{
		DirectX::XMMATRIX proj;
	};

	struct EnvironmentBuffer
	{
		DirectX::XMMATRIX reflection;
		DirectX::XMMATRIX shadow;
		DirectX::XMMATRIX refShadow;
		DirectionalLight dirLight;
	};

	

public:
	Impl() : isDirty(false)
	{}

	~Impl() = default;

	Buffer<BUFFER_DRAWING, OnDrawingBuffer> drawBuffer;
	Buffer<BUFFER_DRAWING_STATE, OnDrawStateBuffer> drawStateBuffer;
	Buffer<BUFFER_FRAME_CHANGE, OnFrameChangeBuffer> frameBuffer;
	Buffer<BUFFER_RESIZE, OnResizeBuffer> resizeBuffer;
	Buffer<BUFFER_ENVIRONMENT, EnvironmentBuffer> enviromentBuffer;

	bool isDirty;
	std::vector<BasicBuffer*> pBufferVec;

	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3D11PixelShader> pPixelShader;

	ComPtr<ID3D11InputLayout> pVertexLayout;

	ComPtr<ID3D11ShaderResourceView> pTexture;
	ComPtr<ID3D11ShaderResourceView> pTextureCube;
};

namespace
{
	static BasicEffect *g_instance = nullptr;
}

BasicEffect::BasicEffect()
{
	if (g_instance)
		throw std::exception("BasicEffect is a singleton");
	g_instance = this;
	m_pImpl = std::make_unique<BasicEffect::Impl>();
}


BasicEffect::~BasicEffect()
= default;



BasicEffect::BasicEffect(BasicEffect&& moveFrom) noexcept
{
	m_pImpl.swap(moveFrom.m_pImpl);
}

BasicEffect& BasicEffect::operator=(BasicEffect&& moveFrom) noexcept
{
	m_pImpl.swap(moveFrom.m_pImpl);
	return *this;
}

BasicEffect& BasicEffect::Instance()
{
	if (!g_instance)
		throw std::exception("BasicEffect not instance");
	return *g_instance;
}

bool BasicEffect::Init(ID3D11Device* device)
{
	if (!device)
		return false;

	if (!m_pImpl->pBufferVec.empty())
		return true;

	if (!RenderState::IsInit())
		return false;


	ComPtr<ID3DBlob> blob;
	DX_DEBUG_TRACE(
		DXUtil::CreateShaderFromFile(L"Shader\\Triangle_VS.cso", L"Shader\\Triangle.vs",
			"VS", "vs_5_0", &blob));
	DX_DEBUG_TRACE(
		device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pImpl->pVertexShader));

	DX_DEBUG_TRACE(
		device->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
			blob->GetBufferPointer(), blob->GetBufferSize(), &m_pImpl->pVertexLayout)
	);


	DX_DEBUG_TRACE(
		DXUtil::CreateShaderFromFile(L"Shader\\Triangle_PS.cso", L"Shader\\Triangle.ps",
			"PS", "ps_5_0", &blob));
	DX_DEBUG_TRACE(
		device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pImpl->pPixelShader));


	m_pImpl->pBufferVec.assign(
		{
		&m_pImpl->drawBuffer,
		&m_pImpl->drawStateBuffer,
		&m_pImpl->frameBuffer,
		&m_pImpl->resizeBuffer,
		&m_pImpl->enviromentBuffer }
	);


	for(auto& b : m_pImpl->pBufferVec)
	{
		DX_DEBUG_TRACE(
			b->CreateBuffer(device));
	}

	return true;
}

void BasicEffect::SetRenderDefault(ID3D11DeviceContext* deviceContext) const
{
	SetInputLayout(deviceContext);
	deviceContext->RSSetState(nullptr);
	SetPSSampler(deviceContext, RenderState::s_linearWrap.GetAddressOf());


	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void BasicEffect::SetInputLayout(ID3D11DeviceContext* deviceContext) const
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(m_pImpl->pVertexLayout.Get());
	deviceContext->VSSetShader(m_pImpl->pVertexShader.Get(), nullptr, 0);
}



void BasicEffect::SetPSSampler(ID3D11DeviceContext* deviceContext,ID3D11SamplerState *const* sampler) const
{
	deviceContext->PSSetShader(m_pImpl->pPixelShader.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, sampler);
}

void BasicEffect::SetRenderAlphaBlend(ID3D11DeviceContext* deviceContext) const
{
	SetInputLayout(deviceContext);

	deviceContext->RSSetState(RenderState::s_noCull.Get());
	SetPSSampler(deviceContext,RenderState::s_linearWrap.GetAddressOf());

	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(RenderState::s_transparent.Get(), nullptr, 0xFFFFFFFF);
}

void BasicEffect::SetRenderNoDoubleBlend(ID3D11DeviceContext* deviceContext, UINT stencilRef) const
{
	SetInputLayout(deviceContext);
	deviceContext->RSSetState(RenderState::s_noCull.Get());
	SetPSSampler(deviceContext, RenderState::s_linearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderState::s_noDoubleBlend.Get(), stencilRef);
	deviceContext->OMSetBlendState(RenderState::s_transparent.Get(), nullptr, 0xFFFFFFFF);
}

void BasicEffect::SetWriteStencilOnly(ID3D11DeviceContext* deviceContext, UINT stencilRef) const
{
	SetInputLayout(deviceContext);
	deviceContext->RSSetState(nullptr);
	SetPSSampler(deviceContext, RenderState::s_linearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderState::s_writeStencil.Get(), stencilRef);
	deviceContext->OMSetBlendState(RenderState::s_noColorWrite.Get(), nullptr, 0xFFFFFFFF);
}

void BasicEffect::SetRenderDefaultWithStencil(ID3D11DeviceContext* deviceContext, UINT stencilRef) const
{
	SetInputLayout(deviceContext);
	deviceContext->RSSetState(RenderState::s_cullClockWise.Get());
	SetPSSampler(deviceContext, RenderState::s_linearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderState::s_drawWithStencil.Get(), stencilRef);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void BasicEffect::SetRenderAlphaBlendWithStencil(ID3D11DeviceContext* deviceContext, UINT stencilRef) const
{
	SetInputLayout(deviceContext);
	deviceContext->RSSetState(RenderState::s_noCull.Get());
	SetPSSampler(deviceContext, RenderState::s_linearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderState::s_drawWithStencil.Get(), stencilRef);
	deviceContext->OMSetBlendState(RenderState::s_transparent.Get(), nullptr, 0xFFFFFFFF);	
}

void BasicEffect::SetWorldMatrix(DirectX::FXMMATRIX W) const
{
	auto buffer = m_pImpl->drawBuffer.Get();
	buffer.world = DirectX::XMMatrixTranspose(W);
	buffer.worldInvTranspose = DirectX::XMMatrixInverse(nullptr, W);
	m_pImpl->drawBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetViewMatrix(DirectX::FXMMATRIX V) const
{
	auto buffer = m_pImpl->frameBuffer.Get();
	buffer.view = DirectX::XMMatrixTranspose(V);
	m_pImpl->frameBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetProjMatrix(DirectX::FXMMATRIX P) const
{
	auto buffer = m_pImpl->resizeBuffer.Get();
	buffer.proj = DirectX::XMMatrixTranspose(P);
	m_pImpl->resizeBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetReflectionMatrix(DirectX::FXMMATRIX R) const
{
	auto buffer = m_pImpl->enviromentBuffer.Get();
	buffer.reflection = DirectX::XMMatrixTranspose(R);
	m_pImpl->enviromentBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetShadowMatrix(DirectX::FXMMATRIX S) const
{
	auto buffer = m_pImpl->enviromentBuffer.Get();
	buffer.shadow = DirectX::XMMatrixTranspose(S);
	m_pImpl->enviromentBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetRefShadowMatrix(DirectX::FXMMATRIX RefS) const
{
	auto buffer = m_pImpl->enviromentBuffer.Get();
	buffer.refShadow = DirectX::XMMatrixTranspose(RefS);
	m_pImpl->enviromentBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetDirLight(const DirectionalLight& dirLight) const
{
	auto buffer = m_pImpl->enviromentBuffer.Get();
	buffer.dirLight = dirLight;
	m_pImpl->enviromentBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetMaterial(const Material& material) const
{
	auto buffer = m_pImpl->drawBuffer.Get();
	buffer.mat = material;
	m_pImpl->drawBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetTexture(ID3D11ShaderResourceView* texture) const
{
	m_pImpl->pTexture = texture;
}

void BasicEffect::SetEyePos(DirectX::FXMVECTOR eyePos) const
{
	auto buffer = m_pImpl->frameBuffer.Get();
	buffer.eyePos = eyePos;
	m_pImpl->frameBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetTextureCube(ID3D11ShaderResourceView* textureCube) const
{
	m_pImpl->pTextureCube = textureCube;
}

void BasicEffect::SetReflectionState(bool isOn) const
{
	auto buffer = m_pImpl->drawStateBuffer.Get();
	buffer.isReflection = isOn;
	m_pImpl->drawStateBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::SetShadowState(bool isOn) const
{
	auto buffer = m_pImpl->drawStateBuffer.Get();
	buffer.isShadow = isOn;
	m_pImpl->drawStateBuffer.PostValue(&buffer);
	m_pImpl->isDirty = true;
}

void BasicEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	auto& pBufferVec = m_pImpl->pBufferVec;
	pBufferVec[BUFFER_DRAWING]->VSBindBuffer(deviceContext);
	pBufferVec[BUFFER_DRAWING_STATE]->VSBindBuffer(deviceContext);
	pBufferVec[BUFFER_FRAME_CHANGE]->VSBindBuffer(deviceContext);
	pBufferVec[BUFFER_RESIZE]->VSBindBuffer(deviceContext);
	pBufferVec[BUFFER_ENVIRONMENT]->VSBindBuffer(deviceContext);

	pBufferVec[BUFFER_DRAWING]->PSBindBuffer(deviceContext);
	pBufferVec[BUFFER_DRAWING_STATE]->PSBindBuffer(deviceContext);
	pBufferVec[BUFFER_FRAME_CHANGE]->PSBindBuffer(deviceContext);
	pBufferVec[BUFFER_ENVIRONMENT]->PSBindBuffer(deviceContext);


	deviceContext->PSSetShaderResources(0, 1, m_pImpl->pTexture.GetAddressOf());
	deviceContext->PSSetShaderResources(1, 1, m_pImpl->pTextureCube.GetAddressOf());
	if(m_pImpl->isDirty)
	{
		m_pImpl->isDirty = false;
		for (auto& b : pBufferVec)
		{
			b->PostToPipeline(deviceContext);
		}
	}

}




