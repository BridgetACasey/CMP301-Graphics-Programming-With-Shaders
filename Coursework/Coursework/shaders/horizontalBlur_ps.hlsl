Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float lowerBound;
    float upperBound;
    float standDev;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

static const float twoPi = 6.28319;
static const float e = 2.71828;
static const int kernelSize = 21;

float gaussianHorizontal(int x)
{
    float sigmaSquared = standDev * standDev;
    
    return ((1.0f / sqrt(twoPi * sigmaSquared)) * pow(e, -(x * x) / (2.0f * sigmaSquared)));
}

float4 main(InputType input) : SV_TARGET
{
    //Sampling the texture
    uint width, height, levels;
    shaderTexture.GetDimensions(0, width, height, levels);
    
    float uOffset = 1.0f / width;
    float kernelSum = 0.0f;
    float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //Summing sampled colour values with respect to a gaussian factor
    for (int x = lowerBound; x <= upperBound; x++)
    {
        float gaussian = gaussianHorizontal(x);
        kernelSum += gaussian;
        colour += gaussian * shaderTexture.Sample(SampleType, (input.tex + float2(uOffset * x, 0.0f)));
    }
    
    colour /= kernelSum;

    return colour;
}