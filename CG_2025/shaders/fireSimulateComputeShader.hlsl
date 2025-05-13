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

struct SortListStruct{
    uint index;
    float distanceSq;
};

RWStructuredBuffer<Particle> particleBuffer : register(u0);
RWStructuredBuffer<SortListStruct> sortListBuffer: register(u1);
AppendStructuredBuffer<uint> deadListBuffer: register(u2);

cbuffer ParticleParams : register(b0)
{
    float deltaTime;
    uint numParticles;
    uint numEmitInTHisFrame;
    int _;
};

[numthreads(32, 24, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x * DTid.y;

    if (index >= numParticles)
        return;

    Particle p = particleBuffer[index];

    p.position += p.velocity * deltaTime;
    p.lifetime += deltaTime;

    if (p.lifetime >= p.maxLifetime)
    {
        p.position = float3(0, 0, 0);
        p.velocity = float3(0, 0, 0);
        p.lifetime = 0.0f;
    }

    particleBuffer[index] = p;
}