#include "pch.h"
#include "GridFloor.h"

void GridFloor::Initialize(ComPtr<ID3D11Device> dev) {

	//Call method from model class
	InitGraphics(dev, modelVertices, ARRAYSIZE(modelVertices));
}

void GridFloor::Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> constantbuffer, CBUFFER cbuffer) {

	//Call method from model class
	DrawGraphics(devcon, constantbuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, cbuffer, ARRAYSIZE(modelVertices));

}

VERTEX * GridFloor::GetModelVertices() {
	return modelVertices;
}

UINT GridFloor::GetVertArraySize() {
	return ARRAYSIZE(modelVertices);
}
