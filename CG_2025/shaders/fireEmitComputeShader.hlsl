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
ConsumeStructuredBuffer<uint> deadListIn : register(u1);

cbuffer ParticleParams : register(b0)
{
    float deltaTime;
    uint numParticles;
    uint numEmitInTHisFrame;
    int _1;
    float3 emitPosition;
    int _2;
};

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    for (uint i = 0; i < numEmitInTHisFrame; ++i)
    {
        uint index = deadListIn.Consume();

        Particle p;
        p.position = emitPosition;
        p.prevPosition = emitPosition;
        p.velocity = float3(0.0f, 1.0f, 0.0f) * 0.5f;
        p.acceleration = float3(0, -0.0981f, 0); 
        p.maxLifetime = 3.0f;
        p.lifetime = 0.0f;

        p.initialColor = float4(1, 0.5, 0.0, 1);  
        p.endColor = float4(0.1, 0.1, 0.1, 0);  

        p.initialSize = 0.5;
        p.endSize = 0.5;

        p.initialWeight = 1.0;
        p.endWeight = 1.0;

        p._1 = 0; p._2 = 0; p._3 = 0; p._4 = 0; // padding

        particlePool[index] = p;
    }
}