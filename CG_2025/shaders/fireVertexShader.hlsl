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

StructuredBuffer<SortListStruct> sortListBuffer : register(t0);
StructuredBuffer<Particle> particlePool : register(t1);

struct VSInput
{
    uint vertexID : SV_VertexID;
};

struct VSOutput
{
    uint particleIndex : INDEX;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.particleIndex = sortListBuffer[input.vertexID].index;
    return output;
}
