struct VS_IN
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float4 worldPos : TEXCOORD1;
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

PS_IN VSMain(VS_IN input)
{

	PS_IN output = (PS_IN)0;
	output.pos = mul(float4(input.pos.xyz, 1.0f), constData.worldViewProj);
	output.worldPos = mul(float4(input.pos.xyz, 1.0f), constData.world);
	output.col = input.col;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{	 

	float4 color = input.col;

	return color;
}