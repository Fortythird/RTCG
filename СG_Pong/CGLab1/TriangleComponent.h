#pragma once

#include "export.h"
#include "GameComponent.h"

struct TriangleComponentParameters {
	DirectX::XMFLOAT4* points;

	int* indeces;
	int numPoints;
	int numIndeces;

	DirectX::SimpleMath::Vector3 compPosition;
};

struct ConstData
{
	float x;
	float y;
	float dummy0;
	float dummy1;
};

class TriangleComponent : public GameComponent {
private:
	DirectX::SimpleMath::Matrix GetModelMatrix();

	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	UINT strides[4];
	UINT offsets[4];
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

public:
	ID3D11BlendState* blend;
	ConstData constData;
	ID3D11SamplerState* sampler;
	ID3D11Buffer* constBuffer;

	ID3D11RasterizerState* rastState;
	ID3D11InputLayout* layout;

	DirectX::SimpleMath::Vector3 compPosition;

	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);

	int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin32 display, HRESULT result);
	void DestroyResources();
	void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor);
	void Update(ID3D11DeviceContext* context);
};