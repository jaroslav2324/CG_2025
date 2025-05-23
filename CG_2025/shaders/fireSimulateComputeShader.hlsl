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

RWStructuredBuffer<Particle> particlePool : register(u0);
RWStructuredBuffer<SortListStruct> sortListBuffer: register(u1);
AppendStructuredBuffer<uint> deadListBuffer: register(u2);

cbuffer ParticleParams : register(b0)
{
    float deltaTime;
    uint maxNumParticles;
    uint numEmitInThisFrame;
    int numDeadParticles;
    float3 emitPosition;
    int _2;
};

// TODO move in constant buffer
float3 cameraPosition = float3(0, 0, 0); 

// TODO valid?
float distanceSquared(float3 a, float3 b)
{
    float3 d = a - b;
    return dot(d, d);
}

[numthreads(32, 24, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint sortStructIndex = DTid.y * 32 + DTid.x;

    uint numAliveParticles = maxNumParticles - numDeadParticles;
    if (sortStructIndex >= numAliveParticles)
        return;

    // simulate
    SortListStruct sls = sortListBuffer[sortStructIndex];
    uint particleIndex = sls.index;
    Particle p = particlePool[particleIndex];

    p.lifetime += deltaTime;
    p.prevPosition = p.position;
    p.position += p.velocity * deltaTime;
    p.velocity += p.acceleration * deltaTime;

    [branch]
    if (p.lifetime >= p.maxLifetime)
    {
        deadListBuffer.Append(particleIndex);
        sls.distanceSq = 100000.0f;
        p.initialColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
        sortListBuffer.DecrementCounter();
    }
    else
    {
        float distSq = distanceSquared(p.position, cameraPosition);
        sls.distanceSq = distSq;
    }
    particlePool[particleIndex] = p;
    sortListBuffer[sortStructIndex] = sls;
}
