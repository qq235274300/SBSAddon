#include "dx11Graphics.h"

dx11Graphics::dx11Graphics()
	:CreateSwapChainSuccess(false)
{
}

void dx11Graphics::TryCreateSwapChainforWnd(api::device *device,  HWND _hwnd)
{
	HRESULT hResult;
	
	ID3D11Device *baseDevice = reinterpret_cast<ID3D11Device *>(device->get_native());

	// 假设你已经有了设备和窗口句柄
	hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&pDevice);
	assert(SUCCEEDED(hResult));


	//// 1. 创建一个 DXGI_SWAP_CHAIN_DESC 结构
	//DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	//swapChainDesc.BufferCount = 2;                      // 双缓冲
	//swapChainDesc.BufferDesc.Width = 1044;               // 窗口宽度
	//swapChainDesc.BufferDesc.Height = 811;              // 窗口高度
	//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 背景缓冲区格式
	//swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // 刷新率
	//swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // 用作渲染目标
	//swapChainDesc.OutputWindow = g_hWnd;                    // 目标窗口
	//swapChainDesc.SampleDesc.Count = 1;                   // 不进行抗锯齿
	//swapChainDesc.SampleDesc.Quality = 0;
	//swapChainDesc.Windowed = TRUE;                        // 窗口模式

#ifdef Factory0
	{
		// IDXGIFactory
		IDXGIFactory *dxgiFactory = nullptr;
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
		if (FAILED(hr)) {
			// 错误处理
		}
		// 3. 创建交换链
		IDXGISwapChain *swapChain = nullptr;
		hr = dxgiFactory->CreateSwapChain(d3d11Device, &swapChainDesc, &swapChain);

		if (FAILED(hr)) {
			// 错误处理
		}
		// 4. 清理工厂对象
		dxgiFactory->Release();
	}

#else
	
	{
		// Get DXGI Factory (needed to create Swap Chain)
		IDXGIFactory2 *dxgiFactory;
		{
			IDXGIDevice1 *dxgiDevice;
			HRESULT hResult = pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);
			assert(SUCCEEDED(hResult));

			IDXGIAdapter *dxgiAdapter;
			hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
			assert(SUCCEEDED(hResult));
			dxgiDevice->Release();

			DXGI_ADAPTER_DESC adapterDesc;
			dxgiAdapter->GetDesc(&adapterDesc);

			OutputDebugStringA("Graphics Device: ");
			OutputDebugStringW(adapterDesc.Description);

			hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&dxgiFactory);
			assert(SUCCEEDED(hResult));
			dxgiAdapter->Release();
		}

		DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {};
		d3d11SwapChainDesc.Width = 1044; // use window width 1044
		d3d11SwapChainDesc.Height = 811; // use window height811
		d3d11SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		d3d11SwapChainDesc.SampleDesc.Count = 1;
		d3d11SwapChainDesc.SampleDesc.Quality = 0;
		d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		d3d11SwapChainDesc.BufferCount = 2;
		d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH; //DXGI_SCALING_NONE  DXGI_SCALING_STRETCH
		d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;// DXGI_ALPHA_MODE_IGNORE DXGI_ALPHA_MODE_UNSPECIFIED
		d3d11SwapChainDesc.Flags = 0;

		HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(pDevice.Get(), _hwnd, &d3d11SwapChainDesc, 0, 0, &pSwapChain);
		assert(SUCCEEDED(hResult));
		
		dxgiFactory->Release();

		CreateSwapChainSuccess = true;

#endif
	}



}

bool dx11Graphics::GetCreateSwapChainState() const
{
	return CreateSwapChainSuccess;
}
