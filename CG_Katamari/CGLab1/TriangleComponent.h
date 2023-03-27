#pragma once

#include "export.h"
#include "GameComponent.h"

struct TriangleComponentParameters 
{
	DirectX::XMFLOAT4* points;

	int* indeces;
	int numPoints;
	int numIndeces;

	DirectX::SimpleMath::Vector3 compPosition;
};

struct ConstData
{
	DirectX::SimpleMath::Matrix worldViewProj;
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix invertedWorldTransform;
};

class TriangleComponent : public GameComponent 
{
private:
	
	float radius;
	
	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC;
	ID3DBlob* pixelBC;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	UINT strides[4];
	UINT offsets[4];
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	bool isGot = false;
	DirectX::SimpleMath::Matrix parentMtrx;

public:
	DirectX::SimpleMath::Matrix GetModelMatrix();
	TriangleComponent* parent;

	ConstData constData;
	ID3D11Buffer* constBuffer;

	DirectX::SimpleMath::Quaternion rotate = DirectX::SimpleMath::Quaternion::Identity;
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 offset;
	DirectX::SimpleMath::Vector4* normals;

	ID3D11RasterizerState* rastState;
	ID3D11InputLayout* layout;

	DirectX::SimpleMath::Vector3 compPosition;

	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);

	int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin32 display, HRESULT result);
	void DestroyResources();
	void Draw(ID3D11DeviceContext* context);
	void Update(ID3D11DeviceContext* context, Camera* camera) override;
};