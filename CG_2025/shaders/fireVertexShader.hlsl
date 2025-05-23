
struct SortListStruct{
    uint index;
    float distanceSq;
};

StructuredBuffer<SortListStruct> sortListBuffer : register(t0);

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
