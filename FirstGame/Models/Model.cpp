#include "pch.h"
#include "Model.h"
#include <WICTextureLoader.h>

Model::Model() {

	//setup for initial world matrix that has no translation, scaling, or rotation
	position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	scale = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//set matrices to be identity matrices (i.e. no translation, rotation, or scale)
	matTranslate = XMMatrixIdentity();
	matRotate = XMMatrixIdentity();
	matScale = XMMatrixIdentity();

	//set world matrix in same way (no need to multiply)
	matWorld = XMMatrixIdentity();

}

//Initialise model that has no indices
void Model::InitGraphics(ComPtr<ID3D11Device> dev, VERTEX vertices[], UINT vertArraySize) {
	
	// create the vertex buffer for cubes
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VERTEX) * vertArraySize; //Can use index 0 because vertices are the same for both. Might need to adjust later
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { vertices, 0, 0 };

	dev->CreateBuffer(&bd, &srd, &vertexbuffer);

	//Load texture using DirectXTK method
	HRESULT hr = CreateWICTextureFromFile(dev.Get(), nullptr, texfilename, nullptr, &texture, 0);

	//Initialise states
	InitStates(dev);
}

//Initialise model that has indices defined
void Model::InitIndexedGraphics(ComPtr<ID3D11Device> dev, VERTEX vertices[], UINT vertArraySize, short indices[], UINT indArraySize) {

	// create the vertex buffer for cubes
	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VERTEX) * vertArraySize; //Can use index 0 because vertices are the same for both. Might need to adjust later
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { vertices, 0, 0 };

	HRESULT hr = dev->CreateBuffer(&bd, &srd, &vertexbuffer);

	//create the index buffer for indices
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.ByteWidth = sizeof(short) * indArraySize;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA isrd = { indices, 0, 0 };

	hr = dev->CreateBuffer(&ibd, &isrd, &indexbuffer);

	//Load texture using DirectXTK method
	hr = CreateWICTextureFromFile(dev.Get(), nullptr, texfilename, nullptr, &texture, 0);

	//Initialise states
	InitStates(dev);
}

void Model::InitStates(ComPtr<ID3D11Device> dev)
{
	//Setup blending state

	D3D11_BLEND_DESC bd;
	bd.RenderTarget[0].BlendEnable = TRUE;    // turn on color blending
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;    // use addition in the blend equation
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;         // use source's alpha
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;    // use inverse source alpha
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd.IndependentBlendEnable = FALSE;    // only use RenderTarget[0]
	bd.AlphaToCoverageEnable = FALSE;    // enable alpha-to-coverage

	dev->CreateBlendState(&bd, &blendstate);

	//Setup depth stencil state to prevent double blending

	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;

	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	//backface setting, though not currently needed.
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	dev->CreateDepthStencilState(&noDoubleBlendDesc, &noDoubleBlendSS);

}

void Model::DrawGraphics(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY topology, CBUFFERPEROBJECT cbPerObject, UINT vertArraySize) {
	
	// set the blend state
	devcon->OMSetBlendState(blendstate.Get(), 0, 0xffffffff);
	devcon->OMSetDepthStencilState(nullptr, 0);

	//Set Vertex Buffers
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset); //set vertex buffer
	devcon->IASetPrimitiveTopology(topology);

	//update constant buffer with final matrix
	devcon->UpdateSubresource(m_cbufferPerObject.Get(), 0, 0, &cbPerObject, 0, 0);

	// tell the GPU which texture to use
	devcon->PSSetShaderResources(0, 1, texture.GetAddressOf());

	//draw grid floor
	devcon->Draw(vertArraySize, 0);
}

