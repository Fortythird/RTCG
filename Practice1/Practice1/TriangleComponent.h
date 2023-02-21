#pragma once

#include "Export.h"
#include "DisplayWin32.h"
#include "GameComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
using namespace DirectX;


class TriangleComponent /* : public GameComponent*/
{
public:
	ID3D11InputLayout* layout;
	D3D11_BUFFER_DESC vertexBufDesc;
	D3D11_BUFFER_DESC indexBufDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	ID3D11RasterizerState* rastState;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	D3D11_SUBRESOURCE_DATA vertexData;
	int indeces[6] = { 0,1,2, 1,0,3 };
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	XMFLOAT4 points[8];
	UINT strides[1] = { 32 };
	UINT offsets[1] = { 0 };
	
	TriangleComponent(Microsoft::WRL::ComPtr<ID3D11Device> _device, ID3D11DeviceContext* _context, ID3D11InputLayout* _layout, ID3D11RasterizerState* _rastState) //: GameComponent(game)
	{
		rastState = _rastState;
		layout = _layout;
		//hWnd = _hWnd;
		Initialize(_device, _context);
	}

	void DestroyResources();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11DeviceContext* context, 
		ID3D11RenderTargetView* rtv, float totalTime);
	void Update();
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11DeviceContext* context);

private:
	HWND hWnd;
	
};

