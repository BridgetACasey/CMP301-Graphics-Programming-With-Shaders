#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class BillboardingShader : public BaseShader
{
private:
	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

public:
	BillboardingShader(ID3D11Device* device, HWND hwnd);
	~BillboardingShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT3 camPos);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11SamplerState* sampleState;
};