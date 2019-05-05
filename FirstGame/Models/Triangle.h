#pragma once
#include "pch.h"
#include "../Content/ShaderStructures.h"

using namespace DirectX;

class Triangle {

public:
	Triangle();

	//Get vertices
	VERTEX * GetModelVertices();

	//Methods for updating position, rotation, and scale of model
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	XMMATRIX GetWorldMatrix();
	UINT GetVertArraySize();


private:

	XMVECTOR position; //define x, y, and z position in world space
	XMVECTOR rotation; //define rotation in x,y, and z axis.
	XMVECTOR scale; //define scale in x, y, and z direction

	XMMATRIX matTranslate; //translation matrix
	XMMATRIX matRotate; // rotation matrix
	XMMATRIX matScale; // scaling matrix

	XMMATRIX matWorld; //world matrix

	void UpdateWorldMatrix();

	//define triangle vertices
	VERTEX modelVertices[3] = {
			{ 0.0f, 0.5f, 0.0f , 1.0f, 0.0f, 0.0f },
			{ 0.45f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f },
			{ -0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f }
	};

};