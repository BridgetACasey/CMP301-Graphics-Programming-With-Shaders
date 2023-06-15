struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output = (OutputType)0;

    //Passing through the vertex shader, as all vertex manipulation will be done in the geometry shader
    output.position = input.position;
    output.tex = input.tex;
    output.normal = input.normal;

    return output;
}