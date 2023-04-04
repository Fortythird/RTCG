struct VS_IN
{
	float4 pos : POSITION;
	float2 texCoord : TEXCOORD;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct ConstantData
{
	float4x4 worldViewProj;
	float4x4 world;
	float4x4 invertedWorldTransform;
};

cbuffer ConstBuf:register(b0)
{
	ConstantData constData;
};

Texture2D TextureImg:register(t0);
SamplerState Sampler:register(s0);

PS_IN VSMain(VS_IN input)
{

	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), constData.worldViewProj);
	output.uv = input.texCoord;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{	 
	return TextureImg.SampleLevel(Sampler,input.uv.yx,0);
}