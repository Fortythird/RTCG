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
	lightConstBuffer = nullptr;
	textureBuffer = nullptr;
	samplerState = nullptr;
	depthSamplerState = nullptr;
	rastState = nullptr;
	lightBuffer = nullptr;
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
	lightConstBuffer = nullptr;
	textureBuffer = nullptr;
	samplerState = nullptr;
	depthSamplerState = nullptr;
	lightBuffer = nullptr;
	rastState = nullptr;

	topologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

		//__debugbreak();

		return 0;
	}

	//ConstData constData;
	//LightData lightData;

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

	D3D11_BUFFER_DESC lightBufDesc = {};
	lightBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufDesc.MiscFlags = 0;
	lightBufDesc.StructureByteStride = 0;
	lightBufDesc.ByteWidth = sizeof(LightData);
	result = device->CreateBuffer(&lightBufDesc, nullptr, &lightBuffer);

	if (FAILED(result)) {
		std::cout << "Error while light buffer creating...";
	}

	D3D11_BUFFER_DESC lightConstBufDesc = {};
	lightConstBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightConstBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightConstBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightConstBufDesc.MiscFlags = 0;
	lightConstBufDesc.StructureByteStride = 0;
	lightConstBufDesc.ByteWidth = sizeof(LightConstData);
	result = device->CreateBuffer(&lightConstBufDesc, nullptr, &lightConstBuffer);
	
	if (FAILED(result)) {
		std::cout << "Error while light const buffer creating...";
	}

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
		},
		D3D11_INPUT_ELEMENT_DESC {
			"NORMAL",
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

	if (FAILED(result)) {
		std::cout << "\nError while creating rasterizer state..\n";
	}

	result = CreateDDSTextureFromFile(device.Get(), texturePath, &textureBuffer, &textureView);

	if (FAILED(result)) {
		std::cout << "\nError while creating DDS texture..\n";
	}

	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(&SamplerDesc, &samplerState);

	D3D11_SAMPLER_DESC DepthSamplerDesc = {};
	DepthSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	DepthSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	DepthSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&DepthSamplerDesc, &depthSamplerState);

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

	if (textureBuffer != nullptr)
	{
		textureBuffer->Release();
	}

	if (constBuffer != nullptr) 
	{
		constBuffer->Release();
	}

	if (lightConstBuffer != nullptr)
	{
		lightConstBuffer->Release();
	}

	if (lightBuffer != nullptr)
	{
		lightBuffer->Release();
	}

}


void TriangleComponent::Update(ID3D11DeviceContext* context, Camera* camera) 
{
	/*//constData.worldViewProj = GetModelMatrix() * camera->viewMatrix * camera->projectionMatrix;
	//constData.worldViewProj = constData.worldViewProj.Transpose();

	constData.world = DirectX::SimpleMath::Matrix::CreateTranslation(parameters.compPosition);
	constData.world = GetModelMatrix().Transpose();
	constData.invertedWorldTransform = GetModelMatrix().Transpose().Invert().Transpose();

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

	context->Unmap(constBuffer, 0);*/

	lightData.direction = DirectX::SimpleMath::Vector4(2.0f, 2.0f, 1.0f, 1.0f);
	lightData.color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	lightData.viewerPosition = DirectX::SimpleMath::Vector4(camera->position.x, camera->position.y, camera->position.z, 1.0f);

	auto dir = DirectX::SimpleMath::Vector3(lightData.direction.x, lightData.direction.y, lightData.direction.z);
	dir.Normalize();
	float destination = 60.0f;
	lightData.worldViewProj = /*GetModelMatrix() */ DirectX::SimpleMath::Matrix::CreateLookAt(dir * destination, dir * destination - dir, DirectX::SimpleMath::Vector3(0, 0, 1))
		* DirectX::SimpleMath::Matrix::CreateOrthographic(60, 60, 0.1f, 150.0f);
	lightData.worldViewProj = lightData.worldViewProj.Transpose();

	D3D11_MAPPED_SUBRESOURCE subresourse2 = {};
	context->Map(
		lightBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subresourse2
	);

	memcpy(
		reinterpret_cast<float*>(subresourse2.pData),
		&lightData,
		sizeof(LightData)
	);
	context->Unmap(lightBuffer, 0);

	if (parent != nullptr) if ((parent->pos - pos).Length() <= radius * 1.5f && !isGot)
	{
		isGot = true;
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(parent->GetModelMatrix());
		pos = DirectX::XMVector3TransformCoord(pos, DirectX::XMMatrixInverse(&det, parent->GetModelMatrix()));
		parent->rotate.Inverse(rotate);
		parent->radius += 0.2f;
		//parent->pos += DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.2f);
		std::cout << "Got some stuff!" << std::endl;
	}
}

