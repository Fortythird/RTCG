#include "export.h"
#include "TriangleComponent.h"

TriangleComponent::TriangleComponent() 
{
	parameters.points = nullptr;
	parameters.indeces = nullptr;
	parameters.numPoints = 0;
	parameters.numIndeces = 0;

	compPosition = DirectX::SimpleMath::Vector3(0, 0, 0);

	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;

	blend = nullptr;
	rastState = nullptr;
	sampler = nullptr;

	for (int i = 0; i < 4; i++) {
		strides[i] = 0;
	}
	
	for (int i = 0; i < 4; i++) {
		offsets[i] = 0;
	}
}

TriangleComponent::TriangleComponent(TriangleComponentParameters param) 
{
	parameters.points = param.points;
	parameters.indeces = param.indeces;
	parameters.numPoints = param.numPoints;
	parameters.numIndeces = param.numIndeces;

	compPosition = param.compPosition;

	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;

	blend = nullptr;
	rastState = nullptr;
	sampler = nullptr;

	for (int i = 0; i < 4; i++)
	{
		strides[i] = 0;
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
		L"../Shaders/SecondExampleShader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode
	);

	if (FAILED(result)) 
	{
		if (errorVertexCode)
		{
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			MessageBox(display.getHWND(), L"../Shaders/SecondExampleShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}

	constData = { 0,0 };

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DYNAMIC; // Тип заполняемости буфера
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(ConstData);
	device->CreateBuffer(&constBufDesc, nullptr, &constBuffer);

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
		L"../Shaders/SecondExampleShader.hlsl",
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
		   "COLOR",
		   0,
		   DXGI_FORMAT_R32G32B32A32_FLOAT,
		   0,
		   D3D11_APPEND_ALIGNED_ELEMENT,
		   D3D11_INPUT_PER_VERTEX_DATA,
		   0
		}
	};

	device->CreateInputLayout(
		inputElements,
		2,
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout
	);

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * parameters.numPoints;

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

	strides[0] = 32;
	strides[1] = 32;
	strides[2] = 32;
	strides[3] = 32;
	offsets[0] = 0;
	offsets[1] = 0;
	offsets[2] = 0;
	offsets[3] = 0;

	// RASTERIZER state
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	result = device->CreateRasterizerState(&rastDesc, &rastState);

	return 0;
}

void TriangleComponent::DestroyResources() {
	if (vertexShader != nullptr) {
		vertexShader->Release();
	}

	if (pixelShader != nullptr) {
		pixelShader->Release();
	}

	if (layout != nullptr) {
		layout->Release();
	}

	if (vertexBuffer != nullptr) {
		vertexBuffer->Release();
	}

	if (indexBuffer != nullptr) {
		indexBuffer->Release();
	}

	if (rastState != nullptr) {
		rastState->Release();
	}

	if (sampler != nullptr) {
		sampler->Release();
	}

	if (constBuffer != nullptr) {
		constBuffer->Release();
	}

	if (blend != nullptr) {
		blend->Release();
	}
}

void TriangleComponent::Update(ID3D11DeviceContext* context) {
	D3D11_MAPPED_SUBRESOURCE res = {};
	context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	auto dataP = static_cast<float*>(res.pData);
	memcpy(dataP, &constData, sizeof(ConstData));

	context->Unmap(constBuffer, 0);

}

void TriangleComponent::Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor) {
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

		context->DrawIndexed(
			parameters.numIndeces,
			0,
			0
		);
	}
}