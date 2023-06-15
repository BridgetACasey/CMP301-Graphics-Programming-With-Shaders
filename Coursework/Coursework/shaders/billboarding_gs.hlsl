cbuffer MatrixBuffer : register(b0)
{
   matrix worldMatrix;
   matrix viewMatrix;
   matrix projectionMatrix;
}

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float padding;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

static const float2 vertexOffsets[4] =
{
    float2(-1.0f, -1.0f), //v0,lower-left
    float2(-1.0f, 1.0f), //v1,upper-left
    float2(1.0f, -1.0f), //v2,lower-right
    float2(1.0f, 1.0f) //v3,upper-right
};

[maxvertexcount(4)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
    float4 worldPosition = mul(input[0].position, worldMatrix);
    
    float3 up = float3(0.0f, 1.0f, 0.0f);   //Global up vector
    float3 forward = normalize(cameraPosition - input[0].position.xyz);
    float3 right = normalize(cross(up, forward));
    
    //Create the four vertices
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        OutputType output = (OutputType)0;
        
        output.position = float4(worldPosition.xyz + (right * vertexOffsets[i].x) + (up * vertexOffsets[i].y), 1.0f);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        
        output.tex = ((vertexOffsets[i] * float2(1.0f, -1.0f)) / 2.0f) + 0.5f;
        
        output.normal = forward;

        triStream.Append(output);
    }
}