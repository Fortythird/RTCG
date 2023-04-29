#include "export.h"
#include "Game.h"
#include "TriangleComponent.h"

Game::Game() 
{
	context = nullptr;
	swapChain = nullptr;
	rtv = nullptr;
	debug = nullptr;
	BGcolor = new float[4] { 0.0f, 0.0f, 0.0f, 0.0f };

	shadowDepthTexture = nullptr;
	sceneDepthTexture = nullptr;
	depthView = nullptr;
	shadowDepthView = nullptr;
	resView = nullptr;
	shadowRastState = nullptr;
}

void Game::Init() 
{
	inputDevice.Init(display.getHWND());

	display.CreateDisplay(&inputDevice);

	PrepareResources();
}

void Game::Run() 
{
	Init();

	bool isExitRequested = false;
	MSG msg = {};

	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT) {
				isExitRequested = true;
			}
		}

		PrepareFrame();

		Update();
		DrawShadows();
		Draw();
		
		swapChain->Present(1, 0);
	}

	DestroyResources();
}

int Game::PrepareResources() 
{
	D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_1};

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = display.getScreenWidth();
	swapDesc.BufferDesc.Height = display.getScreenHeight();
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = display.getHWND();
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	viewport = {};
	viewport.Width = static_cast<float>(display.getScreenWidth());
	viewport.Height = static_cast<float>(display.getScreenHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	shadowViewport = {};
	shadowViewport.Width = 5000;
	shadowViewport.Height = 5000;
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;
	shadowViewport.MinDepth = 0;
	shadowViewport.MaxDepth = 1.0f;

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context
	);

	if (FAILED(res))
	{
		std::cout << "Error while create device and swap chain" << std::endl;
	}

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.ArraySize = 1; 
	depthTexDesc.MipLevels = 1;
	//depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.Width = 5000;
	depthTexDesc.Height = 5000;
	depthTexDesc.SampleDesc = { 1, 0 };
	res = device->CreateTexture2D(&depthTexDesc, nullptr, &shadowDepthTexture);
	depthTexDesc.Width = 800;
	depthTexDesc.Height = 800;
	res = device->CreateTexture2D(&depthTexDesc, nullptr, &sceneDepthTexture);

	if (FAILED(res))
	{
		std::cout << "Error while creating texture 2D" << std::endl;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStenDesc = {};
	depthStenDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStenDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStenDesc.Texture2D.MipSlice = 0;
	//depthStenDesc.Flags = 0;
	res = device->CreateDepthStencilView(sceneDepthTexture, &depthStenDesc, &depthView);

	if (FAILED(res))
	{
		std::cout << "Error while creating scene depth stencil view" << std::endl;
	}

	res = device->CreateDepthStencilView(shadowDepthTexture, &depthStenDesc, &shadowDepthView);

	if (FAILED(res))
	{
		std::cout << "Error while creating shadow depth stencil view" << std::endl;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResViewDesc = {};
	shaderResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResViewDesc.Texture2D.MipLevels = 1;
	res = device->CreateShaderResourceView(shadowDepthTexture, &shaderResViewDesc, &resView);

	if (FAILED(res))
	{
		std::cout << "Error while creating shader resource view" << std::endl;
	}

	/*D3D11_SAMPLER_DESC comparisonSamplerDesc = {};
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	res = device->CreateSamplerState(&comparisonSamplerDesc, &samplerState);

	D3D11_RASTERIZER_DESC drawRenderStateDesc = {};
	drawRenderStateDesc.CullMode = D3D11_CULL_BACK;
	drawRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	drawRenderStateDesc.DepthClipEnable = true;
	res = device->CreateRasterizerState(&drawRenderStateDesc, &drawRastState);
	
	D3D11_RASTERIZER_DESC shadowRenderStateDesc = {};
	shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;
	res = device->CreateRasterizerState(&shadowRenderStateDesc, &shadowRastState);

	/*D3D11_DEPTH_STENCIL_DESC depthStateDesc = {};
	depthStateDesc.DepthEnable = true;
	depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStateDesc.StencilEnable = false;

	res = device->CreateDepthStencilState(&depthStateDesc, &depthState);
	context->OMSetDepthStencilState(depthState, 0);*/

	ID3D11Texture2D* backTexture;
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTexture);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTexture, nullptr, &rtv);

	for (int i = 0; i < Components.size(); i++)
	{
		Components[i]->Init(device, display, res);
	}

	return 0;
}

void Game::DestroyResources() 
{
	for (int i = 0; i < Components.size(); i++) 
	{
		Components[i]->DestroyResourses();
	}

	if (context != nullptr) 
	{
		context->ClearState();
		context->Release();
	}

	if (swapChain != nullptr) 
	{
		swapChain->Release();
	}

	if (device != nullptr) 
	{
		device->Release();
	}

	if (debug != nullptr) 
	{
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}

	if (shadowDepthTexture) 
	{
		shadowDepthTexture->Release();
	}

	if (sceneDepthTexture)
	{
		sceneDepthTexture->Release();
	}

	if (depthView != nullptr) 
	{
		depthView->Release();

	}
	
	if (shadowDepthView != nullptr) 
	{
		shadowDepthView->Release();

	}
}

void Game::PrepareFrame() 
{
	auto curTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
	prevTime = curTime;
	totalTime += deltaTime;
	frameCount++;

	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;
		totalTime = 0.0f;
		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(display.getHWND(), text);
		frameCount = 0;
	}

	context->ClearState();
	context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->ClearDepthStencilView(shadowDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->ClearRenderTargetView(rtv, BGcolor);
}

void Game::Update() 
{
	camera.at(0)->Update(
		deltaTime, 
		display.getScreenWidth(), 
		display.getScreenHeight()
	);

	for (int i = 0; i < Components.size(); i++) 
	{
		Components[i]->Update(context, camera.at(0));
	}
}

void Game::Draw() 
{
	camera.at(0)->Update(
		deltaTime,
		display.getScreenWidth(),
		display.getScreenHeight()
	);

	context->ClearRenderTargetView(rtv, BGcolor);
	
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &rtv, depthView);

	for (int i = 0; i < Components.size(); i++)
	{
		Components[i]->Draw(context, camera.at(0), resView);
	}

	ID3D11RenderTargetView* nullrtv[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	context->OMSetRenderTargets(1, nullrtv, nullptr);
}

void Game::DrawShadows()
{
	context->RSSetViewports(1, &shadowViewport);
	ID3D11RenderTargetView* nullrtv[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	context->OMSetRenderTargets(1, nullrtv, shadowDepthView);

	for (int i = 0; i < Components.size(); i++)
	{
		Components[i]->DrawShadow(context, device);
	}	
	
	context->ClearState();
}