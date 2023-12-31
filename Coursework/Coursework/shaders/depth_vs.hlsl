Texture2D heightMapTexture : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer GeometryBuffer : register(b1)
{
    float amplitude;
    float geometryType;
    float2 padding;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float getHeight(float2 uv)
{
    float height = heightMapTexture.SampleLevel(sampler0, uv, 0).r;
    
    return height;
}

OutputType main(InputType input)
{
    OutputType output;

    if (geometryType < 1.0f)
    {
        input.position.y = getHeight(input.tex) * amplitude;
    }
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
	
    return output;
}