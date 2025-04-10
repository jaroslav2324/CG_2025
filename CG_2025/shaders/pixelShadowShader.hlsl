
struct Material {
	float4 ambient;
	float4 diffuse;
	float4 speculiar;
	float4 exponent;
};

cbuffer ShadowObjectTransform : register(b0)
{
    matrix transformMatrix;
    matrix scaleMatrix;
    matrix rotationMatrix;
    matrix translationMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float2 screenCoords;
    float2 unused;
    Material material;
    float4 camPos;
};

cbuffer ShadowLightData : register(b1) 
{
    matrix lightViewMatrix;
    matrix lightProjMatrix;
    float4 lightSourcePos;
};


struct PS_IN
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};


float4 PSMain(PS_IN input) : SV_TARGET
{
    float distToLight = length(input.worldPos - lightSourcePos.xyz);
    float normalizedDepth = distToLight / lightSourcePos.w;

    normalizedDepth = saturate(normalizedDepth);

    return float4(normalizedDepth, normalizedDepth, normalizedDepth, 1.0f);
}