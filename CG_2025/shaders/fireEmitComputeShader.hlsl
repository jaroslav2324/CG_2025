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

float distanceSquared(float3 a, float3 b)
{
    float3 d = a - b;
    return dot(d, d);
}

RWStructuredBuffer<Particle> particlePool : register(u0);
RWStructuredBuffer<SortListStruct> sortListBuffer: register(u1);
ConsumeStructuredBuffer<uint> deadListIn : register(u2);

StructuredBuffer<Particle> injectionBuffer : register(t0);

cbuffer ParticleParams : register(b0)
{
    float deltaTime;
    uint maxNumParticles;
    uint numEmitInThisFrame;
    uint numAliveParticles;
    float3 emitPosition;
    int _1;
    float3 cameraPosition;
    int _2;
    float4x4 view;
    float4x4 proj;
};

[numthreads(32, 32, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.y * 32 + DTid.x;
    uint numDeadParticles = maxNumParticles - numAliveParticles;
    if (index >= numDeadParticles || index >= numEmitInThisFrame || index >= maxNumParticles){
        return;
    }

    uint particleIndex = deadListIn.Consume();

    Particle p = injectionBuffer[index];

    particlePool[particleIndex] = p;
    SortListStruct sls;
    sls.index = particleIndex;
    sls.distanceSq = distanceSquared(p.position, cameraPosition);
    uint sortListIndex = sortListBuffer.IncrementCounter();
    sortListBuffer[sortListIndex] = sls;
}