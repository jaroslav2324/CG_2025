
struct PS_IN
{
    float4 pos: SV_POSITION;
    float4 tex : TEXCOORD0;
    float3 norm: NORMAL;
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
    float2 farNear;
    Material material;
    float4 camPos;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

struct GBuffer{
    float4 depth: SV_Target0;
    float4 normal: SV_Target1;
    float4 diffuse: SV_Target2;
    float4 specExp: SV_Target3;
};

float convertZToDepth(float distance)
{
    float nearPlane = constData.farNear.x;
    float farPlane = constData.farNear.y;
    float z = distance;
    float depth = (farPlane + nearPlane) / (farPlane - nearPlane) - (2.0 * farPlane * nearPlane) / ((farPlane - nearPlane) * z);
    return 0.5f * (depth + 1.0f); 
}


GBuffer PSMain(PS_IN input)
{
    GBuffer buf = (GBuffer)0;
    buf.depth.x = convertZToDepth(input.pos.z);
    buf.normal.xyz = input.norm;
    buf.diffuse.rgb = constData.material.diffuse.rgb;
    buf.specExp.rgb = constData.material.speculiar.rgb;
    buf.specExp.w = constData.material.exponent.r;
    return buf;
}