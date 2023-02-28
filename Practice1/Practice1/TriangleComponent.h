#pragma once

#include "Export.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
using namespace DirectX;

class TriangleComponent /*: public GameComponent*/
{
public:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	ID3D11InputLayout* layout;
	D3D11_BUFFER_DESC vertexBufDesc;
	D3D11_BUFFER_DESC indexBufDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	ID3D11RasterizerState* rastState;
	CD3D11_RASTERIZER_DESC rastDesc;
	ID3D11RenderTargetView* rtv;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	D3D11_SUBRESOURCE_DATA vertexData;
	int indeces[6] = { 0,1,2, 1,0,3 };
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	float offset;
	UINT strides[1];
	UINT offsets[1];
	
	//TriangleComponent(Game* _gameInstance)
	//{
	//	gameInstance = _gameInstance;
	//	Initialize();
	//}
	TriangleComponent(Microsoft::WRL::ComPtr<ID3D11Device> _device, ID3D11RasterizerState* _rastState /*ID3D11InputLayout* _layout/*, float _offset*/);
	void DestroyResources();
	void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv);
	void Update();
	void Initialize();	
};

