#pragma once
#include "pch.h"
#include "Model.h"
#include "../Content/ShaderStructures.h"

using namespace DirectX;
using namespace Platform;

class MeshGeometry : public Model {

public:

	MeshGeometry();

	// Methods to initialise and render graphics (using Model class methods)
	
	void Initialize(ComPtr<ID3D11Device> dev, std::string filename);
	OBJINFO ReadObjFileVertices(std::string filename);
	std::vector<VERTEX> CombineVerticesFromFile(std::vector<XMFLOAT3> vertices, std::vector<XMFLOAT3> normals, std::vector<XMFLOAT2> texcoords);
	VERTEX GetVertex(std::vector<XMFLOAT3> vertices, std::vector<XMFLOAT3> normals, std::vector<XMFLOAT2> texcoords, std::array<int, 3> vertexInfo);

	void DrawObject(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject);
	void DrawShadow(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject);


private:

	MATERIAL mMeshMaterial;
	MATERIAL mShadowMaterial;

	UINT vertArraySize;
	UINT indArraySize;

};