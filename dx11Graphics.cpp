#include "dx11Graphics.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

dx11Graphics::dx11Graphics()
	:CreateSwapChainSuccess(false),CanPresent(false), swapchainEvent(SwapchainEvent::init)
{
	reshade::log::message(reshade::log::level::info,"dx11Graphics Construct");
}

dx11Graphics::~dx11Graphics()
{
	reshade::log::message(reshade::log::level::info, "dx11Graphics DeConstruct");
}

void dx11Graphics::TryCreateSwapChainforWnd(api::device *device,  HWND _hwnd)
{
	
	HRESULT hResult;
	
	ID3D11Device *baseDevice = reinterpret_cast<ID3D11Device *>(device->get_native());

	// 假设你已经有了设备和窗口句柄
	hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&pDevice);
	assert(SUCCEEDED(hResult));

	/*Microsoft::WRL::ComPtr<ID3D11DeviceContext>baseContext;
	baseDevice->GetImmediateContext(&baseContext);
	hResult = baseContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void **)&pContext);
	assert(SUCCEEDED(hResult));
	baseContext->Release();*/

	baseDevice->GetImmediateContext(&pContext);
	pDevice->GetImmediateContext(&pContext);
	assert(pContext);

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

	////创建renderTarget
	//bool createTargetState  = win32CreateD3D11RenderTargets(pDevice.Get(),pSwapChain.Get());
	//assert(createTargetState);

	////创建ID3D11VertexShader ID3D11PixelShader ID3D11InputLayout ID3D11SamplerState ID3D11ShaderResourceView
	//{
	//	ID3DBlob *vsBlob;
	//	{
	//		ID3DBlob *shaderCompileErrorsBlob;
	//		HRESULT hResult = D3DCompileFromFile(L"SBSshader.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vsBlob, &shaderCompileErrorsBlob);
	//		if (FAILED(hResult))
	//		{
	//			const char *errorString = NULL;
	//			if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	//				errorString = "Could not compile shader; file not found";
	//			else if (shaderCompileErrorsBlob) {
	//				errorString = (const char *)shaderCompileErrorsBlob->GetBufferPointer();
	//				shaderCompileErrorsBlob->Release();
	//			}
	//			MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
	//		}

	//		hResult = pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVertexShader);
	//		assert(SUCCEEDED(hResult));
	//	}


	//	// Create Pixel Shader
	//	{
	//		ID3DBlob *psBlob;
	//		ID3DBlob *shaderCompileErrorsBlob;
	//		HRESULT hResult = D3DCompileFromFile(L"SBSshader.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &shaderCompileErrorsBlob);
	//		if (FAILED(hResult))
	//		{
	//			const char *errorString = NULL;
	//			if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	//				errorString = "Could not compile shader; file not found";
	//			else if (shaderCompileErrorsBlob) {
	//				errorString = (const char *)shaderCompileErrorsBlob->GetBufferPointer();
	//				shaderCompileErrorsBlob->Release();
	//			}
	//			MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
	//		}

	//		hResult = pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPixelShader);
	//		assert(SUCCEEDED(hResult));
	//		psBlob->Release();
	//	}


	//	// Create Input Layout
	//	{
	//		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	//		{
	//			{ "POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//		};

	//		HRESULT hResult = pDevice->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pInputLayout);
	//		assert(SUCCEEDED(hResult));
	//		vsBlob->Release();
	//	}

	//}

	////创建vertexBuffer Sampler  ShaderResourceView
	//{
	//	float vertexData[] = { // x, y, u, v
	//	   -0.5f,  0.5f, 0.f, 0.f,
	//	   0.5f, -0.5f, 1.f, 1.f,
	//	   -0.5f, -0.5f, 0.f, 1.f,
	//	   -0.5f,  0.5f, 0.f, 0.f,
	//	   0.5f,  0.5f, 1.f, 0.f,
	//	   0.5f, -0.5f, 1.f, 1.f
	//	};
	//	stride = 4 * sizeof(float);
	//	numVerts = sizeof(vertexData) / stride;
	//	offset = 0;

	//	D3D11_BUFFER_DESC vertexBufferDesc = {};
	//	vertexBufferDesc.ByteWidth = sizeof(vertexData);
	//	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//	D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };

	//	HRESULT hResult = pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &pVertexBuffer);
	//	assert(SUCCEEDED(hResult));


	//	// Create Sampler State
	//	D3D11_SAMPLER_DESC samplerDesc = {};
	//	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//	samplerDesc.BorderColor[0] = 1.0f;
	//	samplerDesc.BorderColor[1] = 1.0f;
	//	samplerDesc.BorderColor[2] = 1.0f;
	//	samplerDesc.BorderColor[3] = 1.0f;
	//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//	pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	//}

	////测试加载图片
	//{
	//	// Load Image
	//	int texWidth, texHeight, texNumChannels;
	//	int texForceNumChannels = 4;
	//	unsigned char *testTextureBytes = stbi_load("SBSTexture.png", &texWidth, &texHeight,
	//												&texNumChannels, texForceNumChannels);
	//	assert(testTextureBytes);
	//	int texBytesPerRow = 4 * texWidth;

	//	// Create Texture
	//	D3D11_TEXTURE2D_DESC textureDesc = {};
	//	textureDesc.Width = texWidth;
	//	textureDesc.Height = texHeight;
	//	textureDesc.MipLevels = 1;
	//	textureDesc.ArraySize = 1;
	//	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//	textureDesc.SampleDesc.Count = 1;
	//	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	//	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	//	D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
	//	textureSubresourceData.pSysMem = testTextureBytes;
	//	textureSubresourceData.SysMemPitch = texBytesPerRow;

	//	
	//	pDevice->CreateTexture2D(&textureDesc, &textureSubresourceData, &pTexture);

	//	pDevice->CreateShaderResourceView(pTexture.Get(), nullptr, &pSRV);

	//	free(testTextureBytes);
	//}