void Model::DrawIndexedGraphics(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY topology, CBUFFERPEROBJECT cbPerObject, UINT indArraySize) {
	
	// set the blend state
	devcon->OMSetBlendState(blendstate.Get(), 0, 0xffffffff);
	devcon->OMSetDepthStencilState(nullptr, 0);

	// set the vertex buffer and index buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);
	devcon->IASetIndexBuffer(indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	//// set the primitive topology
	devcon->IASetPrimitiveTopology(topology);

	//XMMATRIX matFinal = matWorld * matView * matProj; //calculate matrix to go from 3D to 2D

	// tell the GPU which texture to use
	devcon->PSSetShaderResources(0, 1, texture.GetAddressOf());

	// load the data into the constant buffer
	devcon->UpdateSubresource(m_cbufferPerObject.Get(), 0, 0, &cbPerObject, 0, 0);

	devcon->DrawIndexed(indArraySize, 0, 0);
}

void Model::DrawIndexedShadows(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY topology, CBUFFERPEROBJECT cbPerObject, UINT indArraySize) {

	devcon->OMSetBlendState(blendstate.Get(), 0, 0xffffffff);
	devcon->OMSetDepthStencilState(noDoubleBlendSS.Get(), 0);

	// set the vertex buffer and index buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);
	devcon->IASetIndexBuffer(indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	//// set the primitive topology
	devcon->IASetPrimitiveTopology(topology);

	// tell the GPU which texture to use
	devcon->PSSetShaderResources(0, 1, texture.GetAddressOf());

	// load the data into the constant buffer
	devcon->UpdateSubresource(m_cbufferPerObject.Get(), 0, 0, &cbPerObject, 0, 0);

	devcon->DrawIndexed(indArraySize, 0, 0);
}



void Model::SetTextureFile(const wchar_t* filename) {
	texfilename = filename;
}

void Model::SetPosition(float x, float y, float z) {

	//update position vector (unsure if needed at this stage, but keeping for now)
	position = XMVectorSet(x, y, z, 0.0f);

	//update Translation matrix
	matTranslate = XMMatrixTranslation(x, y, z);

	//update world matrix
	UpdateWorldMatrix();
}

void Model::SetRotation(float x, float y, float z) {

	//update rotation vector (unsure if needed at this stage, but keeping for now)
	rotation = XMVectorSet(x, y, z, 0.0f);

	//set each rotation matrix (indiv axis) to identity matrix

	XMMATRIX matRotateX = XMMatrixIdentity();
	XMMATRIX matRotateY = XMMatrixIdentity();
	XMMATRIX matRotateZ = XMMatrixIdentity();

	//check if change required before updating corresponding axis rotation
	if (x != 0) {
		matRotateX = XMMatrixRotationX(x);
	}
	if (y != 0) {
		matRotateY = XMMatrixRotationY(y);
	}
	if (z != 0) {
		matRotateZ = XMMatrixRotationZ(z);
	}
	matRotate = matRotateX * matRotateY * matRotateZ;

	//update world matrix
	UpdateWorldMatrix();
}

void Model::AdjustRotation(float x, float y, float z) {

	//set each rotation matrix (indiv axis) to identity matrix

	XMMATRIX matRotateX = XMMatrixIdentity();
	XMMATRIX matRotateY = XMMatrixIdentity();
	XMMATRIX matRotateZ = XMMatrixIdentity();

	//check if change required before updating corresponding axis rotation
	if (x != 0) {
		matRotateX = XMMatrixRotationX(x);
	}
	if (y != 0) {
		matRotateY = XMMatrixRotationY(y);
	}
	if (z != 0) {
		matRotateZ = XMMatrixRotationZ(z);
	}
	
	//Don't reset matRotate for adjusting rotation
	matRotate = matRotate * matRotateX * matRotateY * matRotateZ;

	//update world matrix
	UpdateWorldMatrix();
}

void Model::SetScale(float x, float y, float z) {

	//update scale vector (unsure if needed at this stage, but keeping for now)
	scale = XMVectorSet(x, y, z, 0.0f);

	//update Translation matrix
	matScale = XMMatrixScaling(x, y, z);

	//update world matrix
	UpdateWorldMatrix();
}

void Model::UpdateWorldMatrix() {
	matWorld = matScale * matRotate * matTranslate;
}

XMMATRIX Model::GetWorldMatrix() {
	return matWorld;
}

XMMATRIX Model::GetRotationMatrix() {
	return matRotate;
}

XMVECTOR Model::GetPosition() {
	return position;
}

XMVECTOR Model::GetRotation() {
	return rotation;
}