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
	~dx11Graphics();
	void TryCreateSwapChainforWnd(api::device *device,HWND _hwnd);
	void TryPresent();
	void CheckRenderTargetValid();
public:
	bool GetCreateSwapChainState()const ;
private:
	bool win32CreateD3D11RenderTargets(ID3D11Device1 *d3d11Device, IDXGISwapChain1 *swapChain);
private:
	bool CreateSwapChainSuccess = false;
	bool CanPresent = false;
	Microsoft::WRL::ComPtr<ID3D11Device1> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	UINT numVerts;
	UINT stride;
	UINT offset;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
};

