// Simple shader example.

#pragma once

#include "DXF.h"
#include "AppLight.h"

using namespace std;
using namespace DirectX;

class DepthShader : public BaseShader
{
private:
	struct DepthBufferType
	{
		float nearPlane;
		float farPlane;
		XMFLOAT2 padding;
	};

	struct GeometryBufferType
	{
		float amplitude;
		float geometryType;
		XMFLOAT2 padding;
	};

public:
	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMap, float nearP, float farP, float ampl, float geoType);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* depthBuffer;
	ID3D11Buffer* geometryBuffer;
};