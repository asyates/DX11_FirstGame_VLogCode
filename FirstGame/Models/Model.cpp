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

}

void Model::DrawGraphics(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY topology, CBUFFERPEROBJECT cbPerObject, UINT vertArraySize) {
	
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
