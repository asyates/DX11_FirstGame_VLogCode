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
	lookAngleXZ = pi / 2;
	Cam.UpdateCameraLookAtXZ(lookAngleXZ);

}

// this function performs updates to the state of the game
void CGame::Update(std::array<bool, 4> wasd_keys) {

	//Update camera position based on wasd keypress and jump
	UpdateGameCamera(wasd_keys);
	
	Time += 0.03f;

	if (arrowFired == true) {
		//move cam position in positive direction we are looking
		XMVECTOR arrowPosition = arrow.GetPosition();

		float new_x = XMVectorGetByIndex(arrowPosition, 0) + abs(arrowSpeedX) * cos(-arrowDirection);
		float new_y = XMVectorGetByIndex(arrowPosition, 1) + arrowSpeedY;
		float new_z = XMVectorGetByIndex(arrowPosition, 2) + abs(arrowSpeedX) * sin(-arrowDirection);

		arrowAngle = atan(arrowSpeedY / arrowSpeedX);

		arrow.SetPosition(new_x, new_y, new_z);

		arrow.SetRotation(0.0f, 0.0f, (pi / 2) - arrowAngle); //initial pi/2 brings it down onto x-z plane
		arrow.AdjustRotation(0.0f, pi+arrowDirection, 0.0f); //initial pi brings it in line with x axis (same as camera zero angle position)

		arrowSpeedY -= gravity;

		//check if below ground. If true, arrow is no longer rendered
		if (new_y <= 0.0f) {
			arrowFired = false;
		}

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
		0.05f,                                                           // the near view-plane
		200);                                                        // the far view-plane

	// Setup constant buffer content to be updated per frame
	cbPerFrame.DiffuseVector = XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f); //light direction
	cbPerFrame.EyePos = Cam.GetCameraPosition();

	// load the data into the constant buffer
	devcon->UpdateSubresource(m_cbufferPerFrame.Get(), 0, 0, &cbPerFrame, 0, 0);

	//Set up shadow matrix
	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //define in xz plane
	XMMATRIX S = XMMatrixShadow(shadowPlane, -cbPerFrame.DiffuseVector);
	XMMATRIX shadowOffsetY = XMMatrixTranslation(0.0f, 0.01f, 0.0f);
	
	//Draw map grid (before other objects to avoid blending with back buffer)
	DrawFloor(matView, matProjection); //Might need to change order of shadow render at later stage.

	// Draw arrow if fired
	if (arrowFired == true) {
		DrawArrow(matView, matProjection, S, shadowOffsetY);
	}

	//render cubes
	DrawCubes(matView, matProjection, S, shadowOffsetY);

	//render tortoise model
	DrawTortoise(matView, matProjection, S, shadowOffsetY);

	// switch the back buffer and the front buffer
	swapchain->Present(1, 0);
}

//Function for drawing cubes, to be called during game rendering.
void CGame::DrawCubes(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matShadow, XMMATRIX shadowOffsetY) {

	//Update rotation matrix with Time value (set in Update function)
	modCubes[1].SetRotation(0.0f, Time, 0.0f);

	//Run for loop to produce the final matrix for all cubes in mod_cubes array before drawing them.
	for (int i = 0; i < ARRAYSIZE(modCubes); i++) {

		//calculate final transformation matrix for object
		cbPerObject.matFinal = modCubes[i].GetWorldMatrix() * matView * matProjection;

		//draw cube
		modCubes[i].DrawObject(devcon, m_cbufferPerObj, cbPerObject);

		//Calculate transformation matrix to tranform vertex onto shadow plane
		XMMATRIX matWorldShadow = modCubes[i].GetWorldMatrix() *matShadow * shadowOffsetY;

		//Assign shadow transformation matrix to constant buffer
		cbPerObject.matFinal = matWorldShadow * matView * matProjection;

		//Draw Shadow
		modCubes[i].DrawShadow(devcon, m_cbufferPerObj, cbPerObject);

	}
}

void CGame::DrawArrow(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matShadow, XMMATRIX shadowOffsetY) {
	//calculate final transformation matrix for object
	cbPerObject.matFinal = arrow.GetWorldMatrix() * matView * matProjection;

	arrow.DrawObject(devcon, m_cbufferPerObj, cbPerObject); 	//draw tortoise

	//Calculate transformation matrix to tranform vertex onto shadow plane
	XMMATRIX matWorldShadow = arrow.GetWorldMatrix() *matShadow * shadowOffsetY;
	cbPerObject.matFinal = matWorldShadow * matView * matProjection; //Assign shadow transformation matrix to constant buffer

	arrow.DrawShadow(devcon, m_cbufferPerObj, cbPerObject); 	//Draw Shadow
}

