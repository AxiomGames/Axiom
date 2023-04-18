
struct VS_INPUT
{
	float3 position : POSITION;
	float2 uv       : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

cbuffer cBuffer : register(b0)
{
    float4x4 wpMat;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
    o.position = mul(wpMat, float4(i.position, 1.0f));
	o.uv = i.uv;
	return o;
}

Texture2D _texture : register(t0);
SamplerState _sampler : register(s0);

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return _texture.Sample(_sampler, input.uv);
}