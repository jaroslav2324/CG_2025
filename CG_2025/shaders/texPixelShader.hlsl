
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

Texture2D myTexture : register(t0);
SamplerState samplerState: register(s0);

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = myTexture.Sample(samplerState, input.tex.xy);
    col *= float4(input.color.xyz, 1.0f);
    return col;
//return float4(input.color.xyz, 1.0f);
}