Texture2D Texture;
SamplerState ss;

//pixel shader
float4 main(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD0, float4 lightViewPos : TEXCOORD1, float4 lightPos : TEXCOORD2) : SV_TARGET
{
	return color * Texture.Sample(ss, texcoord);
}
