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

Texture2D<float4> depthAmbientTex: register(t0);
Texture2D<float4> diffuseNormalTex: register(t1);

cbuffer ParticleParams : register(b0)
{
    float deltaTime;
    uint maxNumParticles;
    uint numEmitInThisFrame;
    int numAliveParticles;
    float3 emitPosition;
    int _1;
    float3 cameraPosition;
    int _2;
    float4x4 view;
    float4x4 proj;
};

float3 unpackNormals(float packedLatitudeLongitude)
{
    uint packed = asint(packedLatitudeLongitude);
    uint val1 = (packed >> 16) & 0xFFFF;
    uint val2 = packed & 0xFFFF;
    float latitude = val1 / 65535.0;
    float longitude = val2 / 65535.0;

    float PI = 3.14159265f;
    // to radians
    latitude = latitude * PI;
    longitude = (longitude * 2.0f - 1.0) * PI;

    float x = sin(latitude) * cos(longitude);
    float y = sin(latitude) * sin(longitude);
    float z = cos(latitude);
    return normalize(float3(x, y, z));
}

float distanceSquared(float3 a, float3 b)
{
    float3 d = a - b;
    return dot(d, d);
}

#define NUM_THREADS_IN_GROUP 1024

[numthreads(32, 32, 1)]
void CSMain(uint3 DTid : SV_GroupThreadID,
 uint3 Gid : SV_GroupID)
{
    uint threadGroupOffset = NUM_THREADS_IN_GROUP * (Gid.x + Gid.y * 32);
    uint sortStructIndex = threadGroupOffset + DTid.y * 32 + DTid.x;

    if (sortStructIndex >= numAliveParticles || sortStructIndex >= maxNumParticles)
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
        p.initialColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
        p.endColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
        sortListBuffer.DecrementCounter();
    }
    else
    {
        float distSq = distanceSquared(p.position, cameraPosition);
        sls.distanceSq = distSq;

        float4 viewPartPos = mul(float4(p.position, 1.0f), view);
        viewPartPos /= viewPartPos.w;
        float4 projPartPos = mul(float4(viewPartPos.xyz, 1.0f), proj);
        projPartPos /= projPartPos.w;
        if (projPartPos.x < 1 && projPartPos.x > -1 && projPartPos.y < 1 && projPartPos.y > -1)
        {
            int x = (projPartPos.x + 1.0f) / 2.0f * 800.0f;
            int y = (-projPartPos.y + 1.0f) / 2.0f * 800.0f;
            float linearDepth = depthAmbientTex.Load(int3(x, y, 0)).x;
            if (linearDepth * linearDepth < distanceSquared(viewPartPos, float3(0.0f, 0.0f, 0.0f)) / ((10.0f - 0.01) * (10.0f - 0.01)) && p._1 >= 5.0  )
            {
                // reflect velocity
                float3 normal =unpackNormals( diffuseNormalTex.Load(int3(x, y, 0)).w);
                if (dot(normal, p.velocity) <= -0.01f){
                    p.velocity = p.velocity - 2 * dot(p.velocity, normal) * normal;
                    //p.velocity *= 0.8;
                    p._1 = 0.0;
                }
            }
        }
    }
    p._1 += 1.0;
if (p._1 > 10.0f)
{
p._1 = 0.0f;
}
    particlePool[particleIndex] = p;
    sortListBuffer[sortStructIndex] = sls;
}
