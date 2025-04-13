
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
    float4 lightSourcePos; // xyz Ч позици€ источника света, w Ч shineDistance
};


struct PS_IN
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};


float4 PSMain(PS_IN input) : SV_TARGET
{
    // обман

    return float4 (0.0f, 0.0f, 0.0f, 0.0f);
    //return float4(normalizedDepth, normalizedDepth, normalizedDepth, 1.0f);
}