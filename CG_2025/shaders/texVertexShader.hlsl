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
};

struct ConstantData
{
    float4x4 transformMatrix;
    float2 screenCoords;
    float2 useless;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

PS_IN VSMain(VS_IN input, uint vId : SV_VertexID)
{
    float mulCoeff = 7.0f;
    float sinArg = (constData.useless.x * 2.0f - 1.0f) * 3.14f * input.tex.w / 6.28f * mulCoeff;
    input.pos = input.pos + input.norm * input.tex.z * 2 * mulCoeff + mulCoeff * input.norm * sin(sinArg);
    float4 clipPos = mul(float4(input.pos.xyz, 1.0f), constData.transformMatrix);

    PS_IN output = (PS_IN) 0;
    
    output.pos = clipPos;
    output.tex.xy = input.tex.xy;
    
    return output;
}