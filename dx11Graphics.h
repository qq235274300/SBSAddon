#pragma once
#include "dx11Common.h"
#include <wrl.h>

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dxgi.lib")

class dx11Graphics
{

public:
	dx11Graphics();

	void TryCreateSwapChainforWnd(api::device *device,HWND _hwnd);

public:
	bool GetCreateSwapChainState()const ;
private:
	bool CreateSwapChainSuccess = false;
	Microsoft::WRL::ComPtr<ID3D11Device1> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain;
};

