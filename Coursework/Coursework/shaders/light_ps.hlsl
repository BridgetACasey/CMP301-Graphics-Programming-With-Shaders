// Light pixel shader

Texture2D texture0 : register(t0);
Texture2D depthMapTextures[4] : register(t1);
Texture2D heightMapTexture : register(t5);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1);

struct Light
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
    float range;
    float3 position;
    float exponent;
    float3 attenuation;
    float specularPower;
    float4 lightType;
    float shadowBias;
    float nearPlane;
    float farPlane;
    float softShadowsEnabled;
    float softenRadius;
    float projectionType;
    float renderShadows;
    float padding;
};

cbuffer LightBuffer : register(b0)
{
    Light lights[4];
};

cbuffer VertexManipulationBuffer : register(b1)
{
    float amplitude;
    float renderType;
    float terrainResolution;
    float geometryType;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float4 lightViewPosition[4] : TEXCOORD3;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateDirectional(Light light, float3 normal)
{
    float intensity = saturate(dot(normal, -light.direction));
    float4 colour = saturate(light.diffuse * intensity);
  
    return colour + light.ambient;
}

float4 calculatePoint(Light light, float3 wPos, float3 normal)
{
    //Get the vector to the light source from the surface for omnidirectional lighting
    float3 lightVector = light.position - wPos;
    float distanceToSurface = length(lightVector);
    
    lightVector = normalize(lightVector);
	
    float intensity = saturate(dot(normal, lightVector));
    float4 diffuse = saturate(intensity * light.diffuse);
	
    float3 baseAttenuation = float3(1.0f, distanceToSurface, distanceToSurface * distanceToSurface);
    
    float attenuation = 1.0f / dot(light.attenuation, baseAttenuation);
    float4 ambient = light.ambient;
	
    return (ambient * attenuation) + (diffuse * attenuation);
}

float4 calculateSpot(Light light, float3 wPos, float3 normal)
{
    float3 lightVector = light.position - wPos;
    float distanceToSurface = length(lightVector);
    
    if (distanceToSurface > light.range)
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
	
    lightVector = normalize(lightVector);

    float intensity = saturate(dot(normal, lightVector));
    float4 diffuse = saturate(intensity * light.diffuse);
	  
    //lamberts cosine rule
    float spot = pow(max(dot(lightVector, -light.direction), 0.0f), light.exponent);
    
    float3 baseAttenuation = float3(1.0f, distanceToSurface, distanceToSurface * distanceToSurface);
    float attenuation = spot / dot(light.attenuation, baseAttenuation);
    
    float4 ambient = light.ambient;

    return ambient * spot + (diffuse * attenuation);
}

float4 calculateSpecular(Light light, float3 viewVector, float3 normal)
{
    // blinn-phong specular calculation
    float3 halfway = normalize(light.direction + viewVector);
    
    float specularIntensity = pow(max(dot(normal, halfway), 0.0f), light.specularPower);
    
    return saturate(light.specular * specularIntensity);
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    
    projTex *= float2(0.5f, -0.5f);
    projTex += float2(0.5f, 0.5f);
    
    return projTex;
}

float3 estimateNormalsByHeightMap(float2 tex)
{
    uint levels, width, height;
    heightMapTexture.GetDimensions(0, width, height, levels);
    
    //Per pixel rendering
    
    //Space between height values in normalised uv space (0,1)
    float texelCellSpaceU = 1.0f / (float) width;
    float texelCellSpaceV = 1.0f / (float) height;
    //Space between cells in world space
    float worldCellSpace = 1.0f / ((float) width / terrainResolution);
    
    float2 leftTex = tex + float2(-texelCellSpaceU, 0.0f);
    float2 rightTex = tex + float2(texelCellSpaceU, 0.0f);
    float2 topTex = tex + float2(0.0f, -texelCellSpaceV);
    float2 bottomTex = tex + float2(0.0f, texelCellSpaceV);
    
    float leftY = heightMapTexture.SampleLevel(diffuseSampler, leftTex, 0).r;
    float rightY = heightMapTexture.SampleLevel(diffuseSampler, rightTex, 0).r;
    float topY = heightMapTexture.SampleLevel(diffuseSampler, topTex, 0).r;
    float bottomY = heightMapTexture.SampleLevel(diffuseSampler, bottomTex, 0).r;
    
    float3 tangent = normalize(float3(2.0f * worldCellSpace, (rightY - leftY) * amplitude, 0.0f));
    float3 bitan = normalize(float3(0.0f, (bottomY - topY) * amplitude, -2.0f * worldCellSpace));

    return cross(tangent, bitan);
}

float softenShadowEdges(Light light, float4 lightViewPosition, Texture2D depthMap, float2 tex, float3 worldPos)
{
    float shadow = 0.0f;
    float levels;
    float2 depthMapSize;
    
    depthMap.GetDimensions(0, depthMapSize[0], depthMapSize[1], levels);
    
    float2 texelSize = 1.0f / depthMapSize;
    float depth = depthMap.Sample(shadowSampler, tex.xy).r;
    float lightDepth = lightViewPosition.z / lightViewPosition.w;
    float pixelCount = 0.0f;
    
    //Sampling depth values around a point for a given radius
    for (int x = -light.softenRadius; x <= light.softenRadius; ++x)
    {
        for (int y = -light.softenRadius; y <= light.softenRadius; ++y)
        {
            float nextDepth = depthMap.Sample(shadowSampler, tex.xy + (float2(x, y) * texelSize)).r;
            shadow += (lightDepth - light.shadowBias) > nextDepth ? 1.0f : 0.0f;
            pixelCount += 1.0f;
        }
    }
    
    //Checking if the current light is a spotlight and ensuring the tex coords are normalised
    if(light.lightType.z == 1.0f)
    {
        if (tex.x < 0.0f || tex.x > 1.0f || tex.y < 0.0f || tex.y > 1.0f)
        {
            return (shadow / pixelCount);
        }
    }
    
    return (1.0f - (shadow / pixelCount));
}

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 lightColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 specularColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 normals = input.normal;
    float2 pTexCoord;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    textureColour = texture0.Sample(diffuseSampler, input.tex);
    
    //If working with the terrain and per pixel normal calculations are selected
    if(geometryType < 1.0f && renderType <= 1.0f)
    {
        normals = estimateNormalsByHeightMap(input.tex);
    }
    
    //If normals are set to render, don't calculate lighting, return normals for the current pixel
    if (renderType == 1.0f || renderType == 3.0f)
    {
        return float4(normals.xyz, 1.0f);
    }
    
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        if (lights[i].lightType.w > 0.0f)
        {
            lightColour += 0.0f;
            specularColour += 0.0f;
        }
        
        else
        {            
            // Calculate the projected texture coordinates.
            pTexCoord = getProjectiveCoords(input.lightViewPosition[i]);
            
            if (lights[i].lightType.y > 0.0f)
            {
                lightColour += calculatePoint(lights[i], input.worldPosition, normals);
            }
            
            if(lights[i].renderShadows == 1.0f) //Shadows are enabled
            {
                if (lights[i].softShadowsEnabled == 1.0f)   //Soft shadows are enabled
                {
                    float soften = softenShadowEdges(lights[i], input.lightViewPosition[i], depthMapTextures[i], pTexCoord, input.worldPosition);
                
                    if (lights[i].lightType.x > 0.0f)
                    {
                        lightColour += (soften * calculateDirectional(lights[i], normals));
                    }
                    else if (lights[i].lightType.z > 0.0f)
                    {
                        lightColour += (soften * calculateSpot(lights[i], input.worldPosition, normals));
                    }
                    
                    specularColour += (soften * calculateSpecular(lights[i], input.viewVector, normals));
                }
            
            // Shadow test. Is or isn't in shadow
                else if (hasDepthData(pTexCoord))   //Shadows are enabled, but soft shadows are not
                {
                // Has depth map data
                    if (!isInShadow(depthMapTextures[i], pTexCoord, input.lightViewPosition[i], lights[i].shadowBias))
                    {
                    // is NOT in shadow, therefore light               
                        if (lights[i].lightType.x > 0.0f)
                        {
                            lightColour += calculateDirectional(lights[i], normals);
                        }
                        else if (lights[i].lightType.z > 0.0f)
                        {
                            lightColour += calculateSpot(lights[i], input.worldPosition, normals);
                        }
                    
                        specularColour += calculateSpecular(lights[i], input.viewVector, normals);
                    }
                }
            }
            
            else    //Shadows are disabled, calculate light colour normally
            {
                if (lights[i].lightType.x > 0.0f)
                {
                    lightColour += calculateDirectional(lights[i], normals);
                }
                else if (lights[i].lightType.z > 0.0f)
                {
                    lightColour += calculateSpot(lights[i], input.worldPosition, normals);
                }
                
                specularColour += calculateSpecular(lights[i], input.viewVector, normals);
            }
        }
    }
    
    if (textureColour.a >= 0.9f)    //Only return the colour if the texture is not transparent
    {
        return specularColour + (saturate(lightColour) * textureColour);
    }
    
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}