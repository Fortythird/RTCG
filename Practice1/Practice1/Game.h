#pragma once

#include "Export.h"
#include "TriangleComponent.h"

#include "DisplayWin32.h"
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


class Game
{
public:
	Game* instance;
	ID3D11InputLayout* layout;
	ID3D11Texture2D* backBuffer;
	ID3D11DeviceContext* context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	std::chrono::time_point<std::chrono::steady_clock> PrevTime;
	ID3D11RenderTargetView* rtv;
	std::vector<TriangleComponent*> gcVector;
	IDXGISwapChain* swapChain;
	ID3D11RasterizerState* rastState;
	float totalTime;
	float deltaTime;
	int screenWidth;
	int screenHeight;
	DisplayWin32 display;
	unsigned int frameCount;
	D3D11_SUBRESOURCE_DATA vertexData = {};
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* pixelBC = nullptr;
	HWND hWnd = nullptr;
	D3D11_VIEWPORT viewport;

	Game(int _width, int _height)
	{
		screenHeight = _height;
		screenWidth = _width;
		instance = this;
	}

	void PrepareResources();
	void DestroyResources();
	void PrepareFrame();
	void Draw();
	void Update();
	void Run();

private:

};

