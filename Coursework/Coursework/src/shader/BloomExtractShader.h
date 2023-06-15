#pragma once

#include "DXF.h"
#include "BaseShader.h"

using namespace DirectX;

class BloomExtractShader : public BaseShader
{
private:
	struct BloomExtractBufferType
	{
		float threshold;
		float intensity;
		float saturation;
		float sceneIntensity;
		float sceneSaturation;
		XMFLOAT3 padding;
	};

public:
	BloomExtractShader(ID3D11Device* device, HWND hwnd);
	~BloomExtractShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* colourTexture,
	float threshold, float intensity, float saturation, float sIntensity, float sSaturation);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* bloomExtractBuffer;
	ID3D11SamplerState* sampleState;
};