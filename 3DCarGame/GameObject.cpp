#include "GameObject.h"
#include "Buffer.h"


GameObject::GameObject() : isDirty(false),
                           m_vertexStride(0), m_indexCount(0),
                           m_position({0, 0, 0}),
                           m_rotation({0, 0, 0}),
                           m_scale({1, 1, 1})

{}

GameObject::~GameObject()
= default;

DirectX::XMFLOAT3 GameObject::GetPosition() const
{
	return m_position;
}

DirectX::XMFLOAT3 GameObject::GetRotation() const
{
	return m_rotation;
}

DirectX::XMFLOAT3 GameObject::GetScale() const
{
	return m_scale;
}

void GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	DirectX::XMStoreFloat4x4(&m_worldMatrix, world);
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_worldMatrix = world;
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}

void GameObject::SetMaterial(const Material& mat)
{
	m_material = mat;
}



void GameObject::Draw(ID3D11DeviceContext* deviceContext,BasicEffect& effect)
{
	UINT strides = m_vertexStride;
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offset);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	ComPtr<ID3D11Buffer> cbuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, &cbuffer);

	if(isDirty)
	{
		isDirty = false;
		const DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
		const DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
		const DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		SetWorldMatrix(rotationMat * scaleMat * translationMat);
	}

	// const DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	// const DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	// const DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	
	effect.SetWorldMatrix(DirectX::XMLoadFloat4x4(&m_worldMatrix));
	effect.SetTexture(m_pTexture.Get());
	effect.SetMaterial(m_material);
	effect.Apply(deviceContext);


	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void GameObject::SetPosition(DirectX::XMFLOAT3 newPos)
{
	isDirty = true;
	m_position = newPos;
}

void GameObject::SetRotation(DirectX::XMFLOAT3 newRot)
{
	isDirty = true;
	m_rotation = newRot;
}

void GameObject::SetScale(DirectX::XMFLOAT3 newScale)
{
	isDirty = true;
	m_scale = newScale;
}






