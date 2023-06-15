#pragma once

#include "DXF.h"
#include "BaseShader.h"

using namespace DirectX;

class BloomCompositeShader : public BaseShader
{
private:
	struct BloomCompositeBufferType
	{
		float threshold;
		float intensity;
		float saturation;
		float sceneIntensity;
		float sceneSaturation;
		XMFLOAT3 padding;
	};

public:
	BloomCompositeShader(ID3D11Device* device, HWND hwnd);
	~BloomCompositeShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* colourTexture, ID3D11ShaderResourceView* bloomTexture,
		float threshold, float intensity, float saturation, float sIntensity, float sSaturation);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* bloomCompositeBuffer;
	ID3D11SamplerState* sampleState;
};