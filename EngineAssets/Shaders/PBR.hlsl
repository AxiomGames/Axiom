
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color    : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

VS_OUTPUT VS(VS_INPUT i)
{
	VS_OUTPUT o;
	o.position.xyz = i.position;
	o.position.w = 1;
	o.color = i.color;
	return o;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.color.xyz, 1);
}