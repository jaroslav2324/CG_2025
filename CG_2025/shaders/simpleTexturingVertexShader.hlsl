struct VS_IN
{
    float4 pos : POSITION0;
    float4 tex : TEXCOORD;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 tex : TEXCOORD;
};

struct ConstantData
{
    float4x4 transformMatrix;
    float2 screenCoords;
    float2 useless;
};

Texture2D texture : register(t0);
SamplerState samplerState: register(s0);

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

PS_IN VSMain(VS_IN input, uint vId : SV_VertexID)
{
    float4 clipPos = mul(float4(input.pos.xyz, 1.0f), constData.transformMatrix);

    PS_IN output = (PS_IN) 0;
    
    output.pos = clipPos;
    output.tex = input.tex;
    
    return output;
}