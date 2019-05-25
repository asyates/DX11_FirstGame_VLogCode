#pragma once
using namespace DirectX;

// a struct to represent a single vertex
struct VERTEX
{
	float X, Y, Z;    // vertex position
	float NX, NY, NZ;	  // normal direction
	float U, V;		  // texture coordinates
};

//structure to represent constant buffer
struct CBUFFERPEROBJECT
{
	XMMATRIX matFinal;
	XMMATRIX matWorld; 
	XMMATRIX matRotate; //for rotating normals
};

struct CBUFFERPERFRAME {
	XMVECTOR DiffuseVector;
	XMVECTOR DiffuseColor;
	XMVECTOR SpecPower;
	XMVECTOR SpecColor;
	XMVECTOR AmbientColor;
	XMVECTOR EyePos; //for specular lighting
};
