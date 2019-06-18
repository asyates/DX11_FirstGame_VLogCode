#pragma once
#include "Content/Camera.h"
#include "Models/Model.h"
#include "Content/ShaderStructures.h"
#include "Common/DirectXHelper.h"
#include "Models/Cube.h"
#include "Models/GridFloor.h"
#include "Models/MeshGeometry.h"

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
	void Update(std::array<bool, 4> wasd_keys, std::array<bool, 4> direction_keys, std::array<bool, 2> gh_keys);
	void Render();

	//Functions Called From App Class
	void FireArrow();
	void PlayerJump(std::array<bool, 4> wasd_keys);
	void StopPlayerJump();

private:
	ComPtr<ID3D11Device> dev;                      // the device interface
	ComPtr<ID3D11DeviceContext1> devcon;            // the device context interface
	ComPtr<IDXGISwapChain1> swapchain;              // the swap chain interface
	ComPtr<ID3D11RenderTargetView> rendertarget;    // the render target interface
	ComPtr<ID3D11Buffer> vertexbuffer;              // the vertex buffer interface
	ComPtr<ID3D11Buffer> grid_vbuffer;				// vertex buffer interface for grid
	ComPtr<ID3D11Buffer> indexbuffer;				// the index buffer interface
	ComPtr<ID3D11Buffer> m_cbufferPerObj;			// constant buffer to be updated for each object render
	ComPtr<ID3D11Buffer> m_cbufferPerFrame;			// constant buffer to be updated for each frame.
	ComPtr<ID3D11DepthStencilView> zbuffer;         // depth buffer interface
	ComPtr<ID3D11VertexShader> vertexshader;        // the vertex shader interface
	ComPtr<ID3D11PixelShader> pixelshader;          // the pixel shader interface
	ComPtr<ID3D11InputLayout> inputlayout;          // the input layout interface
	ComPtr<ID3D11ShaderResourceView> texture1;		// avatar texture

private:
	float Time;
	float scaleFact; //for scaling model (demo purposes, possibly delete!)
	float modelScale;
	float lookAngle; //angle camera is looking
	float pMoveSpeed = 0.1f;
	float pi = 3.14f;
	float initJumpVelocity = 0.15f; //initial jump velocity
	float currFallVelocity = 0.15f; //current jump velocity, will be adjusted while jumping
	float gravity = 0.005f;			//gravity acting to reduce jump velocity
	bool playerJumping = false;
	float jumpAngle;

	Camera Cam;
	Cube modCubes[2];
	MeshGeometry filemesh[2];
	MeshGeometry arrow; //define arrow

	GridFloor gFloor;

	//define object materials
	MATERIAL mGridMat;
	
	//Constant buffer structures
	CBUFFERPERFRAME cbPerFrame;
	CBUFFERPEROBJECT cbPerObject;


private:
	void DrawCubes(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matShadow, XMMATRIX shadowOffset);
	void DrawGrid(XMMATRIX matView, XMMATRIX matProjection);

	void UpdateGameCamera(std::array<bool, 4> wasd_keys, std::array<bool, 4> direction_keys);
	bool CheckObjPointCollision(XMVECTOR point);

	std::array<bool, 2> ws_jump = { false, false }; //check if keys w or s (forward/backward) are pressed down when jump occurred
	bool stationary_jump = false; //will become true if player jumps while neither w or s keys are pressed.
	bool arrowFired = false; //false if arrow not fired
	float arrowDirection; //hold direction arrow is fired. 
};