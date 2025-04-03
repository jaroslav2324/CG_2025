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

//     float3 I = float3(0.0, 0.0, 0.0);
//     for (int i = 0; i < 4; i++){
//         LightSource ls = lights[i];
//         float3 L = normalize(ls.position.xyz - globalVertPos);
//         float3 V = normalize(constData.camPos.xyz-globalVertPos);
//         float3 N = normalize(input.norm.xyz);
//         float3 R = normalize(2 * dot(L, N) * N - L);
//         float shineCoeff = clamp(1 - length(ls.position.xyz - globalVertPos) / ls.shineDistance, 0.0f, 1.0f);
//         float3 Ia = constData.material.ambient.xyz;
//         float3 Is = shineCoeff * ls.rgb.xyz * clamp(constData.material.speculiar.xyz  * ls.intensity * pow(dot(V, R),constData.material.exponent.x), 0.0f, 1.0f);
//         float3 Id = shineCoeff * ls.rgb.xyz * constData.material.diffuse.xyz * ls.intensity * clamp(dot(L, N), 0.0f, 1.0f);
//         I = I + Id + Is + Ia / 4;
//     }

    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.viewMatrix);
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.projectionMatrix);

    output.globalPos = float4(globalVertPos, 0.0f);
    //output.color = float4(I, 0.0f);
    output.norm = input.norm;
    output.pos = input.pos;
    output.tex.xy = input.tex.xy;    

    return output;
}