#include "pch.h"
#include "GridFloor.h"

GridFloor::GridFloor() {
	texfilename = L"Images/ground.png";

	//Set material properties
	mGridMaterial.DiffuseColor = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); //light color
	mGridMaterial.SpecColor = XMVectorSet(1.0f, 1.0f, 1.0f, 8.0f); // Specular light colour
	mGridMaterial.AmbientColor = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // ambient light color
}

void GridFloor::Initialize(ComPtr<ID3D11Device> dev) {

	//Call method from model class
	InitGraphics(dev, modelVertices, ARRAYSIZE(modelVertices));

}

void GridFloor::Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Assign material to constant buffer
	cbPerObject.gMaterial = mGridMaterial;

	//Assign world matrix and rotation matrix to constant buffer
	cbPerObject.matWorld = GetWorldMatrix();
	cbPerObject.matRotate = GetRotationMatrix();

	//Call method from model class
	DrawGraphics(devcon, m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, cbPerObject, ARRAYSIZE(modelVertices));
}

VERTEX * GridFloor::GetModelVertices() {
	return modelVertices;
}

UINT GridFloor::GetVertArraySize() {
	return ARRAYSIZE(modelVertices);
}
