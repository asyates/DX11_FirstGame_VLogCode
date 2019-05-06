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

	// set the viewport
	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Window->Bounds.Width;
	viewport.Height = Window->Bounds.Height;
	viewport.MinDepth = 0;    // the closest an object can be on the depth buffer is 0.0
	viewport.MaxDepth = 1;    // the farthest an object can be on the depth buffer is 1.0

	devcon->RSSetViewports(1, &viewport);

	// initialize graphics and the pipeline
	InitGraphics();
	InitPipeline();

	//Initialise time variable
	Time = 0.0f;

}

// this function performs updates to the state of the game
void CGame::Update(std::array<bool, 4> wasd) {

	//Update camera position based on wasd keypress
	Cam.MoveCamera(wasd);

	Time += 0.03f;
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

    // set the vertex buffer and index buffer
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);
    devcon->IASetIndexBuffer(indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    // set the primitive topology
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// calculate the view transformation
	XMMATRIX matView = Cam.GetCameraView();

	// calculate the projection transformation
	CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45),                                      // the field of view
		(FLOAT)Window->Bounds.Width / (FLOAT)Window->Bounds.Height,  // aspect ratio
		1,                                                           // the near view-plane
		200);                                                        // the far view-plane

	//Apply transformations to triangles

	//First Triangle
	mod_cubes[0].SetPosition(0.0f, 0.0f, 0.0f);
	mod_cubes[0].SetRotation(0.0f, 0.1f, 0.0f);
	mod_cubes[0].SetScale(0.2, 2.0, 0.2);
	//Second Triangle
	mod_cubes[1].SetPosition(0.25f, 0.0f, 2.5f);
	mod_cubes[1].SetRotation(0.0f, Time, 0.0f);

	//Draw each triangle in matFinal
	XMMATRIX matFinal[ARRAYSIZE(mod_cubes)];
	for (int i = 0; i < ARRAYSIZE(mod_cubes); i++) {

		//calculate final matrix
		XMMATRIX test = mod_cubes[i].GetWorldMatrix();
		matFinal[i] = mod_cubes[i].GetWorldMatrix() * matView * matProjection;

		// load the data into the constant buffer
		devcon->UpdateSubresource(constantbuffer.Get(), 0, 0, &matFinal[i], 0, 0);
		// draw 3 vertices, starting from vertex 0
		devcon->DrawIndexed(mod_cubes[i].GetIndArraySize(), 0, 0);
	}

	// switch the back buffer and the front buffer
	swapchain->Present(1, 0);
}

// this function loads and initializes all graphics data
void CGame::InitGraphics()
{	

	// create the vertex buffer for cubes
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VERTEX) * mod_cubes[0].GetVertArraySize(); //Can use index 0 because vertices are the same for both. Might need to adjust later
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = {mod_cubes[0].GetModelVertices(), 0, 0 };

	dev->CreateBuffer(&bd, &srd, &vertexbuffer);

	//create the index buffer for indices
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = sizeof(short) * mod_cubes[0].GetIndArraySize();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA isrd = { mod_cubes[0].GetModelIndices(), 0, 0 };

	HRESULT hr = dev->CreateBuffer(&ibd, &isrd, &indexbuffer);
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
		{"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// create the input layout
	dev->CreateInputLayout(ied, ARRAYSIZE(ied), VSFile->Data, VSFile->Length, &inputlayout);
	devcon->IASetInputLayout(inputlayout.Get());

	//create constant buffer
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 64;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	dev->CreateBuffer(&bd, nullptr, &constantbuffer);
	devcon->VSSetConstantBuffers(0, 1, constantbuffer.GetAddressOf());
}