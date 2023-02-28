#include "TriangleComponent.h"

XMFLOAT4 points[] = {
	XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
	XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
	XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
};

TriangleComponent::TriangleComponent(Microsoft::WRL::ComPtr<ID3D11Device> _device, ID3D11RasterizerState* _rastState /*ID3D11InputLayout* _layout/*, float _offset*/)
{
	device = _device;
	rastState = _rastState;
	//layout = _layout;
	Initialize();
	//offset = _offset;
}

void TriangleComponent::Initialize()
{
	//ID3DBlob* errorVertexCode = nullptr;
	//auto res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
	//	nullptr /*macros*/,
	//	nullptr /*include*/,
	//	"VSMain",
	//	"vs_5_0",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
	//	0,
	//	&vertexBC,
	//	&errorVertexCode);

	//if (FAILED(res)) {
	//	// If the shader failed to compile it should have written something to the error message.
	//	if (errorVertexCode) {
	//		char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

	//		std::cout << compileErrors << std::endl;
	//	}
	//}

	//D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	//ID3DBlob* errorPixelCode;
	//res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", Shader_Macros /*macros*/, nullptr /*include*/, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

	//device->CreateVertexShader(
	//	vertexBC->GetBufferPointer(),
	//	vertexBC->GetBufferSize(),
	//	nullptr, &vertexShader);

	//device->CreatePixelShader(
	//	pixelBC->GetBufferPointer(),
	//	pixelBC->GetBufferSize(),
	//	nullptr, &pixelShader);

	//D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	//	D3D11_INPUT_ELEMENT_DESC {
	//		"POSITION",
	//		0,
	//		DXGI_FORMAT_R32G32B32A32_FLOAT,
	//		0,
	//		0,
	//		D3D11_INPUT_PER_VERTEX_DATA,
	//		0},
	//	D3D11_INPUT_ELEMENT_DESC {
	//		"COLOR",
	//		0,
	//		DXGI_FORMAT_R32G32B32A32_FLOAT,
	//		0,
	//		D3D11_APPEND_ALIGNED_ELEMENT,
	//		D3D11_INPUT_PER_VERTEX_DATA,
	//		0}
	//};

	//device->CreateInputLayout(
	//	inputElements,
	//	2,
	//	vertexBC->GetBufferPointer(),
	//	vertexBC->GetBufferSize(),
	//	&layout);


	//vertexBufDesc = {};
	//vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufDesc.CPUAccessFlags = 0;
	//vertexBufDesc.MiscFlags = 0;
	//vertexBufDesc.StructureByteStride = 0;
	//vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	//vertexData = {};
	//vertexData.pSysMem = points;
	//vertexData.SysMemPitch = 0;
	//vertexData.SysMemSlicePitch = 0;

	//device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

	//indexBufDesc = {};
	//indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufDesc.CPUAccessFlags = 0;
	//indexBufDesc.MiscFlags = 0;
	//indexBufDesc.StructureByteStride = 0;
	//indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

	//indexData = {};
	//indexData.pSysMem = indeces;
	//indexData.SysMemPitch = 0;
	//indexData.SysMemSlicePitch = 0;

	//device->CreateBuffer(&indexBufDesc, &indexData, &ib);

	//strides[1] = { 32 };
	//offsets[1] = { 0 };

	//rastDesc = {};
	//rastDesc.CullMode = D3D11_CULL_NONE;
	//rastDesc.FillMode = D3D11_FILL_SOLID;

	//res = device->CreateRasterizerState(&rastDesc, &rastState);

}

void TriangleComponent::Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* _rtv)
{
	rtv = _rtv;
	context->RSSetState(rastState);
	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	//context->OMSetRenderTargets(1, &_rtv, nullptr);
	//context->RSSetState(rastState);

	context->DrawIndexed(6, 0, 0);
}