void TriangleComponent::Draw(ID3D11DeviceContext* context, Camera* camera, ID3D11ShaderResourceView* resView)
{
	auto dir = DirectX::SimpleMath::Vector3(lightData.direction.x, lightData.direction.y, lightData.direction.z);
	dir.Normalize();
	float destination = 60.0f;
	lightConstData.pos = DirectX::SimpleMath::Vector4(dir.x * destination, dir.y * destination, dir.z * destination, 1.0f);
	lightConstData.view = DirectX::SimpleMath::Matrix::CreateLookAt(dir * destination, dir * destination - dir, DirectX::SimpleMath::Vector3(0, 0, 1));
	lightConstData.projView = lightConstData.view * DirectX::SimpleMath::Matrix::CreateOrthographic(60.0f, 60.0f, 0.1f, 150.0f);

	D3D11_MAPPED_SUBRESOURCE subresourse = {};
	context->Map(
		lightConstBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subresourse
	);

	memcpy(
		reinterpret_cast<float*>(subresourse.pData),
		&lightConstData,
		sizeof(LightConstData)
	);
	context->Unmap(lightConstBuffer, 0);

	constData.worldViewProj = GetModelMatrix() * camera->viewMatrix * camera->projectionMatrix;
	constData.worldViewProj = constData.worldViewProj.Transpose();

	constData.world = DirectX::SimpleMath::Matrix::CreateTranslation(parameters.compPosition);
	constData.world = GetModelMatrix().Transpose();
	constData.invertedWorldTransform = GetModelMatrix().Transpose().Invert().Transpose();

	D3D11_MAPPED_SUBRESOURCE subresourse2 = {};
	context->Map(
		constBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subresourse2
	);

	memcpy(
		reinterpret_cast<float*>(subresourse2.pData),
		&constData,
		sizeof(ConstData)
	);

	context->Unmap(constBuffer, 0);

	if (parameters.numIndeces != 0)
	{
		context->IASetInputLayout(layout);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->VSSetConstantBuffers(0, 1, &constBuffer);
		context->PSSetConstantBuffers(1, 1, &lightBuffer);
		context->PSSetConstantBuffers(2, 1, &lightConstBuffer);
		context->PSSetShaderResources(0, 1, &textureView);
		context->PSSetShaderResources(1, 1, &resView);
		context->PSSetSamplers(0, 1, &samplerState);
		context->PSSetSamplers(1, 1, &depthSamplerState);
		context->RSSetState(rastState);
		context->DrawIndexed(parameters.numIndeces, 0, 0);
	}
}

void TriangleComponent::DrawShadow(ID3D11DeviceContext* context, Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	auto dir = DirectX::SimpleMath::Vector3(lightData.direction.x, lightData.direction.y, lightData.direction.z);
	dir.Normalize();
	float destination = 60.0f;
	constData.worldViewProj = GetModelMatrix() * DirectX::SimpleMath::Matrix::CreateLookAt(dir * destination, dir * destination - dir, DirectX::SimpleMath::Vector3(0, 0, 1))
		* DirectX::SimpleMath::Matrix::CreateOrthographic(60, 60, 0.1f, 150.0f);
	constData.worldViewProj = constData.worldViewProj.Transpose();
	constData.world = DirectX::SimpleMath::Matrix::CreateTranslation(parameters.compPosition);
	constData.world = GetModelMatrix().Transpose();
	constData.invertedWorldTransform = GetModelMatrix().Transpose().Invert().Transpose();

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

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	context->VSSetShader(vertexShader, nullptr, 0);
	//context->PSSetShader(pixelShader, nullptr, 0);
	//context->PSSetShader(nullptr, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &constBuffer);
	//context->PSSetConstantBuffers(1, 1, &lightBuffer);
	//context->PSSetConstantBuffers(1, 0, nullptr);
	//context->PSSetShaderResources(0, 1, &textureView);
	//context->PSSetShaderResources(1, 1, nullptr);
	//context->PSSetSamplers(0, 1, &samplerState);
	//context->PSSetSamplers(1, 1, nullptr);
	//context->RSSetState(rastState);
	context->DrawIndexed(parameters.numIndeces, 0, 0);
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

void TriangleComponent::NormalsCalc() {
	int ind_a, ind_b, ind_c;

	DirectX::SimpleMath::Vector4 a, b, c, p, q, norm;
	DirectX::XMFLOAT4* position = new DirectX::XMFLOAT4[parameters.numPoints / 2];

	int temp = 0;
	for (int i = 0; i < parameters.numPoints / 2; i++) {
		position[i] = parameters.points[temp].position;
		temp += 2;
	}

	for (int i = 0; i < parameters.numIndeces; i += 3) {
		ind_a = parameters.indeces[i];
		ind_b = parameters.indeces[i + 1];
		ind_c = parameters.indeces[i + 2];

		a = position[ind_a];
		b = position[ind_b];
		c = position[ind_c];

		p = a - b;
		q = c - b;

		norm.x = p.y * q.z - p.z * q.y;
		norm.y = p.z * q.x - p.x * q.z;
		norm.z = p.x * q.y - p.y * q.x;
		norm.w = 1.0f;

		normals[ind_a] = norm;
		normals[ind_b] = norm;
		normals[ind_c] = norm;
	};
}