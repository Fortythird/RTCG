#pragma once

#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include "DisplayWin32.h"
#include <vector>
#include "TriangleComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


class Game
{
public:
	ID3D11InputLayout* layout;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* backBuffer;
	ID3D11RenderTargetView* rtv;
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();;
	float totalTime;


	void PrepareResources();
	void PrepareFrame();
	void Draw(float color[4]);
	void Update();
	void Run();

private:
	int screenWidth;
	int screenHeight;
	DisplayWin32 display = DisplayWin32(800, 800);
	unsigned int frameCount;
	D3D11_SUBRESOURCE_DATA vertexData = {};
	ID3D11Buffer* vb;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* pixelBC = nullptr;
	HWND hWnd = nullptr;
	std::vector<TriangleComponent*> gcVector;
};

