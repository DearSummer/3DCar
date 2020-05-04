#pragma once
#include <intsafe.h>
#include <DirectXMath.h>
#include "Geometry.h"
#include <wrl/client.h>
#include "Light.h"
#include "BasicEffect.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
	// 设置缓冲区
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device * device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	// 设置纹理
	void SetTexture(ID3D11ShaderResourceView * texture);
	void SetMaterial(const Material &mat);

	// 绘制
	virtual void Draw(ID3D11DeviceContext * deviceContext,BasicEffect& effect);

	void SetPosition(DirectX::XMFLOAT3 newPos);
	void SetRotation(DirectX::XMFLOAT3 newRot);
	void SetScale(DirectX::XMFLOAT3 newScale);

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	//设置矩阵
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);

	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;
	//获取旋转
	DirectX::XMFLOAT3 GetRotation() const;
	//获取变换
	DirectX::XMFLOAT3 GetScale() const;

protected:


	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool isDirty;
	DirectX::XMFLOAT4X4 m_worldMatrix;				    // 世界矩阵
	Material m_material{};                                // 材质
	ComPtr<ID3D11ShaderResourceView> m_pTexture;		// 纹理
	ComPtr<ID3D11Buffer> m_pVertexBuffer;				// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;				// 索引缓冲区
	UINT m_vertexStride;								// 顶点字节大小
	UINT m_indexCount;								    // 索引数目	


	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_scale;
};

template <class VertexType, class IndexType>
void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	m_vertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof vbd);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(meshData.vertexVec.size()) * m_vertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof InitData);
	InitData.pSysMem = meshData.vertexVec.data();
	device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf());


	// 设置索引缓冲区描述
	m_indexCount = static_cast<UINT>(meshData.indexVec.size());
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof ibd);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_indexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = meshData.indexVec.data();
	device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf());
}

