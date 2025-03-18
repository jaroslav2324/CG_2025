
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 tex : TEXCOORD0;
};

//struct ConstantData
//{
//    float4x4 transformMatrix;
//    float2 screenCoords;
//    float2 useless;
//};

Texture2D myTexture : register(t0);
SamplerState samplerState: register(s0);

//cbuffer ConstBuf : register(b0)
//{
//    ConstantData constData;
//}

float4 PSMain(PS_IN input) : SV_Target
{
    //float4 col = float4(0.5, 0.5, 0.5, 1.0);
    float4 col = myTexture.Sample(samplerState, input.tex.xy);
    return col;
}