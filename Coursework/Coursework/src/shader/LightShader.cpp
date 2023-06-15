#include "LightShader.h"

LightShader::LightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"light_vs.cso", L"light_ps.cso");
}

LightShader::~LightShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void LightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC vertexManipulationBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	vertexManipulationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexManipulationBufferDesc.ByteWidth = sizeof(VertexManipulationBufferType);
	vertexManipulationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexManipulationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexManipulationBufferDesc.MiscFlags = 0;
	vertexManipulationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&vertexManipulationBufferDesc, NULL, &vertexManipulationBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
}

void LightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture,
	ID3D11ShaderResourceView* heightMap, float amplitude, AppLight* light[4], XMFLOAT3& cameraPos, float time, float renderType, float resolution, float geometryType, bool renderShadows)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	CameraBufferType* cameraPtr;
	LightBufferType* lightPtr;
	VertexManipulationBufferType* vertexPtr;
	
	XMMATRIX tworld, tview, tproj, tLightViewMatrix[4], tLightProjectionMatrix[4];

	ID3D11ShaderResourceView* depthMaps[4];

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	
	for (int i = 0; i < 4; i++)
	{
		tLightViewMatrix[i] = XMMatrixTranspose(light[i]->getViewMatrix());

		if (light[i]->getProjectionMatrixType() == 0.0f)
		{
			tLightProjectionMatrix[i] = XMMatrixTranspose(light[i]->getOrthoMatrix());
		}

		else
		{
			tLightProjectionMatrix[i] = XMMatrixTranspose(light[i]->getProjectionMatrix());
		}
	}

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	for (int i = 0; i < 4; i++)
	{
		dataPtr->lightView[i] = tLightViewMatrix[i];
		dataPtr->lightProjection[i] = tLightProjectionMatrix[i];
	}

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = cameraPos;
	cameraPtr->time = time;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	deviceContext->Map(vertexManipulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	vertexPtr = (VertexManipulationBufferType*)mappedResource.pData;
	vertexPtr->amplitude = amplitude;
	vertexPtr->renderType = renderType;
	vertexPtr->terrainResolution = resolution;
	vertexPtr->geometryType = geometryType;
	deviceContext->Unmap(vertexManipulationBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &vertexManipulationBuffer);

	deviceContext->VSSetShaderResources(0, 1, &heightMap);
	deviceContext->VSSetSamplers(0, 1, &sampleState);

	//Additional
	// Send light data to pixel shader
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	
	for (int i = 0; i < 4; i++)
	{
		lightPtr->lights[i].ambient = light[i]->getAmbientColour();
		lightPtr->lights[i].diffuse = light[i]->getDiffuseColour();
		lightPtr->lights[i].specular = light[i]->getSpecularColour();
		lightPtr->lights[i].direction = light[i]->getDirection();
		lightPtr->lights[i].range = light[i]->getRange();
		lightPtr->lights[i].position = light[i]->getPosition();
		lightPtr->lights[i].exponent = light[i]->getExponent();
		lightPtr->lights[i].attenuation = light[i]->getAttenuation();
		lightPtr->lights[i].specularPower = light[i]->getSpecularPower();
		lightPtr->lights[i].lightType = light[i]->getLightType();
		lightPtr->lights[i].shadowBias = light[i]->getShadowBias();
		lightPtr->lights[i].softShadowsEnabled = (float)light[i]->getSoftenShadows();
		lightPtr->lights[i].softenRadius = (float)light[i]->getSoftenRadius();
		lightPtr->lights[i].shadowsEnabled = (float)renderShadows;
		lightPtr->lights[i].padding = 0.0f;

		depthMaps[i] = light[i]->getShadowMap()->getDepthMapSRV();
	}

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &vertexManipulationBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 4, depthMaps);
	deviceContext->PSSetShaderResources(5, 1, &heightMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}