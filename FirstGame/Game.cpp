#pragma once

#include "pch.h"
#include "Game.h"
#include <fstream>
#include <WICTextureLoader.h>


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
	lookAngle = pi/2;
	Cam.UpdateCameraLookAtXZ(lookAngle);

}

// this function performs updates to the state of the game
void CGame::Update(std::array<bool, 4> wasd_keys, std::array<bool, 4> direction_keys, std::array<bool, 2> gh_keys) {

	//Update camera position based on wasd keypress
	UpdateGameCamera(wasd_keys, direction_keys);
	
	
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
    float color[4] = {0.2f, 0.6f, 1.0f, 1.0f};
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

	// Setup constant buffer content to be updated per frame
	cbPerFrame.DiffuseVector = XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f); //light direction
	cbPerFrame.DiffuseColor = XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f); //light color
	cbPerFrame.SpecPower = XMVectorSet(0.0f, 0.0f, 0.0f, 16.0f); // Specular light power
	cbPerFrame.SpecColor = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // Specular light colour
	cbPerFrame.AmbientColor = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // ambient light color
	cbPerFrame.EyePos = Cam.GetCameraPosition();

	// load the data into the constant buffer
	devcon->UpdateSubresource(m_cbufferPerFrame.Get(), 0, 0, &cbPerFrame, 0, 0);

	//Set up shadow matrix
	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //define in xz plane
	XMMATRIX S = XMMatrixShadow(shadowPlane, cbPerFrame.DiffuseVector);

	//render cubes
	DrawCubes(matView, matProjection);

	//Draw map grid
	DrawGrid(matView, matProjection);

	// switch the back buffer and the front buffer
	swapchain->Present(1, 0);
}

//Function for drawing cubes, to be called during game rendering.
void CGame::DrawCubes(XMMATRIX matView, XMMATRIX matProjection) {
		
	//Configure world matrix of first cube
	mod_cubes[0].SetPosition(4.0f, 2.0f, -3.0f);
	mod_cubes[0].SetRotation(0.0f, 0.1f, 0.0f);
	mod_cubes[0].SetScale(0.2f, 2.0f, 0.2f);
	
	//Configure work matrix of second cube
	mod_cubes[1].SetPosition(0.25f, 2.0f, 2.5f);
	mod_cubes[1].SetRotation(0.0f, Time, 0.0f);
	
	// tell the GPU which texture to use
	devcon->PSSetShaderResources(0, 1, texture1.GetAddressOf());

	//Run for loop to produce the final matrix for all cubes in mod_cubes array before drawing them.
	for (int i = 0; i < ARRAYSIZE(mod_cubes); i++) {

		//calculate final matrix
		XMMATRIX matWorld = mod_cubes[i].GetWorldMatrix();
		cbPerObject.matFinal = matWorld * matView * matProjection;
		cbPerObject.matWorld = matWorld;
		cbPerObject.matRotate = mod_cubes[i].GetRotationMatrix();
		
		//draw cube
		mod_cubes[i].Draw(devcon, m_cbufferPerObj, cbPerObject);
	}
}

//draw grid floor
void CGame::DrawGrid(XMMATRIX matView, XMMATRIX matProjection) {
	
	gFloor.SetScale(6.0f, 0.0f, 6.0f);
	
	//calculate final matrix and pass to cbuffer (along with rotation matrix)
	
	XMMATRIX matWorld = gFloor.GetWorldMatrix();
	cbPerObject.matFinal = matWorld * matView * matProjection;
	cbPerObject.matWorld = matWorld;
	cbPerObject.matRotate = gFloor.GetRotationMatrix();

	gFloor.Draw(devcon, m_cbufferPerObj, cbPerObject);
}

// this function loads and initializes all graphics data
void CGame::InitGraphics()
{	
	//Initialise cubes
	mod_cubes[0].SetTextureFile(L"Images/fence.png"); //set new texture file (so it's not default). Currently needs to be set before calling Initialize().
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
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0 , 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// create the input layout
	dev->CreateInputLayout(ied, ARRAYSIZE(ied), VSFile->Data, VSFile->Length, &inputlayout);
	devcon->IASetInputLayout(inputlayout.Get());

	//create matrix constant buffer
	D3D11_BUFFER_DESC mbd = { 0 };

	mbd.Usage = D3D11_USAGE_DEFAULT;
	mbd.ByteWidth = sizeof(cbPerObject);
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	dev->CreateBuffer(&mbd, nullptr, &m_cbufferPerObj);

	devcon->VSSetConstantBuffers(0, 1, m_cbufferPerObj.GetAddressOf());
	
	//create light constant buffer
	D3D11_BUFFER_DESC lbd = { 0 };

	lbd.Usage = D3D11_USAGE_DEFAULT;
	lbd.ByteWidth = sizeof(cbPerFrame);
	lbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	dev->CreateBuffer(&lbd, nullptr, &m_cbufferPerFrame);

	devcon->VSSetConstantBuffers(1, 1, m_cbufferPerFrame.GetAddressOf());

}

void CGame::UpdateGameCamera(std::array<bool, 4> wasd_keys, std::array<bool, 4> direction_keys) {

	if (wasd_keys[0] == true) {
		//if W pressed, move cam position in angle direction
		Cam.UpdateCameraPosition(lookAngle, false);
	};

	if (wasd_keys[1] == true) {
		//if A pressed, increase angle (goes counter-clockwise)
		lookAngle += 0.05f;
		Cam.UpdateCameraLookAtXZ(lookAngle);
	};

	if (wasd_keys[2] == true) {
		//if S pressed, move cam position in the reversed angle direction (2nd argument = true)
		Cam.UpdateCameraPosition(lookAngle, true);
	};

	if (wasd_keys[3] == true) {
		//if D pressed, decrease angle
		lookAngle -= 0.05f;
		Cam.UpdateCameraLookAtXZ(lookAngle);
	};

	if (direction_keys[1] == true) {
		//if Up pressed, increase y coordinate of look direction
		Cam.UpdateCameraLookAtY(true);
	};

	if (direction_keys[3] == true) {
		//if Up pressed, increase y coordinate of look direction
		Cam.UpdateCameraLookAtY(false);
	};

}