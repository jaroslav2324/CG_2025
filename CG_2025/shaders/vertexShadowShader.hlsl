
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

struct VS_IN
{
    float4 position : POSITION0;
    float4 norm : NORMAL;
    float4 tex : TEXCOORD0;
};

struct PS_IN
{
    float4 position : SV_POSITION;
};


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    matrix modelMatrix = mul(scaleMatrix, mul(rotationMatrix, translationMatrix));
    float4 worldPos = mul(float4(input.position.xyz, 1.0f), modelMatrix);

    float4 lightViewPos = mul(worldPos, lightViewMatrix);
    output.position = mul(lightViewPos, lightProjMatrix);
	
    return output;
}