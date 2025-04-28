
struct LightSource {
    float4 position;
    float4 direction;
    float4 rgb;
    int sourceType;
    float shineDistance;
    float angle;
    float intensity;
};

struct ConstantData
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
    float4x4 inverseViewMatrix;
	float4x4 inverseProjectionMatrix;
    float4 camPos;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

cbuffer Light : register(b1)
{
    LightSource ls;
}

struct VS_IN
{
    float4 pos: SV_POSITION;
};

struct PS_IN
{
    float4 pos: SV_POSITION;
    float4 viewPos: POSITION0;
    float4 tex : TEXCOORD0;
};

PS_IN VSMain(VS_IN input, uint id: SV_VertexID)
{
    const int AMBIENT_LIGHT = 0;
    const int DIRECTIONAL_LIGHT = 1;
    const int POINT_LIGHT = 2;
    const int SPOT_LIGHT = 3;

    PS_IN output = (PS_IN) 0;
 
    if (ls.sourceType == AMBIENT_LIGHT || ls.sourceType == DIRECTIONAL_LIGHT)
    {
        output.tex.xy = float2(id & 1, (id & 2) >> 1);
        output.pos = float4(output.tex.xy * float2(2, -2) + float2(-1, 1), 0, 1);
    }
    else if (ls.sourceType == POINT_LIGHT || ls.sourceType == SPOT_LIGHT)
    {
        output.pos = mul(float4(input.pos.xyz, 1.0f), constData.viewMatrix);
        output.viewPos = output.pos;
        output.pos = mul(output.pos, constData.projectionMatrix);
    }

    return output;
}