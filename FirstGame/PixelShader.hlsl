Texture2D Texture;
SamplerState ss;

//pixel shader
float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 texcoord : TEXCOORD0, float4 diffuseColor : COLOR1) : SV_TARGET
{

	float4 new_color = color * Texture.Sample(ss, texcoord);
	//float4 new_color = color;
	
	[flatten]
	if (diffuseColor.a > 0.0f) {
		new_color.a = diffuseColor.a;
	}

	return new_color;
}
