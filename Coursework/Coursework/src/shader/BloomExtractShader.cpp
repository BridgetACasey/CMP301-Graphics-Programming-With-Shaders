#include "BloomExtractShader.h"

BloomExtractShader::BloomExtractShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"bloomExtract_vs.cso", L"bloomExtract_ps.cso");
}

BloomExtractShader::~BloomExtractShader()
{
}

void BloomExtractShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* colourTexture,
	float threshold, float intensity, float saturation, float sIntensity, float sSaturation)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	XMMATRIX tworld, tview, tproj;
	BloomExtractBufferType* bloomPtr;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);

	// Send matrix data
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//Send bloom extract data
	deviceContext->Map(bloomExtractBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	bloomPtr = (BloomExtractBufferType*)mappedResource.pData;
	bloomPtr->threshold = threshold;
	bloomPtr->saturation = saturation;
	bloomPtr->intensity = intensity;
	bloomPtr->sceneIntensity = sIntensity;
	bloomPtr->sceneSaturation = sSaturation;
	bloomPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(bloomExtractBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &bloomExtractBuffer);

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &colourTexture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void BloomExtractShader::initShader(const wchar_t* vs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC bloomExtractBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vs);
	loadPixelShader(ps);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	//Create the bloom extract buffer
	bloomExtractBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bloomExtractBufferDesc.ByteWidth = sizeof(BloomExtractBufferType);
	bloomExtractBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bloomExtractBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bloomExtractBufferDesc.MiscFlags = 0;
	bloomExtractBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&bloomExtractBufferDesc, NULL, &bloomExtractBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}