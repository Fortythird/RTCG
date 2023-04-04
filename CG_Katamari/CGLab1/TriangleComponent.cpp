#include "export.h"
#include "TriangleComponent.h"
#include "Camera.h"

TriangleComponent::TriangleComponent()
{
	parameters.points = nullptr;
	parameters.indeces = nullptr;
	parameters.numPoints = 0;
	parameters.numIndeces = 0;

	compPosition = DirectX::SimpleMath::Vector3(0, 0, 0);

	pos = { 0.0f, 0.0f, 0.0f };
	offset = { 0.0f, 0.0f, 0.0f };
	radius = 1.0f;

	parent = nullptr;
	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;
	textureBuffer = nullptr;
	samplerState = nullptr;
	rastState = nullptr;
	normals = nullptr;

	for (int i = 0; i < 4; i++) 
	{
		strides[i] = 0;
	}

	for (int i = 0; i < 4; i++) 
	{
		offsets[i] = 0;
	}
}

TriangleComponent::TriangleComponent(TriangleComponentParameters param, const wchar_t* _texturePath)
{
	parameters.points = param.points;
	parameters.indeces = param.indeces;
	parameters.numPoints = param.numPoints;
	parameters.numIndeces = param.numIndeces;

	compPosition = param.compPosition;

	pos = { 0.0f, 0.0f, 0.0f };
	offset = { 0.0f, 0.0f, 0.0f };
	radius = 1.0f;

	texturePath = _texturePath;

	parent = nullptr;
	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;
	textureBuffer = nullptr;
	samplerState = nullptr;
	rastState = nullptr;

	topologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	normals = new DirectX::SimpleMath::Vector4[parameters.numPoints / 2];

	for (int i = 0; i < 4; i++) 
	{
		strides[i] = 32;
	}

	for (int i = 0; i < 4; i++) 
	{
		offsets[i] = 0;
	}
}

int TriangleComponent::Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin32 display, HRESULT result) 
{
	ID3DBlob* errorVertexCode = nullptr;
	result = D3DCompileFromFile(
		L"../Shaders/ThirdExampleShader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode
	);

	if (FAILED(result)) {
		if (errorVertexCode)
		{
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			MessageBox(display.getHWND(), L"../Shaders/ThirdExampleShader.hlsl", L"Missing Shader File", MB_OK);
		}

		__debugbreak();

		return 0;
	}

	ConstData constData;

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(constData);
	result = device->CreateBuffer(&constBufDesc, nullptr, &constBuffer);

	if (FAILED(result)) {
		std::cout << "Error while const buffer creating...";
	}

	D3D_SHADER_MACRO Shader_Macros[] = {
		"TEST",
		"1",
		"TCOLOR",
		"float4(0.0f, 1.0f, 0.0f, 1.0f)",
		nullptr,
		nullptr
	};

	ID3DBlob* errorPixelCode;
	result = D3DCompileFromFile(
		L"../Shaders/ThirdExampleShader.hlsl",
		Shader_Macros /*macros*/,
		nullptr /*include*/,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelBC,
		&errorPixelCode
	);

	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr,
		&vertexShader
	);

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr,
		&pixelShader
	);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	   D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	   D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	device->CreateInputLayout(
		inputElements,
		UINT(std::size(inputElements)),
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout
	);

	int size = parameters.numPoints;

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = sizeof(TriangleComponentParameters::Vertex);
	vertexBufDesc.ByteWidth = sizeof(TriangleComponentParameters::Vertex) * (size);

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = parameters.points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * parameters.numIndeces;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = parameters.indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	strides[0] = sizeof(TriangleComponentParameters::Vertex);
	strides[1] = sizeof(TriangleComponentParameters::Vertex);
	strides[2] = sizeof(TriangleComponentParameters::Vertex);
	strides[3] = sizeof(TriangleComponentParameters::Vertex);
	offsets[0] = 0;
	offsets[1] = 0;
	offsets[2] = 0;
	offsets[3] = 0;

	// RASTERIZER state
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	result = device->CreateRasterizerState(&rastDesc, &rastState);
	result = CreateDDSTextureFromFile(device.Get(), texturePath, &textureBuffer, &textureView);

	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(&SamplerDesc, &samplerState);

	return 0;
}

void TriangleComponent::DestroyResources() 
{
	if (vertexShader != nullptr) 
	{
		vertexShader->Release();
	}

	if (pixelShader != nullptr)
	{
		pixelShader->Release();
	}

	if (layout != nullptr) 
	{
		layout->Release();
	}

	if (vertexBuffer != nullptr) 
	{
		vertexBuffer->Release();
	}

	if (indexBuffer != nullptr) 
	{
		indexBuffer->Release();
	}

	if (rastState != nullptr) 
	{
		rastState->Release();
	}

	if (constBuffer != nullptr) 
	{
		constBuffer->Release();
	}

}


void TriangleComponent::Update(ID3D11DeviceContext* context, Camera* camera) 
{
	constData.worldViewProj = GetModelMatrix() * camera->viewMatrix * camera->projectionMatrix;
	constData.worldViewProj = constData.worldViewProj.Transpose();

	D3D11_MAPPED_SUBRESOURCE subresourse = {};
	context->Map(
		constBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subresourse
	);

	memcpy(
		reinterpret_cast<float*>(subresourse.pData),
		&constData,
		sizeof(ConstData)
	);

	context->Unmap(constBuffer, 0);

	if (parent != nullptr) if ((parent->pos - pos).Length() <= (parent->radius + radius) / 2 && !isGot)
	{
		isGot = true;
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(parent->GetModelMatrix());
		pos = DirectX::XMVector3TransformCoord(pos, DirectX::XMMatrixInverse(&det, parent->GetModelMatrix()));
		parent->rotate.Inverse(rotate);
		parent->radius += 0.2f; 
		std::cout << "Got some stuff!" << std::endl;
	}
}

void TriangleComponent::Draw(ID3D11DeviceContext* context) 
{
	if (parameters.numIndeces != 0)
	{
		context->IASetInputLayout(layout);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->VSSetConstantBuffers(0, 1, &constBuffer);
		context->RSSetState(rastState);
		context->PSSetShaderResources(0, 1, &textureView);
		context->PSSetSamplers(0, 1, &samplerState);
		context->DrawIndexed(parameters.numIndeces, 0, 0);
	}
}

void TriangleComponent::SetPos(DirectX::SimpleMath::Vector3 _pos)
{
	pos = _pos;
}

DirectX::SimpleMath::Matrix TriangleComponent::GetModelMatrix()
{
	using namespace DirectX::SimpleMath;
	Matrix model = Matrix::Identity;
	model *= Matrix::CreateFromQuaternion(rotate);
	model *= Matrix::CreateTranslation(pos);
	

	if (parent != nullptr && isGot)
	{
		model *= parent->GetModelMatrix();
	}

	return model;
}
