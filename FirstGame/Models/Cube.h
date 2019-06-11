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
	void DrawObject(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject);
	void DrawShadow(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject);

private:

	//define cube vertices (need 3 for each 'vertex' when including normal vectors)
	VERTEX modelVertices[24] = {

		{-1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f},    // side 1
		{1.0f, -1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f},

		{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,  0.0f, 1.0f},    // side 2
		{-1.0f, 1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f},
		{1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f},

		{-1.0f, 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f},    // side 3
		{-1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f},
		{1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f},
		{1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f},

		{-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f},    // side 4
		{1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f},
		{1.0f, -1.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f},

		{1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f},    // side 5
		{1.0f, 1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f},
		{1.0f, -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f},

		{-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f},    // side 6
		{-1.0f, -1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f},
		{-1.0f, 1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f},
	};

	short modelIndices[36] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 5, 6,    // side 2
		6, 5, 7,
		8, 9, 10,    // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	MATERIAL mCubeMaterial;
	MATERIAL mShadowMaterial;
};