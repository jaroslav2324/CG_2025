// Texture2D tex : register(t0);
// SamplerState samp : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
    float4 color : COLOR0;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
//     float4 texColor = tex.Sample(samp, input.uv);
//     return texColor * input.color;
}