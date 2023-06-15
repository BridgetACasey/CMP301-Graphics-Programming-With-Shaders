#pragma once

#include "DXF.h"
#include "AppLight.h"

using namespace std;
using namespace DirectX;

class LightShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[4];
		XMMATRIX lightProjection[4];
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float time;
	};

	struct VertexManipulationBufferType
	{
		float amplitude;
		float renderType;
		float terrainResolution;
		float geometryType;
	};

	struct LightType
	{
		XMFLOAT4 ambient; //All
		XMFLOAT4 diffuse; //All
		XMFLOAT4 specular; //All
		XMFLOAT3 direction; //Directional and spot
		float range; //Point and spot
		XMFLOAT3 position;	//Point and spot
		float exponent; //Directional and spot
		XMFLOAT3 attenuation; //Point and spot
		float specularPower;
		XMFLOAT4 lightType;	//All
		float shadowBias;
		float nearPlane;
		float farPlane;
		float softShadowsEnabled;
		float softenRadius;
		float projectionType;
		float shadowsEnabled;
		float padding;
	};

	struct LightBufferType
	{
		LightType lights[4];
	};

public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightMap,
		float amplitude, AppLight* light[4], XMFLOAT3& cameraPos, float time, float renderType, float resolution, float geometryType, bool renderShadows);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* vertexManipulationBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
};