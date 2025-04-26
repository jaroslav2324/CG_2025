
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

Texture2DArray shadowMap : register(t1);
SamplerComparisonState shadowCmpSampler : register(s1);

Texture2D<float4> depthTex: register(t2);
Texture2D<float4> normalTex: register(t3);
Texture2D<float4> diffuseTex: register(t4);
Texture2D<float4> specExpTex: register(t5);

int getLayer(int idx, float3 worldPos){
    float depthFromCamera = length(constData.camPos.xyz - worldPos);
    for (int i = 0; i < 4; i++){
        if (depthFromCamera < mapData[idx].distances[i]){
            return i;
        }
    }
    return 3;
}

float getShadowCoeff(int idx, float3 worldPos, LightSource light)
{
    float depthFromCamera = length(constData.camPos.xyz - worldPos);
    int layer = 3;
    for (int i = 0; i < 4; i++){
        if (depthFromCamera < mapData[idx].distances[i]){
            layer = i;
            break;
        }
    }

    matrix lightView = mapData[idx].viewMatrix;
    matrix lightProj = mapData[idx].projectionMatrices[layer];

    float4 lightSpacePos = mul(float4(worldPos, 1.0f), lightView);
    lightSpacePos = mul(lightSpacePos, lightProj);

    lightSpacePos.xyz /= lightSpacePos.w;

    float2 shadowUV = float2(lightSpacePos.x, -lightSpacePos.y) * 0.5f + 0.5f;
    if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1)
        return 1.0f;

    float depth = lightSpacePos.z;
    float bias = 0.0005f;

    float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, float3(shadowUV, layer), depth - bias);
    return lerp(0.3f, 1.0f, shadow); 
}


float4 PSMain(PS_IN input) : SV_Target
{
    float3 globalVertPos = input.globalPos.xyz;
    float3 I = float3(0.0, 0.0, 0.0);
    float3 Ia = constData.material.ambient.xyz;
    for (int i = 1; i < 2; i++){
        LightSource ls = lights[i];
        float3 L;
        // Directional light
        if (ls.sourceType == 0){
            L = -normalize(ls.direction);
        }
        // Point light
        else{
            L = normalize(ls.position.xyz - globalVertPos);
        }
        float3 V = normalize(constData.camPos.xyz-globalVertPos);
        float3 N = normalize(input.norm.xyz);
        float3 R = normalize(2 * dot(L, N) * N - L);
        float shineCoeff = clamp(1 - length(ls.position.xyz - globalVertPos) / ls.shineDistance, 0.0f, 1.0f);
        float3 Is = shineCoeff * ls.rgb.xyz * clamp(constData.material.speculiar.xyz  * ls.intensity * pow(dot(V, R),constData.material.exponent.x), 0.0f, 1.0f);
        float3 Id = shineCoeff * ls.rgb.xyz * constData.material.diffuse.xyz * ls.intensity * clamp(dot(L, N), 0.0f, 1.0f);
        float shadow = getShadowCoeff(i, globalVertPos, ls);
        int layer = getLayer(i, globalVertPos);
        if (shadow < 0.9f)
        {
            if (layer == 0){
                I = I + (Id + Is) * shadow * float3(2.0f, 0.0f, 0.0f);
            }
            else if (layer == 1){
                I = I + (Id + Is) * shadow * float3(0.0f, 2.0f, 0.0f);
            }
            else if (layer == 2){
                I = I + (Id + Is) * shadow * float3(0.0f, 0.0f, 2.0f);
            }
            else if (layer == 3){
                I = I + (Id + Is) * shadow * float3(3.0f, 0.0f, 3.0f);
            }
        }
        else   {
            I = I + (Id + Is) * shadow;
        }  
    }
    I = I + Ia;

    float4 col = myTexture.Sample(samplerState, input.tex.xy);
    col *= float4(I.xyz, 1.0f);
    return col;
}