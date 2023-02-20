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

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


class TriangleComponent
{
public:

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	DirectX::XMFLOAT4 points[8] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	ID3D11RasterizerState* rastState;
	
	TriangleComponent(DisplayWin32* _dW32, HWND _hWnd, Microsoft::WRL::ComPtr<ID3D11Device>* device, ID3D11DeviceContext* context)
	{
		screenHeight =  _dW32->screenHeight;
		screenWidth = _dW32->screenWidth;
		hWnd = _hWnd;
		Initialize(device, context);
	}

	void DestroyResources();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Device>* device, ID3D11DeviceContext* context, ID3D11InputLayout* layout,
		ID3D11RenderTargetView* rtv, float totalTime, float color[4]);
	void Update();

private:
	int screenWidth;
	int screenHeight;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	D3D11_SUBRESOURCE_DATA vertexData = {};
	int indeces[6] = { 0,1,2, 1,0,3 };
	D3D11_SUBRESOURCE_DATA indexData = {};
	HWND hWnd;

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>* device, ID3D11DeviceContext* context);
	
};

