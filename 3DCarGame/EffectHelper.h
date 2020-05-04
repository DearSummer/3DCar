#pragma once
#include <iostream>
#include <wrl/client.h>
#include <d3d11.h>


template<class Type>
struct Aligned
{
	void * operator new (const size_t size)
	{
		const size_t alignedSize = __alignof(Type);
		static_assert(alignedSize > 8, "AlignedNew is only useful for types with > 8 byte alignment! Did you forget a __declspec(align) on DerivedType?");
		void* ptr = _aligned_malloc(size, alignedSize);

		if (!ptr)
			throw std::bad_alloc();

		return ptr;
	}

	void operator delete(void *ptr)
	{
		_aligned_free(ptr);
	}
};


struct BasicBuffer
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	virtual ~BasicBuffer() = default;
protected:
	bool m_isDirty = false;
	ComPtr<ID3D11Buffer> m_pBuffer;

public:
	virtual HRESULT CreateBuffer(ID3D11Device * device) = 0;
	virtual void PostToPipeline(ID3D11DeviceContext * deviceContext) = 0;
	virtual void VSBindBuffer(ID3D11DeviceContext * deviceContext) = 0;
	virtual void PSBindBuffer(ID3D11DeviceContext * deviceContext) = 0;
};


template<UINT solt,class T>
class Buffer : public BasicBuffer
{
public:
	T m_data {};

	void PostValue(T* value);
	T Get();
	HRESULT CreateBuffer(ID3D11Device* device) override;
	void PostToPipeline(ID3D11DeviceContext* deviceContext) override;
	void VSBindBuffer(ID3D11DeviceContext* deviceContext) override;
	void PSBindBuffer(ID3D11DeviceContext* deviceContext) override;	
};

template <UINT solt, class T>
void Buffer<solt, T>::PostValue(T* value)
{
	m_data = *value;
	m_isDirty = true;
}

template <UINT solt, class T>
T Buffer<solt, T>::Get()
{
	return m_data;
}

template <UINT solt, class T>
HRESULT Buffer<solt, T>::CreateBuffer(ID3D11Device* device)
{
	if (m_pBuffer != nullptr)
		return S_OK;

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof cbd);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.ByteWidth = sizeof(T);
	return device->CreateBuffer(&cbd, nullptr, m_pBuffer.GetAddressOf());
}

template <UINT solt, class T>
void Buffer<solt, T>::PostToPipeline(ID3D11DeviceContext* deviceContext)
{
	if(m_isDirty)
	{
		m_isDirty = false;
		D3D11_MAPPED_SUBRESOURCE mappedData;
		deviceContext->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy_s(mappedData.pData, sizeof(T), &m_data, sizeof(T));
		deviceContext->Unmap(m_pBuffer.Get(), 0);
	}
}

template <UINT solt, class T>
void Buffer<solt, T>::VSBindBuffer(ID3D11DeviceContext* deviceContext)
{
	deviceContext->VSSetConstantBuffers(solt, 1, m_pBuffer.GetAddressOf());
}

template <UINT solt, class T>
void Buffer<solt, T>::PSBindBuffer(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(solt, 1, m_pBuffer.GetAddressOf());
}
