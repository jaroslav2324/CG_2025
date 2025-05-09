
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
    float4 diffuseNormal: SV_Target1;
    float4 specExp: SV_Target2;
};

float2 normalToLatitudeLongitude(float3 normal)
{
    float PI = 3.14159265f;
    float theta = acos(normal.z);     
    float phi = atan2(normal.y, normal.x);  

    float latitude = theta / PI;
    float longitude = (phi + PI) / (2.0 * PI);

    return float2(latitude, longitude);
}

float packAsTwoUint16InFloat32(float2 valuesToPack)
{
    uint val1 = valuesToPack.x * 65535.0;
    uint val2 = valuesToPack.y * 65535.0;
    uint packed = ((val1 & 0xFFFF) << 16) | (val2 & 0xFFFF);
    return asfloat(packed);
}

GBuffer PSMain(PS_IN input)
{
    GBuffer buf = (GBuffer)0;
    float nearPlane = constData.nearFar.x;
    float farPlane = constData.nearFar.y;
    float depth = (-input.viewPos.z - nearPlane) / (farPlane - nearPlane);
    float3 textureColor = myTexture.Sample(samplerState, input.tex.xy).rgb;

    buf.depthAmbient.x = depth;
    buf.depthAmbient.yzw = textureColor * constData.material.ambient.xyz;
    buf.diffuseNormal.xyz = textureColor * constData.material.diffuse.rgb;
    buf.diffuseNormal.w = packAsTwoUint16InFloat32(normalToLatitudeLongitude(input.norm));
    buf.specExp.rgb = constData.material.speculiar.rgb;
    buf.specExp.w = constData.material.exponent.r;
    return buf;
}