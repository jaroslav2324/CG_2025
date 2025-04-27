
const int AMBIENT_LIGHT = 0;
const int DIRECTIONAL_LIGHT = 1;
const int POINT_LIGHT = 2;
const int SPOT_LIGHT = 3;

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
    LightSource light; 
};


struct LightSourceShadowMapData {
	matrix viewMatrix;
	matrix projectionMatrices[4];
    float4 distances;
	int textureSize;
};

cbuffer ShadowMapDataBuffer : register(b2) {
    LightSourceShadowMapData mapData; 
};

struct PS_IN
{
    float4 pos: SV_POSITION;
    float4 viewPos: POSITION0;
    float4 tex : TEXCOORD0;
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

Texture2DArray shadowMap : register(t1);
SamplerComparisonState shadowCmpSampler : register(s1);

Texture2D<float4> depthAmbientTex: register(t2);
Texture2D<float4> normalTex: register(t3);
Texture2D<float4> diffuseTex: register(t4);
Texture2D<float4> specExpTex: register(t5);

// int getLayer(int idx, float3 worldPos){
//     float depthFromCamera = length(constData.camPos.xyz - worldPos);
//     for (int i = 0; i < 4; i++){
//         if (depthFromCamera < mapData[idx].distances[i]){
//             return i;
//         }
//     }
//     return 3;
// }

// float getShadowCoeff(int idx, float3 worldPos, LightSource light)
// {
//     float depthFromCamera = length(constData.camPos.xyz - worldPos);
//     int layer = 3;
//     for (int i = 0; i < 4; i++){
//         if (depthFromCamera < mapData[idx].distances[i]){
//             layer = i;
//             break;
//         }
//     }
// 
//     matrix lightView = mapData[idx].viewMatrix;
//     matrix lightProj = mapData[idx].projectionMatrices[layer];
// 
//     float4 lightSpacePos = mul(float4(worldPos, 1.0f), lightView);
//     lightSpacePos = mul(lightSpacePos, lightProj);
// 
//     lightSpacePos.xyz /= lightSpacePos.w;
// 
//     float2 shadowUV = float2(lightSpacePos.x, -lightSpacePos.y) * 0.5f + 0.5f;
//     if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1)
//         return 1.0f;
// 
//     float depth = lightSpacePos.z;
//     float bias = 0.0005f;
// 
//     float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, float3(shadowUV, layer), depth - bias);
//     return lerp(0.3f, 1.0f, shadow); 
// }


float4 PSMain(PS_IN input) : SV_Target
{

    if (light.sourceType == AMBIENT_LIGHT)
    {
        float3 Ia = depthAmbientTex.Load(int3(input.pos.xy, 0)).yzw;
        return float4(Ia, 1.0);
    }
    else
    {
        // TODO: read depth?
        float3 normal = normalTex.Load(int3(input.pos.xy, 0)).xyz;
        float3 diffuse = diffuseTex.Load(int3(input.pos.xy, 0)).xyz;
        float4 specExp = specExpTex.Load(int3(input.pos.xy, 0));
        float3 specular = specExp.xyz;
        float exponent = specExp.w;

        float3 globalVertPos = mul(float4(input.pos.xyz, 1.0f), constData.inverseProjectionMatrix).xyz;
        globalVertPos = mul(float4(globalVertPos.xyz, 1.0f), constData.inverseViewMatrix).xyz;
        
        float shineCoeff = 1.0f;
        float3 L;
        if (light.sourceType == DIRECTIONAL_LIGHT)
        {
            L = -normalize(light.direction);
        }
        else 
        {
            // point or spot
            L = normalize(light.position.xyz - globalVertPos);
            shineCoeff = saturate(1 - length(light.position.xyz - globalVertPos) / light.shineDistance);
        }
        // out of light area
        if (light.sourceType == SPOT_LIGHT && dot(light.direction.xyz, -L) <= cos(light.angle))
        {
            return float4(0.0, 0.0, 0.0, 0.0);
        }

        float3 V = normalize(constData.camPos.xyz-globalVertPos);
        float3 N = normalize(normal);
        float3 R = normalize(2 * dot(L, N) * N - L);
        float3 Is = shineCoeff * light.rgb.xyz * saturate(specular  * light.intensity * pow(dot(V, R), exponent));
        float3 Id = shineCoeff * light.rgb.xyz * diffuse * light.intensity * saturate(dot(L, N));
        //float shadow = getShadowCoeff(i, globalVertPos, light);
        return float4(Id + Is, 1.0f); //* shadow;
    }
}