#include "TriangleComponent.h"

void TriangleComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>* device, ID3D11DeviceContext* context)
{
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	(*device)->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	(*device)->CreateBuffer(&indexBufDesc, &indexData, &ib);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	auto res = (*device)->CreateRasterizerState(&rastDesc, &rastState);

	context->RSSetState(rastState);

		std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;


	//MSG msg = {};
	//bool isExitRequested = false;
	//while (!isExitRequested) {
	//	// Handle the windows messages.
	//	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}

	//	// If windows signals to end the application then exit out.
	//	if (msg.message == WM_QUIT) {
	//		isExitRequested = true;
	//	}


	//	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	//	game->context->ClearRenderTargetView(game->rtv, color);

	//	game->context->DrawIndexed(6, 0, 0);

	//	game->context->OMSetRenderTargets(0, nullptr, nullptr);

	//	game->swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	//}

}

void TriangleComponent::Draw(Microsoft::WRL::ComPtr<ID3D11Device>* device, ID3D11DeviceContext* context, ID3D11InputLayout* layout,
	ID3D11RenderTargetView* rtv, float totalTime, float color[4])
{

	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	auto res = (*device)->CreateRasterizerState(&rastDesc, &rastState);

	context->RSSetState(rastState);

	context->ClearState();

	context->RSSetState(rastState);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	context->OMSetRenderTargets(1, &rtv, nullptr);

	context->ClearRenderTargetView(rtv, color);

	context->DrawIndexed(6, 0, 0);
}