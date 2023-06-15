Texture2D colourTexture : register(t0);

SamplerState Sampler0 : register(s0);

cbuffer BloomExtractBuffer : register(b0)
{
    float threshold;
    float intensity;
    float saturation;
    float sceneIntensity;
    float sceneSaturation;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour = colourTexture.Sample(Sampler0, input.tex);

    //Get a normalised value for the light colour based on the bloom threshold
    return saturate((textureColour - threshold) / (1 - threshold));
}