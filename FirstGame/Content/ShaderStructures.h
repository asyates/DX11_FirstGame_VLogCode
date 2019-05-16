#pragma once
using namespace DirectX;

// a struct to represent a single vertex
struct VERTEX
{
	float X, Y, Z;    // vertex position
	float NX, NY, NZ;	  // normal direction
};

//structure to represent constant buffer
struct CBUFFER
{
	XMMATRIX Final;
	XMMATRIX Rotation; //for rotating normals, ensuring correct lighting.
	XMVECTOR DiffuseVector;
	XMVECTOR DiffuseColor;
	XMVECTOR AmbientColor;
};
