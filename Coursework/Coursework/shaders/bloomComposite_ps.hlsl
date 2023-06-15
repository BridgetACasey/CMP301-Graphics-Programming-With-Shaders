Texture2D colourTexture : register(t0);
Texture2D bloomTexture : register(t1);

SamplerState Sampler0 : register(s0);

static const float3 GrayScaleIntensity = { 0.299f, 0.587f, 0.114f };

cbuffer BloomCompositeBuffer : register(b0)
{
    float threshold;
    float intensity;
    float saturation;
    float sceneIntensity;
    float sceneSaturation;
    float3 padding;
};

float4 adjustSaturation(float4 colour, float saturation)
{
    float intensity = dot(colour.rgb, GrayScaleIntensity);
    
    return float4(lerp(intensity.rrr, colour.rgb, saturation), colour.a);
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float4 sceneColour = colourTexture.Sample(Sampler0, input.tex);
    float4 bloomColour = bloomTexture.Sample(Sampler0, input.tex);
    
    //Adjust saturation based on user input from GUI
    sceneColour = adjustSaturation(sceneColour, sceneSaturation) * sceneIntensity;
    bloomColour = adjustSaturation(bloomColour, saturation) * intensity;
    
    //Blend the scene with the glow map
    sceneColour *= (1 - saturate(bloomColour));
    
    return (sceneColour + bloomColour);
}