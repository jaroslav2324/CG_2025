// ��������� �������
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

// TODO move in constant buffer
float3 cameraPosition = float3(0, 0, 0); 

// TODO valid?
float distanceSquared(float3 a, float3 b)
{
    float3 d = a - b;
    return dot(d, d);
}

RWStructuredBuffer<Particle> particlePool : register(u0);
RWStructuredBuffer<SortListStruct> sortListBuffer: register(u1);
ConsumeStructuredBuffer<uint> deadListIn : register(u2);

cbuffer ParticleParams : register(b0)
{
    float deltaTime;
    uint maxNumParticles;
    uint numEmitInThisFrame;
    uint numDeadParticles;
    float3 emitPosition;
    int _2;
};

[numthreads(32, 32, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.y * 32 + DTid.x;
    if (index >= numDeadParticles || index >= numEmitInThisFrame || index >= maxNumParticles){
        return;
    }

    uint numAliveParticles = maxNumParticles - numDeadParticles;
    uint particleIndex = deadListIn.Consume();

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

    particlePool[particleIndex] = p;
    SortListStruct sls;
    sls.index = particleIndex;
    sls.distanceSq = distanceSquared(p.position, cameraPosition);
    sortListBuffer[numAliveParticles + index] = sls;
    sortListBuffer.IncrementCounter();
}