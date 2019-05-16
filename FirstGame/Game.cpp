#pragma once

#include "pch.h"
#include "Game.h"
#include <fstream>


// this function loads a file into an Array^
Array<byte>^ LoadShaderFile(std::string File)
{
	Array<byte>^ FileData = nullptr;

	// open the file
	std::ifstream VertexFile(File, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
	if (VertexFile.is_open())
	{
		// find the length of the file
		int Length = (int)VertexFile.tellg();

		// collect the file data
		FileData = ref new Array<byte>(Length);
		VertexFile.seekg(0, std::ios::beg);
		VertexFile.read(reinterpret_cast<char*>(FileData->Data), Length);
		VertexFile.close();
	}

	return FileData;
}

// this function initializes and prepares Direct3D for use
void CGame::Initialize()
{
	// Define temporary pointers to a device and a device context
	ComPtr<ID3D11Device> dev11;
	ComPtr<ID3D11DeviceContext> devcon11;

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the device and device context objects
	D3D11CreateDevice(
		nullptr, //nullptr for letting direct3d figure out graphics adapter to use
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&dev11,
		nullptr,
		&devcon11);

	// Convert the pointers from the DirectX 11 versions to the DirectX 11.1 versions
	dev11.As(&dev);
	devcon11.As(&devcon);

	// First, convert our ID3D11Device1 into an IDXGIDevice1
	ComPtr<IDXGIDevice1> dxgiDevice;
	dev.As(&dxgiDevice);

	// Second, use the IDXGIDevice1 interface to get access to the adapter
	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	// Third, use the IDXGIAdapter interface to get access to the factory
	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	// set up the swap chain description
	DXGI_SWAP_CHAIN_DESC1 scd = { 0 };
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // how the swap chain should be used
	scd.BufferCount = 2;                                  // a front buffer and a back buffer
	scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // the most common swap chain format
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    // the recommended flip mode
	scd.SampleDesc.Count = 1;                             // disable anti-aliasing

	CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer

	dxgiFactory->CreateSwapChainForCoreWindow(
		dev.Get(),                                  // address of the device
		reinterpret_cast<IUnknown*>(Window),        // address of the window
		&scd,                                       // address of the swap chain description
		nullptr,                                    // advanced
		&swapchain);                                // address of the new swap chain pointer

	// get a pointer directly to the back buffer
	ComPtr<ID3D11Texture2D> backbuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

	// create a render target pointing to the back buffer
	dev->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);

	// create a zbuffer
	D3D11_TEXTURE2D_DESC texd;
	backbuffer->GetDesc(&texd); // THIS IS KEY! 
	
	texd.Usage = D3D11_USAGE_DEFAULT;
	texd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> zbuffertexture;

	HRESULT hr = dev->CreateTexture2D(&texd, nullptr, &zbuffertexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = dev->CreateDepthStencilView(zbuffertexture.Get(), &dsvd, &zbuffer);

	//get dpi settings to convert width and height of viewport to pixels
	ID2D1Factory* pFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);

	// get screen dpi (dots per inch)
	float dpiX, dpiY;
	pFactory->GetDesktopDpi(&dpiX, &dpiY);

	// set the viewport
	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (Window->Bounds.Width)*(dpiX/96.0f);
	viewport.Height = (Window->Bounds.Height)*(dpiY/96.0f);
	viewport.MinDepth = 0;    // the closest an object can be on the depth buffer is 0.0
	viewport.MaxDepth = 1;    // the farthest an object can be on the depth buffer is 1.0

	devcon->RSSetViewports(1, &viewport);

	// initialize graphics and the pipeline
	InitGraphics();
	InitPipeline();

	//Initialise time and model_scale variable
	Time = 0.0f;
	scaleFact = 1.0f;
	modelScale = 1.0f;

}

// this function performs updates to the state of the game
void CGame::Update(std::array<bool, 4> wasd_keys, std::array<bool, 2> gh_keys) {

	//Update camera position based on wasd keypress
	Cam.MoveCamera(wasd_keys);
	Time += 0.03f;

	//TO DELETE?? (DEMO CODE ONLY)
	if (gh_keys[0] == true) {
		scaleFact += 0.05f;
		mod_cubes[1].SetScale(scaleFact, scaleFact, scaleFact);
	}
	if (gh_keys[1] == true) {
		scaleFact -= 0.05f;
		mod_cubes[1].SetScale(scaleFact, scaleFact, scaleFact);
	}

}

