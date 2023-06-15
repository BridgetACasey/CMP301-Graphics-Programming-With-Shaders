cbuffer DepthBuffer : register(b0)
{
    float near;
    float far;
    float2 padding;
}

struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float lineariseDepthValue(float depth)
{
    float z = depth * 2.0f - 1.0f;  //Converting back to normalised device coordinates
    
    return ((2.0f * near * far) / (far + near - z * (far - near)));
}

float4 main(InputType input) : SV_TARGET
{
    // Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    float depthValue = input.depthPosition.z / input.depthPosition.w;
    
    float linearDepth = lineariseDepthValue(depthValue);
    
    return float4(linearDepth, linearDepth, linearDepth, 1.0f);
}