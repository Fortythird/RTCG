#pragma once

#include "export.h"
#include "DisplayWin32.h"

using namespace DirectX;

class GameComponent 
{
	public:
		virtual int Init(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin32 display, HRESULT res) { return 0; };
	
		virtual void DestroyResourses() {};

		virtual void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, float* BGcolor) {};

		virtual void Update(ID3D11DeviceContext* context) {};
};