#pragma once
#include "Content/Camera.h"
#include "Content/ShaderStructures.h"
#include "Common/DirectXHelper.h"

#include "Models/Triangle.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;
using namespace Platform;

class CGame
{
public:

	void Initialize();
	void InitGraphics();
	void InitPipeline();
	void Update(std::array<bool, 4> wasd);
	void Render();

private:
	ComPtr<ID3D11Device> dev;                      // the device interface
	ComPtr<ID3D11DeviceContext1> devcon;            // the device context interface
	ComPtr<IDXGISwapChain1> swapchain;              // the swap chain interface
	ComPtr<ID3D11RenderTargetView> rendertarget;    // the render target interface
	ComPtr<ID3D11Buffer> vertexbuffer;              // the vertex buffer interface
	ComPtr<ID3D11VertexShader> vertexshader;        // the vertex shader interface
	ComPtr<ID3D11PixelShader> pixelshader;          // the pixel shader interface
	ComPtr<ID3D11InputLayout> inputlayout;          // the input layout interface

	ComPtr<ID3D11Buffer> constantbuffer;			// constant buffer
	ComPtr<ID3D11DepthStencilView> zbuffer;         // depth buffer

	float Time;
	Camera Cam;
	Triangle mod_triangles[2];

};