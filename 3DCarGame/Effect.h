#pragma once
#include <wrl/client.h>

struct ID3D11DeviceContext;

class IEffect
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IEffect() = default;
	IEffect(const IEffect&) = delete;
	IEffect& operator=(const IEffect&) = delete;

	IEffect(IEffect&& moveFrom) = default;
	IEffect& operator=(IEffect&& moveFrom) = default;

	virtual ~IEffect() = default;

	//更新并绑定常量缓冲区
	virtual void Apply(ID3D11DeviceContext * deviceContext) = 0;
};
