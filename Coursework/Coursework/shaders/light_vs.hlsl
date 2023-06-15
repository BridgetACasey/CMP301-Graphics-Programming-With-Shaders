Texture2D heightMapTexture : register(t0);

SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[4];
    matrix lightProjectionMatrix[4];
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float time;
}

cbuffer VertexManipulationBuffer : register(b2)
{
    float amplitude;
    float renderType;
    float terrainResolution;
    float geometryType;
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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float4 lightViewPosition[4] : TEXCOORD3;
};

float3 estimateNormalsByHeightMap(float2 tex)
{
    uint levels, width, height;
    heightMapTexture.GetDimensions(0, width, height, levels);
    
    //Per vertex rendering
    
    //Space between height values in normalised uv space (0,1)
    float texelCellSpaceU = 1.0f / terrainResolution;
    float texelCellSpaceV = 1.0f / terrainResolution;
    //Space between cells in world space
    float worldCellSpace = 1.0f;
    
    float2 leftTex = tex + float2(-texelCellSpaceU, 0.0f);
    float2 rightTex = tex + float2(texelCellSpaceU, 0.0f);
    float2 topTex = tex + float2(0.0f, -texelCellSpaceV);
    float2 bottomTex = tex + float2(0.0f, texelCellSpaceV);
    
    float leftY = heightMapTexture.SampleLevel(sampler0, leftTex, 0).r;
    float rightY = heightMapTexture.SampleLevel(sampler0, rightTex, 0).r;
    float topY = heightMapTexture.SampleLevel(sampler0, topTex, 0).r;
    float bottomY = heightMapTexture.SampleLevel(sampler0, bottomTex, 0).r;
    
    float3 tangent = normalize(float3(2.0f * worldCellSpace, (rightY - leftY) * amplitude, 0.0f));
    float3 bitan = normalize(float3(0.0f, (bottomY - topY) * amplitude, -2.0f * worldCellSpace));

    return cross(tangent, bitan);
}

float getHeight(float2 uv)
{
    float height = heightMapTexture.SampleLevel(sampler0, uv, 0).r;
    
    return height;
}

OutputType main(InputType input)
{
    OutputType output;
    
    if(geometryType < 1.0f) //If working with the terrain
    {
        input.position.y = getHeight(input.tex) * amplitude;
        
        if(renderType >= 2.0f)  //If per vertex normals are selected
        {
            input.normal = estimateNormalsByHeightMap(input.tex);
        }
    }
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.worldPosition = mul(input.position, worldMatrix);
    
    output.viewVector = cameraPosition.xyz - output.worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);
    
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Calculate the position of the vertice as viewed by the light source.
    
    for (int i = 0; i < 4; i++)
    {
        output.lightViewPosition[i] = mul(input.position, worldMatrix);
        output.lightViewPosition[i] = mul(output.lightViewPosition[i], lightViewMatrix[i]);
        output.lightViewPosition[i] = mul(output.lightViewPosition[i], lightProjectionMatrix[i]);
    }

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}