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
    float4 offset;
    float4 color;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData constData;
}

PS_IN VSMain(VS_IN input, uint vId : SV_VertexID)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = float4(input.pos.xyz + constData.offset.xyz, 1.0f);
    output.col = input.col;
	
    return output;
}