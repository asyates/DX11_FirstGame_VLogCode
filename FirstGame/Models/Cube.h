#pragma once
#include "pch.h"
#include "Model.h"
#include "../Content/ShaderStructures.h"

using namespace DirectX;

class Cube : public Model { 

public:
	Cube();

	//Get vertices
	VERTEX * GetModelVertices();
	UINT GetVertArraySize();
	
	//Get indices
	short * GetModelIndices();
	UINT GetIndArraySize();

	// Methods to initialise and render graphics (using Model class methods)
	void Initialize(ComPtr<ID3D11Device> dev);
	void Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> constantbuffer, XMMATRIX matFinal);

private:

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