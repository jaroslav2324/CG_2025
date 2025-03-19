struct LightSource {
    float4 position;
    float4 direction;
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
    float4 tex : TEXCOORD0;
    float4 color: COLOR;
    float4 V: NORMAL0;
    float4 N: NORMAL1;
    float4 L: NORMAL2;
    float4 R: NORMAL3;
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
    float4x4 rotationMatrix;
    float2 screenCoords;
    float2 useless;
    Material material;
    float4 camPos;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

PS_IN VSMain(VS_IN input, uint vId : SV_VertexID)
{
    //float mulCoeff = 7.0f;
    //float sinSign = (vId % 2 == 0) ? 1.0f : -1.0f;
    //float sinArg = sinSign * (constData.useless.x * 2.0f - 1.0f) * 3.14f * input.tex.w / 6.28f * mulCoeff;
    //input.pos = input.pos + input.norm * input.tex.z * 2 * mulCoeff + mulCoeff * input.norm * sin(sinArg);
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.transformMatrix);
    input.norm = mul(float4(input.norm.xyz, 0.0f), constData.rotationMatrix);
    input.norm = float4(normalize(input.norm.xyz), 0.0f);

    PS_IN output = (PS_IN) 0;

    LightSource ls = lights[0];
    float4 L = normalize(ls.position - input.pos);
    float4 V = normalize(-input.pos);
    float4 N = normalize(input.norm);
    float4 R = normalize(2 * dot(L, N) * N - L);
    float3 Ia = 0.25f * constData.material.ambient.xyz;
    float3 Is = clamp(constData.material.speculiar.xyz  * ls.intensity * pow(dot(V, R),constData.material.exponent.x / 20.0f), 0.0f, 1.0f);
    float3 Id = constData.material.diffuse.xyz * ls.intensity * clamp(dot(L, N), 0.0f, 1.0f);
    float3 I = Id + Is + Ia;

    output.L = L;
    output.R = R;
    output.N = N;
    output.V = V;
    output.color = float4(I, 0.0f);
    output.pos = input.pos;
    output.tex.xy = input.tex.xy;    

    return output;
}