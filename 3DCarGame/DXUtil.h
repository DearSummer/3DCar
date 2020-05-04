#pragma once
#include <windows.h>
#include <string>
#include "WICTextureLoader.h"
#include <vector>

struct ID3D11DeviceContext;
struct ID3D11Device;

namespace DXUtil
{

	template<class D3DObject>
	void SafeDelete(D3DObject *obj)
	{
		if(obj)
		{
			obj->Release();
			obj = nullptr;
		}
	}

	HRESULT CreateShaderFromFile(
		const WCHAR* csoFileNameOut,
		const WCHAR* hlslFileName,
		LPCSTR entryPoint,
		LPCSTR shaderModel,
		ID3DBlob **ppBlobOut
	);

	HRESULT CreateWICTexture2DCubeFromFile(
		ID3D11Device * d3dDevice,
		ID3D11DeviceContext * d3dDeviceContext,
		const std::vector<std::wstring>& cubeMapFileNames,
		ID3D11Texture2D** textureArray,
		ID3D11ShaderResourceView** textureCubeView);
}