// this function renders a single frame of 3D graphics
void CGame::Render() {

    // set our new render target object as the active render target
    devcon->OMSetRenderTargets(1, rendertarget.GetAddressOf(), zbuffer.Get());

    // clear the back buffer to a deep blue
    float color[4] = {0.1f, 0.3f, 0.6f, 1.0f};
    devcon->ClearRenderTargetView(rendertarget.Get(), color);

    // clear the depth buffer
    devcon->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// calculate the view transformation
	XMMATRIX matView = Cam.GetCameraView();

	// calculate the projection transformation
	CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45),                                      // the field of view
		(FLOAT)Window->Bounds.Width / (FLOAT)Window->Bounds.Height,  // aspect ratio
		1,                                                           // the near view-plane
		200);                                                        // the far view-plane

	// Setup constant buffer content
	CBUFFER cbuffer;
	cbuffer.DiffuseVector = XMVectorSet(5.0f, 1.5f, -2.0f, 0.0f);
	cbuffer.DiffuseColor = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
	cbuffer.AmbientColor = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);

	//render cubes
	DrawCubes(cbuffer, matView, matProjection);

	//Draw map grid
	DrawGrid(cbuffer,matView, matProjection);

	// switch the back buffer and the front buffer
	swapchain->Present(1, 0);
}

//Function for drawing cubes, to be called during game rendering.
void CGame::DrawCubes(CBUFFER cbuffer, XMMATRIX matView, XMMATRIX matProjection) {
		
	//Configure world matrix of first cube
	mod_cubes[0].SetPosition(4.0f, 1.0f, -3.0f);
	mod_cubes[0].SetRotation(0.0f, 0.1f, 0.0f);
	mod_cubes[0].SetScale(0.2f, 2.0f, 0.2f);
	
	//Configure work matrix of second cube
	mod_cubes[1].SetPosition(0.25f, 1.0f, 2.5f);
	mod_cubes[1].SetRotation(0.0f, Time, 0.0f);
	
	//Run for loop to produce the final matrix for all cubes in mod_cubes array before drawing them.

	for (int i = 0; i < ARRAYSIZE(mod_cubes); i++) {

		//calculate final matrix
		XMMATRIX matWorld = mod_cubes[i].GetWorldMatrix();
		cbuffer.Final = matWorld * matView * matProjection;
		cbuffer.Rotation = mod_cubes[i].GetRotationMatrix();
		
		//draw cube
		mod_cubes[i].Draw(devcon, constantbuffer, cbuffer);
	}
}

//draw grid floor
void CGame::DrawGrid(CBUFFER cbuffer, XMMATRIX matView, XMMATRIX matProjection) {
	
	gFloor.SetScale(6.0f, 0.0f, 6.0f);
	
	//calculate final matrix and pass to cbuffer (along with rotation matrix)
	XMMATRIX matWorld = gFloor.GetWorldMatrix();
	cbuffer.Final = matWorld * matView * matProjection;
	cbuffer.Rotation = gFloor.GetRotationMatrix();

	gFloor.Draw(devcon, constantbuffer, cbuffer);
}

// this function loads and initializes all graphics data
void CGame::InitGraphics()
{	
	//Initialise cubes
	mod_cubes[0].Initialize(dev);
	mod_cubes[1].Initialize(dev);

	//Initialise Grid
	gFloor.Initialize(dev);

}

// this function initializes the GPU settings and prepares it for rendering
void CGame::InitPipeline()
{
	// load the shader files
	Array<byte>^ VSFile = LoadShaderFile("VertexShader.cso");
	Array<byte>^ PSFile = LoadShaderFile("PixelShader.cso");

	// create the shader objects
	dev->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &vertexshader);
	dev->CreatePixelShader(PSFile->Data, PSFile->Length, nullptr, &pixelshader);

	// set the shader objects as the active shaders
	devcon->VSSetShader(vertexshader.Get(), nullptr, 0);
	devcon->PSSetShader(pixelshader.Get(), nullptr, 0);

	//initialise input layout
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// create the input layout
	dev->CreateInputLayout(ied, ARRAYSIZE(ied), VSFile->Data, VSFile->Length, &inputlayout);
	devcon->IASetInputLayout(inputlayout.Get());

	//create constant buffer
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBUFFER);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	dev->CreateBuffer(&bd, nullptr, &constantbuffer);
	devcon->VSSetConstantBuffers(0, 1, constantbuffer.GetAddressOf());
}