void CGame::DrawTortoise(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matShadow, XMMATRIX shadowOffsetY) {

	//calculate final transformation matrix for object
	cbPerObject.matFinal = tortoise.GetWorldMatrix() * matView * matProjection;

	tortoise.DrawObject(devcon, m_cbufferPerObj, cbPerObject); 	//draw tortoise

	//Calculate transformation matrix to tranform vertex onto shadow plane
	XMMATRIX matWorldShadow = tortoise.GetWorldMatrix() *matShadow * shadowOffsetY;
	cbPerObject.matFinal = matWorldShadow * matView * matProjection; //Assign shadow transformation matrix to constant buffer

	tortoise.DrawShadow(devcon, m_cbufferPerObj, cbPerObject); 	//Draw Shadow

}

//draw grid floor
void CGame::DrawFloor(XMMATRIX matView, XMMATRIX matProjection) {
		
	//calculate final matrix and pass to cbuffer (Note: bug in visual studio currently presents performing this operation within gFloor object)
	cbPerObject.matFinal = gFloor.GetWorldMatrix() * matView * matProjection;
	gFloor.Draw(devcon, m_cbufferPerObj, cbPerObject);

}

void CGame::UpdateGameCamera(std::array<bool, 4> wasd_keys) {
	

	//if player is jumping or above Y = 1.0f
	if ((playerJumping == true) || (XMVectorGetByIndex(Cam.GetCameraPosition(), 1) > 1.0f)) {

		//adjust camera position to indicate jump.
		Cam.AdjustCameraPositionY(currFallVelocity);

		if (CheckObjPointCollision(Cam.GetCameraPosition())) {
			Cam.AdjustCameraPositionY(-currFallVelocity);
			StopPlayerJump();
			playerFalling = false;
		}
		else {
			playerFalling = true;
			currFallVelocity = currFallVelocity - gravity; //reduce jump velocity by a small amount each time (gravity), such that it becomes negative and player eventually falls.
		}

		if (XMVectorGetByIndex(Cam.GetCameraPosition(), 1) <= 1.0f) {
			StopPlayerJump();
		}
	} 

	//for each key direction, perform checks to see if player needs to be moved, depending on whether player is allowed to move in that direction (not mid-Jump). 
	//Also perform collision detection and update camera movements
	for (size_t i = 0; i < wasd_keys.size(); i++) {
		
		//check (1) if pressed while player is mid-jump, (2) if player is jumping in the given direction, (3) player has not jumped from stationary position
		if ((((wasd_keys[i] == true) && (playerJumping == false)) || (wasd_jump[i] == true))) {

			float angle; //angle player will be moving

			//if jumping in given direction, do not change direction based on lookAngleXZ (set to jumpAngle).
			if (wasd_jump[i] == true) {
				angle = jumpAngle; //if jumping assign angle to be the same as the angle at jump
			}
			else {
				angle = lookAngleXZ; //new direction
			}
			
			// [0] forward, [2] back
			if ((i == 0) || (i == 2)) {

				Cam.MoveCameraForwardBack((1-(int)i)*pMoveSpeedXZ, angle);  //move camera
				
				//check for collision
				if (CheckObjPointCollision(Cam.GetCameraPosition())) {
					Cam.MoveCameraForwardBack(((int)i-1)*pMoveSpeedXZ, angle); //move back if collision with object
				}		
			}
			// [1] left, [3] right
			else { 

				Cam.MoveCameraSideToSide(((int)i-2)*pMoveSpeedXZ, angle);  //move camera

				//check for collision
				if (CheckObjPointCollision(Cam.GetCameraPosition())) {
					Cam.MoveCameraSideToSide((2-(int)i)*pMoveSpeedXZ, angle); //move back if collision with object
				}
			}
			
			//Update camera look at vector following movement
			Cam.UpdateCameraLookAtXZ(lookAngleXZ);
			Cam.UpdateCameraLookAtY(lookAngleXZ, lookAngleY);
		}
	}

}

