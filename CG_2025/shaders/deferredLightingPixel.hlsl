


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
    float4 camDir;
    int flags;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

Texture2DArray shadowMap : register(t0);
SamplerComparisonState shadowCmpSampler : register(s1);

Texture2D<float4> depthAmbientTex: register(t1);
Texture2D<float4> diffuseNormalTex: register(t2);
Texture2D<float4> specExpTex: register(t3);
Texture2D<float> nonlinearDepthTex: register(t4);

float3 unpackNormals(float packedLatitudeLongitude)
{
    uint packed = asint(packedLatitudeLongitude);
    uint val1 = (packed >> 16) & 0xFFFF;
    uint val2 = packed & 0xFFFF;
    float latitude = val1 / 65535.0;
    float longitude = val2 / 65535.0;

    float PI = 3.14159265f;
    // to radians
    latitude = latitude * PI;
    longitude = (longitude * 2.0f - 1.0) * PI;

    float x = sin(latitude) * cos(longitude);
    float y = sin(latitude) * sin(longitude);
    float z = cos(latitude);
    return normalize(float3(x, y, z));
}

float getShadowCoeff(float depth, float3 worldPos)
{
    int layer = 3;
    for (int i = 0; i < 4; i++){
        if (depth < mapData.distances[i] / 9.9f){
            layer = i;
            break;
        }
    }

    matrix lightView = mapData.viewMatrix;
    matrix lightProj = mapData.projectionMatrices[layer];

    float4 lightSpacePos = mul(float4(worldPos, 1.0f), lightView);
    lightSpacePos = mul(lightSpacePos, lightProj);

    lightSpacePos.xyz /= lightSpacePos.w;

    float2 shadowUV = float2(lightSpacePos.x, -lightSpacePos.y) * 0.5f + 0.5f;
    if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1)
        return 1.0f;

    depth = lightSpacePos.z;
    float bias = 0.0015f;

    float shadow = shadowMap.SampleCmpLevelZero(shadowCmpSampler, float3(shadowUV, layer), depth - bias);
    return lerp(0.3f, 1.0f, shadow); 
}


float4 PSMain(PS_IN input) : SV_Target
{
    const int AMBIENT_LIGHT = 0;
    const int DIRECTIONAL_LIGHT = 1;
    const int POINT_LIGHT = 2;
    const int SPOT_LIGHT = 3;

    float4 depthAmbient = depthAmbientTex.Load(int3(input.pos.xy, 0));

    [branch]
    if (light.sourceType == AMBIENT_LIGHT)
    {
        // ambient
        return light.intensity * float4(depthAmbient.yzw, 1.0);
    }
    else
    {
        float depth = depthAmbient.x;
        float nonlinearDepth = nonlinearDepthTex.Load(int3(input.pos.xy, 0));
        float4 diffuseNormal = diffuseNormalTex.Load(int3(input.pos.xy, 0));
        float3 diffuse = diffuseNormal.xyz;
        float3 normal =unpackNormals(diffuseNormal.w) ;
        float4 specExp = specExpTex.Load(int3(input.pos.xy, 0));
        float3 specular = specExp.xyz;
        float exponent = specExp.w;

        float ndcX = input.pos.x / 800.0f * 2.0f - 1.0f;
        float ndcY = -(input.pos.y / 800.0f * 2.0f - 1.0f);
        float4 viewSpaceVertPos = mul(float4(ndcX, ndcY, nonlinearDepth, 1.0f), constData.inverseProjectionMatrix);
        viewSpaceVertPos /= viewSpaceVertPos.w;
        float4 globalSpaceVertPos = mul(viewSpaceVertPos, constData.inverseViewMatrix);
        globalSpaceVertPos /= globalSpaceVertPos.w;
        float3 globalVertPos = globalSpaceVertPos.xyz;

        float shineCoeff = 1.0f;
        float3 L;
        float shadow = 1.0f;

        [branch]
        if (light.sourceType == DIRECTIONAL_LIGHT)
        {
            L = -normalize(light.direction);
            shadow = getShadowCoeff(depth, globalVertPos);
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
        return float4(Id + Is, 1.0f) * shadow; 
    }
}