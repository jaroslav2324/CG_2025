
enum LightSourceType {
    AMBIENT_LIGHT = 0,
    DIRECTIONAL_LIGHT = 1,
    POINT_LIGHT = 2,
    SPOT_LIGHT = 3
};

struct LightSource {
    float4 position;
    float4 direction;
    float4 rgb;
    LightSourceType sourceType;
    float shineDistance;
    float angle;
    float intensity;
};

struct ConstantData
{
    float4x4 transformMatrix;
	float4x4 scaleMatrix;
	float4x4 rotationMatrix;
	float4x4 translationMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
    float2 screenCoords;
    float2 useless;
    Material material;
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
    PS_IN output = (PS_IN) 0;
 
    switch  (ls.sourceType)
    {
        case AMBIENT_LIGHT:
        case DIRECTIONAL_LIGHT:
            output.tex = float2(id & 1, (id & 2) >> 1);
            output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);
            break;

        case POINT_LIGHT:
        case SPOT_LIGHT: 
            output.pos = mul(float4(input.pos.xyz, 1.0f), constData.viewMatrix);
            viewPos = output.pos;
            output.pos = mul(float4(input.pos.xyz, 1.0f), constData.projectionMatrix);
            break;
    }

    return output;
}