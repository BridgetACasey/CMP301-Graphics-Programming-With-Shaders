#include "DepthShader.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depth_vs.cso", L"depth_ps.cso");
}

DepthShader::~DepthShader()
{
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

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC depthBufferDesc;
	D3D11_BUFFER_DESC geometryBufferDesc;

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

	depthBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	depthBufferDesc.ByteWidth = sizeof(DepthBufferType);
	depthBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	depthBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&depthBufferDesc, NULL, &depthBuffer);

	geometryBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	geometryBufferDesc.ByteWidth = sizeof(GeometryBufferType);
	geometryBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	geometryBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	geometryBufferDesc.MiscFlags = 0;
	geometryBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&geometryBufferDesc, NULL, &geometryBuffer);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMap, float nearP, float farP, float ampl, float geoType)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	DepthBufferType* depthPtr;
	GeometryBufferType* geoPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld, tview, tproj;

	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->VSSetShaderResources(0, 1, &heightMap);

	deviceContext->Map(geometryBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	geoPtr = (GeometryBufferType*)mappedResource.pData;
	geoPtr->amplitude = ampl;
	geoPtr->geometryType = geoType;
	geoPtr->padding = XMFLOAT2(0.0f, 0.0f);
	deviceContext->Unmap(geometryBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &geometryBuffer);

	deviceContext->Map(depthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	depthPtr = (DepthBufferType*)mappedResource.pData;
	depthPtr->nearPlane = nearP;
	depthPtr->farPlane = farP;
	depthPtr->padding = XMFLOAT2(0.0f, 0.0f);
	deviceContext->Unmap(depthBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &depthBuffer);
}