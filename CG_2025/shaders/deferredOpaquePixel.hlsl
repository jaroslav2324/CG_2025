
struct PS_IN
{
    float4 pos: SV_POSITION;
    float4 viewPos: POSITION0;
    float4 globalPos: POSITION1;
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
    float2 nearFar;
    Material material;
    float4 camPos;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

Texture2D myTexture : register(t0);
SamplerState samplerState: register(s0);

struct GBuffer{
    float4 depthAmbient: SV_Target0;
    float4 normal: SV_Target1;
    float4 diffuse: SV_Target2;
    float4 specExp: SV_Target3;
    float4 globalCoords: SV_Target4;
};

GBuffer PSMain(PS_IN input)
{
    GBuffer buf = (GBuffer)0;
    float nearPlane = constData.nearFar.x;
    float farPlane = constData.nearFar.y;
    float depth = (-input.viewPos.z - nearPlane) / (farPlane - nearPlane);
    float3 textureColor = myTexture.Sample(samplerState, input.tex.xy).rgb;

    buf.globalCoords = input.globalPos;
    buf.depthAmbient.x = depth;
    buf.depthAmbient.yzw = textureColor * constData.material.ambient.xyz;
    buf.normal.xyz = input.norm;
    buf.diffuse.rgb = textureColor * constData.material.diffuse.rgb;
    buf.specExp.rgb = constData.material.speculiar.rgb;
    buf.specExp.w = constData.material.exponent.r;
    return buf;
}