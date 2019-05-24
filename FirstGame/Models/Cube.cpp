#include "pch.h"
#include "Cube.h"
#include <WICTextureLoader.h>

Cube::Cube() {

}

void Cube::Initialize(ComPtr<ID3D11Device> dev) {
	
	//Call method from model class
	InitIndexedGraphics(dev, modelVertices, ARRAYSIZE(modelVertices), modelIndices, ARRAYSIZE(modelIndices));

	// load texture using DirectXTK function
	HRESULT hr = CreateWICTextureFromFile(dev.Get(), nullptr, L"Images/avatar.png", nullptr, &texture, 0);

}

void Cube::Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> constantbuffer, CBUFFER cbuffer) {

	//Call method from model class
	DrawIndexedGraphics(devcon, constantbuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, cbuffer, ARRAYSIZE(modelIndices));

}

VERTEX * Cube::GetModelVertices() {
	return modelVertices;
}

short * Cube::GetModelIndices() {
	return modelIndices;
}

UINT Cube::GetVertArraySize() {
	return ARRAYSIZE(modelVertices);
}

UINT Cube::GetIndArraySize() {
	return ARRAYSIZE(modelIndices);
}