ID3D11Texture2D *pBackBuffer;
pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pTarget);
pBackBuffer->Release();



	CanPresent = true;
}

void dx11Graphics::TryPresent(HWND _hwnd)
{
	if (CanPresent)
	{
		FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };

		pContext->ClearRenderTargetView(pTarget.Get(), backgroundColor);


		RECT winRect;
		GetClientRect(_hwnd, &winRect);
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f };
		pContext->RSSetViewports(1, &viewport);

		pContext->OMSetRenderTargets(1, &pTarget, nullptr);

		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext->IASetInputLayout(pInputLayout.Get());

		pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

		pContext->PSSetShaderResources(0, 1, &pSRV);
		pContext->PSSetSamplers(0, 1, &pSamplerState);

		pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		pContext->Draw(numVerts, 0);

		pSwapChain->Present(1, 0);
	}
	
}

void dx11Graphics::CheckRenderTargetValid()
{
	if (pTarget)
	{
		
	}
	else
	{
		MessageBoxA(0, "errorString", "pTargetw Empty", MB_ICONERROR | MB_OK);
	}
}

void dx11Graphics::DrawTestTriangle()
{
	if (!pTarget)
	{
		MessageBoxA(0, "errorString", "pTargetw Empty", MB_ICONERROR | MB_OK);
	}
	ClearBuffer();
	HRESULT hr;

	//struct Vertex
	//{
	//	float x;
	//	float y;
	//	float r;
	//	float g;
	//	float b;
	//};

	//// create vertex buffer (1 2d triangle at center of screen)
	//const Vertex vertices[] =
	//{
	//	{ 0.0f,0.5f,1.0f,0.0f,0.0f },
	//	{ 0.5f,-0.5f,0.0f,1.0f,0.0f },
	//	{ -0.5f,-0.5f,0.0f,0.0f,1.0f },
	//};
	//Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	//D3D11_BUFFER_DESC bd = {};
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.CPUAccessFlags = 0u;
	//bd.MiscFlags = 0u;
	//bd.ByteWidth = sizeof(vertices);
	//bd.StructureByteStride = sizeof(Vertex);
	//D3D11_SUBRESOURCE_DATA sd = {};
	//sd.pSysMem = vertices;
	//pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);

	//// Bind vertex buffer to pipeline
	//const UINT stride = sizeof(Vertex);
	//const UINT offset = 0u;
	//pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	//// create pixel shader
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	//Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	//D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	//pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);

	//// bind pixel shader
	//pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


	//// create vertex shader
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	//D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	//pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);

	//// bind vertex shader
	//pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	//// input (vertex) layout (2d position only)
	//Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	//const D3D11_INPUT_ELEMENT_DESC ied[] =
	//{
	//	{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	//	{ "Color",0,DXGI_FORMAT_R32G32B32_FLOAT,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0 },
	//};
	//pDevice->CreateInputLayout(
	//	ied, (UINT)std::size(ied),
	//	pBlob->GetBufferPointer(),
	//	pBlob->GetBufferSize(),
	//	&pInputLayout
	//);

	//// bind vertex layout
	//pContext->IASetInputLayout(pInputLayout.Get());


	//// bind render target
	//pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);


	//// Set primitive topology to triangle list (groups of 3 vertices)
	//pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//// configure viewport
	//D3D11_VIEWPORT vp;
	//vp.Width = 1024;
	//vp.Height = 768;
	//vp.MinDepth = 0;
	//vp.MaxDepth = 1;
	//vp.TopLeftX = 0;
	//vp.TopLeftY = 0;
	//pContext->RSSetViewports(1u, &vp);


	//pContext->Draw((UINT)std::size(vertices), 0u);

	//Present时候会阻断现在reshade的Present
	pSwapChain->Present(1u, 0u);
}

void dx11Graphics::ClearBuffer()
{
	const float color[] = { 255.f,0.f,0.0f,1.0f };
	pContext->OMSetRenderTargets(1, &pTarget, NULL);
	pContext->ClearRenderTargetView(pTarget.Get(), color);

	pSwapChain->Present(1u, 0u);
}



bool dx11Graphics::GetCreateSwapChainState() const
{
	return CreateSwapChainSuccess;
}

void dx11Graphics::MoveToNextEvent()
{
	switch (swapchainEvent)
	{
	case init:
		swapchainEvent = SwapchainEvent::init_swapchain;
		break;
	case init_swapchain:
		swapchainEvent = SwapchainEvent::create_swapchain;
		break;
	case create_swapchain:
		swapchainEvent = SwapchainEvent::present;
		break;
	case present:
		swapchainEvent = SwapchainEvent::init;
		break;
	default:
		break;
	}
}

bool dx11Graphics::win32CreateD3D11RenderTargets(ID3D11Device1 *d3d11Device, IDXGISwapChain1 *swapChain)
{
	ID3D11Texture2D *d3d11FrameBuffer;
	HRESULT hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&d3d11FrameBuffer);
	assert(SUCCEEDED(hResult));

	hResult = d3d11Device->CreateRenderTargetView(d3d11FrameBuffer, 0, &pTarget);
	assert(SUCCEEDED(hResult));

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	d3d11FrameBuffer->GetDesc(&depthBufferDesc);

	d3d11FrameBuffer->Release();

	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D *depthBuffer;
	d3d11Device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);

	d3d11Device->CreateDepthStencilView(depthBuffer, nullptr, &pDSV);

	depthBuffer->Release();

	return true;
}
