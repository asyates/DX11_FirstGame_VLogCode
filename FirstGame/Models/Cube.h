#pragma once
#include "pch.h"
#include "../Content/ShaderStructures.h"

using namespace DirectX;

class Cube {

public:
	Cube();

	//Get vertices
	VERTEX * GetModelVertices();
	UINT GetVertArraySize();
	//Get indices
	short * GetModelIndices();
	UINT GetIndArraySize();


	//Methods for updating position, rotation, and scale of model
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	XMMATRIX GetWorldMatrix();

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
	VERTEX modelVertices[8] = {
	{-1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 0.0f},    // vertex 0
	{1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f},    // vertex 1
	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f},    // 2 etc.
	{1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f},    
	{-1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f},    
	{1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f},
	};

	short modelIndices[36] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};


};