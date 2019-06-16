#pragma once
using namespace DirectX;

// a struct to represent a single vertex
struct VERTEX
{
	float X, Y, Z;    // vertex position
	float NX, NY, NZ;	  // normal direction
	float U, V;		  // texture coordinates
};

//material structure
struct MATERIAL
{
	MATERIAL() { ZeroMemory(this, sizeof(this)); }

	XMVECTOR DiffuseColor;
	XMVECTOR AmbientColor;
	XMVECTOR SpecColor; // w = specPower

};

//structure to represent constant buffer
struct CBUFFERPEROBJECT
{
	XMMATRIX matFinal;
	XMMATRIX matWorld; 
	XMMATRIX matRotate; //for rotating normals
	MATERIAL gMaterial;
};

struct CBUFFERPERFRAME {
	XMVECTOR DiffuseVector;
	XMVECTOR EyePos; //for specular lighting
};

struct OBJINFO {
	std::vector<VERTEX> Vertices;
	std::vector<short> Indices;
};
