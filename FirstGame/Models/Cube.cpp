#include "pch.h"
#include "Cube.h"

Cube::Cube() {
	texfilename = L"Images/avatar.png";
}

void Cube::Initialize(ComPtr<ID3D11Device> dev) {
	
	//Call method from model class
	InitIndexedGraphics(dev, modelVertices, ARRAYSIZE(modelVertices), modelIndices, ARRAYSIZE(modelIndices));

}

void Cube::Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Call method from model class
	DrawIndexedGraphics(devcon, m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, cbPerObject, ARRAYSIZE(modelIndices));

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

