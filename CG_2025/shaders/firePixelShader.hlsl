Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct PSInput
{
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

// TODO: сделать по другому? убрать текстуры?
float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, input.uv);
    return texColor * input.color;
}