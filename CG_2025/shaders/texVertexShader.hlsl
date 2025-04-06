struct LightSource {
    float4 position;
    float4 direction;
    float4 rgb;
    int sourceType;
    float shineDistance;
    float angle;
    float intensity;
};

cbuffer LightBuffer : register(b1) {
    LightSource lights[10]; 
    int lightCount;
};

struct VS_IN
{
    float4 pos : POSITION0;
    float4 norm : NORMAL;
    float4 tex : TEXCOORD0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 norm: NORMAL;
    float4 tex : TEXCOORD0;
    float4 color: COLOR;
    float4 globalPos: POSITION1;
};

struct Material {
	float4 ambient;
	float4 diffuse;
	float4 speculiar;
	float4 exponent;
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

PS_IN VSMain(VS_IN input)
{
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.scaleMatrix);
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.rotationMatrix);
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.translationMatrix);
    float3 globalVertPos = input.pos.xyz;

    input.norm = mul(float4(input.norm.xyz, 0.0f), constData.rotationMatrix);
    input.norm.w = 0.0f;

    PS_IN output = (PS_IN) 0;

    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.viewMatrix);
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.projectionMatrix);

    output.globalPos = float4(globalVertPos, 0.0f);
    output.norm = input.norm;
    output.pos = input.pos;
    output.tex.xy = input.tex.xy;    

    return output;
}