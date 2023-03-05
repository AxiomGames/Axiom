
struct VS_INPUT
{
	float3 position : POSITION;
	float3 color    : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};


struct ModelViewProjection
{
    float4x4 MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.position = mul(ModelViewProjectionCB.MVP, float4(i.position, 1.0f));
	o.color = float4(i.color, 1.0f);
	return o;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.color.xyz, 1.0f);
}