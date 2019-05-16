#pragma once
#include "Content/Camera.h"
#include "Models/Model.h"
#include "Content/ShaderStructures.h"
#include "Common/DirectXHelper.h"
#include "Models/Cube.h"
#include "Models/GridFloor.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;
using namespace Platform;

class CGame
{
public:

	void Initialize();
	void InitGraphics();
	//void InitGrid();
	void InitPipeline();
	void Update(std::array<bool, 4> wasd_keys, std::array<bool, 2> gh_keys);
	void Render();

private:
	ComPtr<ID3D11Device> dev;                      // the device interface
	ComPtr<ID3D11DeviceContext1> devcon;            // the device context interface
	ComPtr<IDXGISwapChain1> swapchain;              // the swap chain interface
	ComPtr<ID3D11RenderTargetView> rendertarget;    // the render target interface
	ComPtr<ID3D11Buffer> vertexbuffer;              // the vertex buffer interface
	ComPtr<ID3D11Buffer> grid_vbuffer;				// vertex buffer interface for grid
	ComPtr<ID3D11Buffer> indexbuffer;				// the index buffer interface
	ComPtr<ID3D11Buffer> constantbuffer;			// constant buffer
	ComPtr<ID3D11DepthStencilView> zbuffer;         // depth buffer
	ComPtr<ID3D11VertexShader> vertexshader;        // the vertex shader interface
	ComPtr<ID3D11PixelShader> pixelshader;          // the pixel shader interface
	ComPtr<ID3D11InputLayout> inputlayout;          // the input layout interface

private:
	float Time;
	float scaleFact; //for scaling model (demo purposes, possibly delete!)
	float modelScale;
	float lookAngle; //angle camera is looking
	float pi = 3.14f;


	Camera Cam;
	Cube mod_cubes[2];
	GridFloor gFloor;
	
private:
	void DrawCubes(CBUFFER cbuffer, XMMATRIX matView, XMMATRIX matProjection);
	void DrawGrid(CBUFFER cbuffer, XMMATRIX matView, XMMATRIX matProjection);

	void UpdateGameCamera(std::array<bool, 4> wasd_keys); 


};