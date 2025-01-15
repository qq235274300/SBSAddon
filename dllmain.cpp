// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include "dx11Graphics.h"
#include <stdio.h>
#include <sstream>
HINSTANCE g_hInstance = nullptr;
HWND g_hWnd = NULL;
BOOL g_bExit = FALSE;
HANDLE hThread = 0;
static int count = 0;

reshade::api::device* g_device;


#define  Factory1

struct __declspec(uuid("2FA5FB3D-7873-4E67-9DDA-5D449DB2CB47")) SBSRenderData
{
	dx11Graphics _dx11Graphics;
	bool initSwapChainSuccess = false;
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
	//count++;
	//char message[256]; // 缓冲区存储格式化后的消息
	//snprintf(message, sizeof(message), "RegisterClassEx failed, count = %d", count);
	//MessageBoxA(0, message, "Fatal Error", MB_OK);

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
			SetWindowPos(g_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

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


static void on_init(reshade::api::device *device)
{
	reshade::unregister_event<reshade::addon_event::init_device>(on_init);
	g_device = device;
	g_device->create_private_data<SBSRenderData>();

	hThread = CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);

	//窗口在线程创建的，等待它创建完
	while (g_hWnd == NULL)
		Sleep(10);

	count++;
	reshade::log::message(reshade::log::level::info, ("on_init countNumber: " + std::to_string(count)).c_str());

	SBSRenderData &devData = g_device->get_private_data<SBSRenderData>();
	devData._dx11Graphics.MoveToNextEvent();
	
}


static void on_init_swapchain(reshade::api::swapchain *swapchain)
{
	SBSRenderData &devData = g_device->get_private_data<SBSRenderData>();
	
	////初始化数据
	reshade::unregister_event<reshade::addon_event::init_swapchain>(on_init_swapchain);
	
	devData._dx11Graphics.TryCreateSwapChainforWnd(swapchain->get_device(), g_hWnd);
	devData._dx11Graphics.MoveToNextEvent();

	
	count++;
	reshade::log::message(reshade::log::level::info, ("on_init_swapchain countNumber: " + std::to_string(count)).c_str());
	
	
}


static bool on_create_swapchain(reshade::api::swapchain_desc &desc, void *)
{
	count++;
	reshade::log::message(reshade::log::level::info, ("on_create_swapchain countNumber: " + std::to_string(count)).c_str());
	return true;
}

static void on_init_command_queue( reshade::api::command_queue *queue)
{
	;
}

void on_present(api::command_queue *queue, api::swapchain *swapchain, const api::rect *source_rect, const api::rect *dest_rect, uint32_t dirty_rect_count, const api::rect *dirty_rects)
{
	
	SBSRenderData &devData = g_device->get_private_data<SBSRenderData>();

	reshade::unregister_event<reshade::addon_event::present>(on_present);
	devData._dx11Graphics.ClearBuffer();
	reshade::register_event<reshade::addon_event::present>(on_present);
	count++;
	reshade::log::message(reshade::log::level::info, ("on_present countNumber: " + std::to_string(count)).c_str());
}

static void on_reshadepresent(api::effect_runtime *runtime)
{
	

}



extern "C" __declspec(dllexport) const char *NAME = "SBS output";
extern "C" __declspec(dllexport) const char *DESCRIPTION = "Duplicate SBS screen into double width buffer and output to glass.";



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

	
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		
		//MessageBoxA(0, "CreateThread Window Failed", "Fatal Error", MB_OK);
		if (!reshade::register_addon(hModule))
		{
			char message[256]; // 缓冲区存储格式化后的消息
			snprintf(message, sizeof(message), " CreateSwapChain Addon register Failed!");
			MessageBoxA(0, message, "Fatal Error", MB_OK);
			return FALSE;
		}
			
		
		g_hInstance = hModule;
			
		reshade::register_event<reshade::addon_event::init_device>(on_init);
		
		//reshade::register_event<reshade::addon_event::reshade_present>(on_reshadepresent);
		reshade::register_event<reshade::addon_event::init_swapchain>(on_init_swapchain);
		reshade::register_event<reshade::addon_event::create_swapchain>(on_create_swapchain);
		reshade::register_event<reshade::addon_event::present>(on_present);
		
	}
	break;
		
    case DLL_THREAD_ATTACH:
	/*	if (g_hInstance == nullptr)
		{
			g_hInstance = hModule;
			hThread = CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);
		}*/
		break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		/*g_bExit = TRUE;
		reshade::unregister_addon(hModule);*/
        break;
    }
    return TRUE;
}

