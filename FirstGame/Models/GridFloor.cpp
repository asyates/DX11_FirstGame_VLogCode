#include "pch.h"
#include "GridFloor.h"

GridFloor::GridFloor() {
	texfilename = L"Images/ground.png";
}

void GridFloor::Initialize(ComPtr<ID3D11Device> dev) {

	//Call method from model class
	InitGraphics(dev, modelVertices, ARRAYSIZE(modelVertices));

}

void GridFloor::Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Call method from model class
	DrawGraphics(devcon, m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, cbPerObject, ARRAYSIZE(modelVertices));

}

VERTEX * GridFloor::GetModelVertices() {
	return modelVertices;
}

UINT GridFloor::GetVertArraySize() {
	return ARRAYSIZE(modelVertices);
}
