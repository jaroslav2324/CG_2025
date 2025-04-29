struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
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

PS_IN VSMain(VS_IN input)
{
    input.pos = mul(float4(input.pos.xyz, 1.0f), constData.transformMatrix);
    PS_IN output = (PS_IN) 0;
    output.pos = input.pos; 
    output.col = input.col;

    return output;
}