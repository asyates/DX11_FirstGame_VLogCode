#include "pch.h"
#include "Cube.h"

Cube::Cube() {
	texfilename = L"Images/avatar.png";

	//define material properties
	mCubeMaterial.DiffuseColor = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); //light color
	mCubeMaterial.AmbientColor = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // ambient light color
	mCubeMaterial.SpecColor = XMVectorSet(1.0f, 1.0f, 1.0f, 4.0f); // Specular light colour

	mShadowMaterial.DiffuseColor = { 0.0f, 0.0f, 0.0f, 0.5f };
	mShadowMaterial.AmbientColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	mShadowMaterial.SpecColor = { 0.0f, 0.0f, 0.0f, 4.0f };

}

void Cube::Initialize(ComPtr<ID3D11Device> dev) {
	
	//Call method from model class
	InitIndexedGraphics(dev, modelVertices, ARRAYSIZE(modelVertices), modelIndices, ARRAYSIZE(modelIndices));

}

void Cube::DrawObject(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Set material to that of a shadow
	cbPerObject.gMaterial = mCubeMaterial;

	//Assign world matrix and rotation to back buffer to be used by vertex shader.
	cbPerObject.matWorld = GetWorldMatrix();
	cbPerObject.matRotate = GetRotationMatrix();
	
	//Call method from model class
	DrawIndexedGraphics(devcon, m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, cbPerObject, ARRAYSIZE(modelIndices));

}

void Cube::DrawShadow(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Set material to that of a shadow
	cbPerObject.gMaterial = mShadowMaterial;

	//Assign world matrix and rotation to back buffer to be used by vertex shader.
	cbPerObject.matWorld = GetWorldMatrix();
	cbPerObject.matRotate = GetRotationMatrix();

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

