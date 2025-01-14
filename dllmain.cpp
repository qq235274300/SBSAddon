// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include "dx11Common.h"
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <assert.h>
#include <wrl.h>
#pragma comment(lib, "dxgi.lib")


using namespace reshade;

HINSTANCE g_hInstance = nullptr;
HWND g_hWnd = NULL;
BOOL g_bExit = FALSE;



#define  Factory1

struct __declspec(uuid("2FA5FB3D-7873-4E67-9DDA-5D449DB2CB47")) SBSRenderData
{
	Microsoft::WRL::ComPtr<ID3D11Device1> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain;

	bool swapChainCreated = false;

};



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	switch (msg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_SIZE:
	{
		break;
	}
	default:
		result = DefWindowProcW(hwnd, msg, wparam, lparam);
	}
	return result;
}

HWND CreateWindowInDLL()
{
	
	{
		WNDCLASSEXW winClass = {};
		winClass.cbSize = sizeof(WNDCLASSEXW);
		winClass.style = CS_HREDRAW | CS_VREDRAW;
		winClass.lpfnWndProc = &WndProc;
		winClass.hInstance = g_hInstance;
		winClass.lpszClassName = L"hahaClass";


		if (!RegisterClassExW(&winClass)) {
			MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
			return NULL;
		}

		RECT initialRect = { 0, 0, 1024, 768 };
		AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
		LONG initialWidth = initialRect.right - initialRect.left;
		LONG initialHeight = initialRect.bottom - initialRect.top;

		g_hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
								winClass.lpszClassName,
								L"SBS",
								WS_OVERLAPPEDWINDOW | WS_VISIBLE,
								CW_USEDEFAULT, CW_USEDEFAULT,
								initialWidth,
								initialHeight,
								0, 0, g_hInstance, 0);


		//不知道为什么但一定要加
		{ 
			ShowWindow(g_hWnd, SW_SHOWNORMAL);

			//Move it to bottom
			SetWindowPos(g_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			UpdateWindow(g_hWnd);
		}
	


		return g_hWnd;
	}
};

DWORD WINAPI WindowThread(LPVOID lpParam)
{
	// 创建窗口
	HWND hwnd = CreateWindowInDLL();

	// 启动消息循环
	MSG msg;
	while (!g_bExit && GetMessage(&msg, hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void TryCreateSwapChain(reshade::api::device *device)
{
	//创建私有数据
	
		HRESULT hResult;
		ID3D11Device1 *d3d11Device;
		ID3D11Device *baseDevice = reinterpret_cast<ID3D11Device *>(device->get_native());

		// 假设你已经有了设备和窗口句柄
		hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&d3d11Device);
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
		IDXGISwapChain1 *d3d11SwapChain;
		{
			// Get DXGI Factory (needed to create Swap Chain)
			IDXGIFactory2 *dxgiFactory;
			{
				IDXGIDevice1 *dxgiDevice;
				HRESULT hResult = d3d11Device->QueryInterface(__uuidof(IDXGIDevice1), (void **)&dxgiDevice);
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

			HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(d3d11Device, g_hWnd, &d3d11SwapChainDesc, 0, 0, &d3d11SwapChain);
			assert(SUCCEEDED(hResult));

			dxgiFactory->Release();
		
#endif
	}
	






}
static void on_init(reshade::api::device *device)
{

	//窗口在线程创建的，等待它创建完
	while (g_hWnd == NULL)
		Sleep(10);

	device->create_private_data<SBSRenderData>();
	//TryCreateSwapChain(device);
}


static void on_init_swapchain(reshade::api::swapchain *swapchain)
{
	
}

static void on_init_command_queue( reshade::api::command_queue *queue)
{
	
	
}

void on_present(api::command_queue *queue, api::swapchain *swapchain, const api::rect *source_rect, const api::rect *dest_rect, uint32_t dirty_rect_count, const api::rect *dirty_rects)
{
	SBSRenderData &devData = queue->get_device()->get_private_data <SBSRenderData>();
	if (devData.swapChainCreated == false)
	{
		TryCreateSwapChain(queue->get_device());
		devData.swapChainCreated = true;
	}
	
}

static void on_reshadepresent(api::effect_runtime *runtime)
{
	TryCreateSwapChain(runtime->get_device());

}

extern "C" __declspec(dllexport) const char *NAME = "SBS output";
extern "C" __declspec(dllexport) const char *DESCRIPTION = "Duplicate SBS screen into double width buffer and output to glass.";


HANDLE hThread = 0;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		g_hInstance = hModule;
		hThread = CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);

		if (!reshade::register_addon(hModule))
			return FALSE;

		
		reshade::register_event<reshade::addon_event::init_device>(on_init);
		reshade::register_event<reshade::addon_event::present>(on_present);
		//RESHADE_DEFINE_ADDON_EVENT_TRAITS(addon_event::reshade_present, void, api::effect_runtime * runtime);
		//reshade::register_event <reshade::addon_event::init_swapchain>(on_init_swapchain);
		//(addon_event::init_command_queue, void, api::command_queue *queue); 
		//reshade::register_event <reshade::addon_event::init_command_queue>(on_init_command_queue);
		
	}
	break;
		
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		/*g_bExit = TRUE;
		reshade::unregister_addon(hModule);*/
        break;
    }
    return TRUE;
}

