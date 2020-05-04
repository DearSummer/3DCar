#pragma once
#include "Light.h"
#include "DXTrace.h"

template<class T,UINT subresource = 0,D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD>
struct BasicBuffer
{
	void PostToPipeline(ID3D11DeviceContext* pDeviceContext,ID3D11Buffer *buffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		DX_DEBUG_TRACE(pDeviceContext->Map(buffer, subresource, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(T), this, sizeof(T));
		pDeviceContext->Unmap(buffer, 0);
	}
};

struct OnDrawingBuffer: BasicBuffer<OnDrawingBuffer>
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInvTranspose;
	Material mat;
};

struct OnDrawStateBuffer :BasicBuffer<OnDrawingBuffer>
{
	int isReflection;
	DirectX::XMFLOAT3 pad;
};

struct OnFrameChangeBuffer :BasicBuffer<OnFrameChangeBuffer>
{
	DirectX::XMMATRIX view;
	DirectX::XMFLOAT4 eyePos;
};

struct OnResizeBuffer: BasicBuffer<OnResizeBuffer>
{
	DirectX::XMMATRIX proj;
};

struct EnvironmentBuffer: BasicBuffer<EnvironmentBuffer>
{
	DirectX::XMMATRIX reflection;
	DirectionalLight dirLight;
};






