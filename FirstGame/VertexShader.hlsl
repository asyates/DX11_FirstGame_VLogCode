struct VOut 
{
	float4 pos : SV_POSITION;
	float4 color: COLOR;
	float2 texcoord: TEXCOORD;
};

cbuffer ConstantBuffer
{
	float4x4 matFinal;
	float4x4 rotation;    //the rotation matrix
	float4 lightvec;      // the diffuse light's vector
	float4 lightcol;      // the diffuse light's color
	float4 ambientcol;    // the ambient light's color
}

//Vertex Shader
VOut main(float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD )
{

	//Create a VOut structure
	VOut output;

	//set the output values
	output.pos = mul(matFinal,pos); //transform vertices from 3D to 2D
	output.color = ambientcol;

	float4 norm = normalize(mul(rotation, normal));    // rotate the normal vectors (to match object rotation), and normalise for vector length 1
	float diffusebrightness = saturate(dot(norm, lightvec));   // force to be between 0 and 1

	output.color += lightcol * diffusebrightness;    // find the diffuse color and add (getting bright than ambient)

	//set texture coordinates unmodified
	output.texcoord = texcoord;

	//return output values
	return output;
}
