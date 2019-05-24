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
struct CBUFFER
{
	XMMATRIX Final;
	XMMATRIX Rotation; //for rotating normals, ensuring correct lighting.
	XMVECTOR DiffuseVector;
	XMVECTOR DiffuseColor;
	XMVECTOR AmbientColor;
};
