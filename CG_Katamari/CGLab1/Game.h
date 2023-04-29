#pragma once

#include "export.h"
#include "DisplayWin32.h"
#include "GameComponent.h"
#include "TriangleComponent.h"
#include "Camera.h"

class Game 
{
public:
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	DisplayWin32 display;
	D3D11_VIEWPORT viewport;
	D3D11_VIEWPORT shadowViewport;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	TriangleComponent triangleComponent;
	ID3D11Debug* debug;
	ID3D11Texture2D* shadowDepthTexture;
	ID3D11Texture2D* sceneDepthTexture;
	ID3D11DepthStencilView* depthView;
	ID3D11DepthStencilView* shadowDepthView;
	ID3D11ShaderResourceView* resView;
	ID3D11DepthStencilState* depthState;
	ID3D11SamplerState* samplerState;
	ID3D11RasterizerState* drawRastState;
	ID3D11RasterizerState* shadowRastState;
	ID3D11Buffer* lightViewProjBuffer;

	std::chrono::time_point<std::chrono::steady_clock> prevTime;

	float deltaTime;
	float totalTime = 0;
	unsigned int frameCount = 0;

	void Init();
	int PrepareResources();
	void DestroyResources();
	void PrepareFrame();
	void Draw();
	void DrawShadows();

	float* BGcolor;

	std::vector <Camera*> camera;

public:
	InputDevice inputDevice;
	std::vector <GameComponent*> Components;
	ID3D11DeviceContext* context;

	bool IsKeyDown(Keys key) { inputDevice.IsKeyDown(key); };
	virtual void Update();

	Game();
	void Run();

	void SetBackgroundColor(float* color) { BGcolor = color; }
};