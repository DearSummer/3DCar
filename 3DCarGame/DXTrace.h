#pragma once

#include <windows.h>

HRESULT WINAPI DXTrace(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);



#if defined(DEBUG) || defined (_DEBUG)
#ifndef DX_DEBUG_TRACE
#define DX_DEBUG_TRACE(x) \
	{															\
		HRESULT hr = (x);										\
		if(FAILED(hr))											\
		{														\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);\
		}														\
	}
#endif

#else
#ifndef DX_DEBUG_TRACE
#define DX_DEBUG_TRACE(x) (x)
#endif
#endif