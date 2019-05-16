#pragma once
#include "pch.h"
#include "Model.h"
#include "../Content/ShaderStructures.h"

class GridFloor : public Model {

public:

	//Get vertices
	VERTEX * GetModelVertices();
	UINT GetVertArraySize();

	// Methods to initialise and render graphics (using Model class methods)
	void Initialize(ComPtr<ID3D11Device> dev);
	void Draw(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> constantbuffer, CBUFFER cbuffer);

private:

	VERTEX modelVertices[4] = {
	{ -1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f },
	{ -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f },
	{ 1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f },

	};

};