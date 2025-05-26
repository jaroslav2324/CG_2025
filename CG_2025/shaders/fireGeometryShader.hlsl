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
    float3 camForward;
    float _1;
    float3 camUp;
    float _2;
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

    float3 camRight = normalize(cross(camForward, camUp)); 

    // float3 forward = -normalize(view._13_23_33); 
    float3 up = normalize(view._12_22_32) * 0.5f * size;       
    float3 right = normalize(view._11_21_31) * 0.5f * size;    

    float3 positions[4] = {
        center - right + up,   
        center + right + up,   
        center - right - up,   
        center + right - up    
    };

    float2 uvs[4] = {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1)
    };

    GSOutput o;

    // Order: 0-1-2-3
    for (int i = 0; i < 4; i++){
        o.pos = mul(float4(positions[i], 1.0f), view); 
        o.pos /= o.pos.w;
        o.pos = mul(o.pos, proj);
        //o.pos /= o.pos.w;
        o.uv = uvs[i];
        o.color = color;
        triStream.Append(o);
    }
 
    triStream.RestartStrip();
}