void CGame::AdjustLookAngleXZ(float x) {
	lookAngleXZ += x;
	Cam.UpdateCameraLookAtXZ(lookAngleXZ);
	Cam.UpdateCameraLookAtY(lookAngleXZ, lookAngleY);
}

void CGame::AdjustLookAngleY(float x) {

	float angleOffset = 0.8f; //max angle camera can reach is angleOffset from the vertical e.g. if set to pi/4, range is -135 and 135.
	lookAngleY += x;

	//don't go outside of looking straight up or straight down
	if (lookAngleY < (-pi / 2 + angleOffset)) { 
		lookAngleY = -pi / 2 + angleOffset;
	}
	else if (lookAngleY > (pi / 2 - angleOffset)) {
		lookAngleY = pi / 2 - angleOffset;
	}

	Cam.UpdateCameraLookAtY(lookAngleXZ, lookAngleY);
}

bool CGame::CheckObjPointCollision(XMVECTOR point) {

	//Set different leeway values for each object in mod_cubes (if keeping, will want to take this out of hardcoding)
	std::array<float, 2> modCubes_lw = { 0.2f , 0.5f };

	for (int i = 0; i < ARRAYSIZE(modCubes); i++) {
		if (modCubes[i].checkPointCollision(point, modCubes_lw[i]) == true) {
			return true;
		}
	}
	return false;
}

void CGame::FireArrow() {
	if (arrowFired == false) {
		arrowFired = true;

		//Update position of arrow to be where player is.
		XMFLOAT4 CamPosition;
		XMStoreFloat4(&CamPosition, Cam.GetCameraPosition());
		arrow.SetPosition(CamPosition.x,CamPosition.y, CamPosition.z);

		//Rotate arrow based on look angle (should be pointed in camera view direction)
		arrowDirection = -lookAngleXZ; //negative because rotate angle increases clockwise (and camera anti-clockwise)
		arrowAngle = lookAngleY;
		arrowSpeed = initArrowSpeed;
		arrowSpeedX = initArrowSpeed * cos(arrowAngle);
		arrowSpeedY = initArrowSpeed * sin(arrowAngle);
		initArrowSpeedY = arrowSpeedY;

	}
}

void CGame::PlayerJump(std::array<bool, 4> wasd_keys) {
	
	//if space has been pressed, and player not already jumping, set playing jumping to true.
	if ((playerJumping == false) && (playerFalling == false)) {
		playerJumping = true;
		playerFalling = true;
		jumpAngle = lookAngleXZ;
		currFallVelocity = initJumpVelocity;

		//if w or s keys pressed when player starts jump, set corresponding values in ws_key bool array to true.
		for (size_t i=0; i < wasd_keys.size(); i++) {
			if (wasd_keys[i] == true) {
				wasd_jump[i] = true;
			}
		}
	}
}

//call when player is no longer jumping (on ground level).
void CGame::StopPlayerJump() {
	playerJumping = false;
	playerFalling = false;
	currFallVelocity = 0; //reset fall velocity
	//reset jump directions (false if not jumping)
	for (size_t i = 0; i < wasd_jump.size(); i++) {
			wasd_jump[i] = false;
	}

}

// this function loads and initializes all graphics data
void CGame::InitGraphics()
{

	//Initialise Arrow
	arrow.SetTextureFile(L"Images/arrowTexture.png");
	arrow.Initialize(dev, "Models/arrow.obj");
	arrow.SetScale(0.3f, 0.3f, 0.3f);

	//Initialise cubes
	modCubes[0].SetTextureFile(L"Images/fence.png"); //set new texture file (so it's not default). Currently needs to be set before calling Initialize().
	modCubes[0].Initialize(dev);
	modCubes[0].SetPosition(3.8f, 2.0f, -2.8f); //set initial position
	modCubes[0].SetScale(0.2f, 2.0f, 0.2f); //set initial scale

	modCubes[1].Initialize(dev);
	modCubes[1].SetPosition(0.25f, 1.4f, 2.5f); //set initial position

	//Initialise Grid Floor
	gFloor.SetTextureFile(L"Images/ground1.png");
	gFloor.Initialize(dev);
	gFloor.SetScale(80.0f, 0.0f, 30.0f);

	//Initialise Tortoise

	tortoise.SetTextureFile(L"Images/tortoise.png");
	tortoise.Initialize(dev, "Models/tortoise.obj");
	tortoise.SetPosition(-10.0f, 2.5f, 0.0f);
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