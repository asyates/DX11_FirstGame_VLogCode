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

//check for collision with point. Allow leeway (float lw).
bool Cube::checkPointCollision(XMVECTOR pos, float lw) {

	XMVECTOR min = { -1.0f, -1.0f, -1.0f, 1.0f };
	XMVECTOR max = { 1.0f, 1.0f, 1.0f, 1.0f };

	//if scale has changed, multiple axis accordingly
	min = XMVector4Transform(min, matScale);
	max = XMVector4Transform(max, matScale);

	//if rotation has changed (can't apply like this for preserving min and max values)
    //min = XMVector4Transform(min, matRotate);
	//max = XMVector4Transform(max, matRotate);

	//if position changed
	min = XMVector4Transform(min, matTranslate);
	max = XMVector4Transform(max, matTranslate);

	XMFLOAT3 position;
	XMFLOAT3 minimum;
	XMFLOAT3 maximum;

	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&minimum, min);
	XMStoreFloat3(&maximum, max);

	// if x between x min and max AND y between y min and max and z etc.
	if ((checkRange(position.x, minimum.x, maximum.x, lw))
		&& (checkRange(position.y, minimum.y, maximum.y, lw))
			&& (checkRange(position.z, minimum.z, maximum.z, lw))
		) {
		return true;
	}
	else {
		return false;
	}

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


//helper function, to move somewhere later? (optional leeway argument gives leeway for collision detection)
bool Cube::checkRange(float x, float min, float max, float lw = 0.0f) {
	
	//if min and max are wrong way round (can happen during rotation), swap back.
	if (max < min) {
		float temp = max;
		max = min;
		min = temp;
	}
	
	return ((x > (min-lw)) && (x < (max+lw)));
}
