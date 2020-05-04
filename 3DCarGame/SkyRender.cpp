#include "SkyRender.h"
#include "WICTextureLoader.h"
#include "DXUtil.h"
#include "Geometry.h"
#include <minwinbase.h>
#include <minwinbase.h>


HRESULT SkyRender::Init(
	ID3D11Device * device, 
	ID3D11DeviceContext * context, 
	const std::vector<std::wstring>&cubemapFilename, 
	float skySphereRadius)
{
	m_pIndexBuffer.Reset();
	m_pVertexBuffer.Reset();
	m_pTextureCube.Reset();

	const HRESULT hr = DXUtil::CreateWICTexture2DCubeFromFile(
		device,context,cubemapFilename,nullptr,&m_pTextureCube);

	if (FAILED(hr))
		return hr;

	return InitResource(device, skySphereRadius);

}

ID3D11ShaderResourceView* SkyRender::GetTexture() const
{
	return m_pTextureCube.Get();
}

void SkyRender::Draw(ID3D11DeviceContext* context, SkyCubeEffect& effect, BasicCamera& camera)
{
	UINT strides[1] = { sizeof(DirectX::XMFLOAT3) };
	UINT offsets[1] = { 0 };
	context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), strides, offsets);
	context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	const DirectX::XMFLOAT3 pos = camera.GetPosition();
	effect.SetWorldViewProjMatrix(DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z) * camera.GetViewProjXM());
	effect.SetTextureCube(m_pTextureCube.Get());
	effect.Apply(context);
	context->DrawIndexed(m_indexCount, 0, 0);
}


HRESULT SkyRender::InitResource(ID3D11Device* device, float skyRadius)
{
	auto sphere = Geometry::CreateSphere<VertexPos>(skyRadius);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof vbd);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(DirectX::XMFLOAT3) * static_cast<UINT>(sphere.vertexVec.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = sphere.vertexVec.data();
	const HRESULT hr = device->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);


	if (FAILED(hr))
		return hr;

	m_indexCount = static_cast<UINT>(sphere.indexVec.size());
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof ibd);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * m_indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = sphere.indexVec.data();
	return device->CreateBuffer(&ibd, &initData, &m_pIndexBuffer);

}
