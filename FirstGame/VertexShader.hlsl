struct VOut 
{
	float4 pos : SV_POSITION;
	float4 color: COLOR;
	float2 texcoord: TEXCOORD0;
	float4 lightViewPos: TEXCOORD1;
	float4 lightPos: TEXCOORD2;
};

struct Material {
	float4 lightcol;      // the diffuse light's color
	float4 ambientcol;    // the ambient light's color
	float4 specPower;	  // specular light power
	float4 specColor;     // specular light color
};

cbuffer	cbPerObject
{
	float4x4 matFinal;
	float4x4 matWorld;    //the world matrix
	float4x4 matRotate;   // the rotation matrix
	Material gMaterial;
}

cbuffer cbPerFrame
{
	float4 lightDirection;      // the diffuse light's vector
	float4 eyePos; // camera position
};

//Vertex Shader
VOut main(float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{

	//Create a VOut structure
	VOut output;

	//set the output values
	output.pos = mul(matFinal,pos); //transform vertices from 3D to 2D
	output.color = gMaterial.ambientcol;

	//invert light direction
	float4 lightDir = -lightDirection;
	lightDir = normalize(lightDir);

	normal.w = 0;
	float4 norm = normalize(mul(matRotate, normal));    // rotate the normal vectors (to match object rotation), and normalise for vector length 1	
	float diffuseFactor = dot(norm, lightDir);
	float diffusebrightness = saturate(diffuseFactor);   // force to be between 0 and 1

	output.color += gMaterial.lightcol * diffusebrightness;    // find the diffuse color and add (getting bright than ambient)
	
	//Specular lighting
	//if surface in line of sight of the light
	if (diffuseFactor > 0.0f) {
		
		float4 r = reflect(-lightDir, norm);

		float4 viewDirection = normalize(eyePos - output.pos);

		//float test1 = dot(viewDirection, r);
		//float test2 = max(test1, 0.0f);
		//float test3 = pow(test2, gMaterial.specPower.w);

		float specFactor = pow(max(dot(viewDirection, r), 0.0f), gMaterial.specPower.w);
		
		output.color += gMaterial.specColor * specFactor; // add specular lighting
	}
	
	output.texcoord = texcoord; //set texture coordinates unmodified

	//return output values
	return output;
}
