#include "pch.h"
#include "Cube.h"

Cube::Cube() {

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

void Cube::SetPosition(float x, float y, float z) {
	
	//update position vector (unsure if needed at this stage, but keeping for now)
	position = XMVectorSet(x, y, z, 0.0f);
	
	//update Translation matrix
	matTranslate = XMMatrixTranslation(x, y, z);

	//update world matrix
	UpdateWorldMatrix();
}

void Cube::SetRotation(float x, float y, float z) {
	
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

void Cube::SetScale(float x, float y, float z) {
	
	//update scale vector (unsure if needed at this stage, but keeping for now)
	scale = XMVectorSet(x, y, z, 0.0f);

	//update Translation matrix
	matScale = XMMatrixScaling(x, y, z);

	//update world matrix
	UpdateWorldMatrix();
}

void Cube::UpdateWorldMatrix() {
	matWorld = matScale * matRotate * matTranslate;
}

XMMATRIX Cube::GetWorldMatrix() {
	return matWorld;
}