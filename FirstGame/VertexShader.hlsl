struct VOut 
{
	float4 pos : SV_POSITION;
	float4 color: COLOR;
};

cbuffer ConstantBuffer
{
	float4x4 matFinal;
}

//Vertex Shader
VOut main(float4 pos : POSITION, float4 color : COLOR)
{

	//Create a VOut structure
	VOut output;

	//set the output values
	output.pos = mul(matFinal,pos); //transform vertices from 3D to 2D
	output.color = color;

	//return output values
	return output;
}
