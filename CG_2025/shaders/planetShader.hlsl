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

PS_IN VSMain(VS_IN input, uint vId : SV_VertexID)
{
    //float4 clipPos = mul(mul(mul(float4(input.pos.xyz, 1.0f), constData.transformMatrix), constData.viewMatrix), constData.projectionMatrix);
    float4 clipPos = mul(float4(input.pos.xyz, 1.0f), constData.transformMatrix);
    //clipPos.x = (clipPos.x / constData.screenCoords.x) * 2.0 - 1.0;
    //clipPos.y = (clipPos.y / constData.screenCoords.y) * 2.0 - 1.0;

    PS_IN output = (PS_IN) 0;
    
    output.pos = clipPos;
    output.col = input.col;
    
    return output;
}