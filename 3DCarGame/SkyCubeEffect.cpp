#include "SkyCubeEffect.h"
#include "EffectHelper.h"
#include <vector>
#include "RenderState.h"
#include "DXTrace.h"
#include "DXUtil.h"
#include "Vertex.h"


class SkyCubeEffect::Impl : public Aligned<Impl>
{
	struct OnFrameChange
	{
		DirectX::XMMATRIX worldViewProj;
	};

public:

	Impl():isDirty(false){}

	bool isDirty;
	std::vector<BasicBuffer *> bufferVec;


	Buffer<0, OnFrameChange> onFrameChangeBuffer;

	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3D11PixelShader> pPixelShader;
	ComPtr<ID3D11InputLayout> pInputLayout;

	ComPtr<ID3D11ShaderResourceView> pTextureCube;
};

namespace
{
	static SkyCubeEffect *g_instance = nullptr;
}

SkyCubeEffect::SkyCubeEffect()
{
	if(g_instance)
		throw std::exception("SkyEffect is a singleton!");

	g_instance = this;
	m_pImpl = std::make_unique<Impl>();
}


SkyCubeEffect::~SkyCubeEffect()
= default;

SkyCubeEffect::SkyCubeEffect(SkyCubeEffect&& moveFrom) noexcept
{
	m_pImpl.swap(moveFrom.m_pImpl);
}

SkyCubeEffect& SkyCubeEffect::operator=(SkyCubeEffect&& moveFrom) noexcept
{
	m_pImpl.swap(moveFrom.m_pImpl);
	return *this;
}


SkyCubeEffect& SkyCubeEffect::Instance()
{
	if(!g_instance)
		throw std::exception("SkyEffect needs an instance!");
	return *g_instance;
}

bool SkyCubeEffect::Init(ID3D11Device* device)
{
	if (!device)
		return false;

	if (!m_pImpl->bufferVec.empty())
		return true;

	if(!RenderState::IsInit())
		throw std::exception("RenderStates need to be initialized first!");

	ComPtr<ID3DBlob> blob;
	DX_DEBUG_TRACE(
		DXUtil::CreateShaderFromFile(L"Shader\\SkyCube_VS.cso", L"Shader\\SkyCube.vs",
			"VS", "vs_5_0", &blob));
	DX_DEBUG_TRACE(
		device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pImpl->pVertexShader));

	DX_DEBUG_TRACE(
		device->CreateInputLayout(VertexPos::inputLayout, ARRAYSIZE(VertexPos::inputLayout),
			blob->GetBufferPointer(), blob->GetBufferSize(), &m_pImpl->pInputLayout)
	);


	DX_DEBUG_TRACE(
		DXUtil::CreateShaderFromFile(L"Shader\\SkyCube_PS.cso", L"Shader\\SkyCube.ps",
			"PS", "ps_5_0", &blob));
	DX_DEBUG_TRACE(
		device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pImpl->pPixelShader));



	m_pImpl->bufferVec.assign(
		{&m_pImpl->onFrameChangeBuffer}
	);

	for (auto& b : m_pImpl->bufferVec)
	{
		DX_DEBUG_TRACE(
			b->CreateBuffer(device));
	}

	return true;

}

void SkyCubeEffect::SetRenderDefault(ID3D11DeviceContext* deviceContext) const
{
	deviceContext->IASetInputLayout(m_pImpl->pInputLayout.Get());
	deviceContext->VSSetShader(m_pImpl->pVertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pImpl->pPixelShader.Get(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(RenderState::s_noCull.Get());

	deviceContext->PSSetSamplers(0, 1, RenderState::s_linearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderState::s_lessEqual.Get(), 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void SkyCubeEffect::SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P) const
{
	SetWorldViewProjMatrix(W * V * P);
}

void SkyCubeEffect::SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP) const
{
	auto b = m_pImpl->onFrameChangeBuffer.Get();
	b.worldViewProj = DirectX::XMMatrixTranspose(WVP);
	m_pImpl->isDirty = true;
	m_pImpl->onFrameChangeBuffer.PostValue(&b);
}

void SkyCubeEffect::SetTextureCube(ID3D11ShaderResourceView* textureCube) const
{
	m_pImpl->pTextureCube = textureCube;
}

void SkyCubeEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	auto& pBuffer = m_pImpl->bufferVec;
	pBuffer[0]->VSBindBuffer(deviceContext);

	deviceContext->PSSetShaderResources(0, 1, m_pImpl->pTextureCube.GetAddressOf());

	if(m_pImpl->isDirty)
	{
		m_pImpl->isDirty = true;
		for(auto& b : pBuffer)
		{
			b->PostToPipeline(deviceContext);
		}
	}
}

