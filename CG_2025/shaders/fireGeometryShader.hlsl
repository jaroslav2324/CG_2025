// Структура частицы
struct Particle {
	float4 initialColor;
	float4 endColor;
	float3 position;        float _1;
	float3 prevPosition;    float _2;
	float3 velocity;        float _3;
	float3 acceleration;    float _4;
	float maxLifetime;
	float lifetime;
	float initialSize;
	float endSize;
	float initialWeight;
	float endWeight;
};

cbuffer CameraData : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3 camRight;
    float3 camUp;
};

struct VSOutput
{
    uint particleIndex : INDEX;
};

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
    float4 color : COLOR0;
};

StructuredBuffer<Particle> particlePool : register(t0);

[maxvertexcount(4)]
void GSMain(point VSOutput input[1], inout TriangleStream<GSOutput> triStream)
{
    Particle p = particlePool[input[0].particleIndex];

    float3 center = p.position;
    float size = lerp(p.initialSize, p.endSize, p.lifetime / p.maxLifetime);
    float4 color = lerp(p.initialColor, p.endColor, p.lifetime / p.maxLifetime);

    float4 viewSpacePos = mul(float4(center, 1.0), view);
    center = viewSpacePos.xyz;

    float3 right = float3(1.0f, 0.0f, 0.0f) * size * 0.5f;
    float3 up    = float3(0.0f, 1.0f, 0.0f) * size * 0.5f;

    float3 positions[4] = {
        center - right + up, 
        center + right + up, 
        center + right - up,
        center - right - up
    };

    // TODO: need texture or not?
    float2 uvs[4] = {
        float2(0, 0),
        float2(1, 0),
        float2(1, 1),
        float2(0, 1)
    };

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        GSOutput o;
        o.pos = mul(float4(positions[i], 1.0), proj);
        o.uv = uvs[i];
        o.color = color;
        triStream.Append(o);
    }
}