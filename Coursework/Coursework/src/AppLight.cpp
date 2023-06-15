#include "AppLight.h"

AppLight::AppLight(XMFLOAT4 type)
{
	lightType = type;

	setPosition(1.0f, 1.0f, 1.0f);
	setDirection(0.1f, -1.0f, 0.1f);
	setSpecularPower(1.0f);

	range = 1.0f;
	exponent = 1.0f;
	attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);

	shadowBias = 0.003f;
	nearPlane = 0.1f;
	farPlane = 100.0f;
	softenShadows = 0.0f;
	softenRadius = 3.0f;

	shadowMap = nullptr;
}

AppLight::~AppLight()
{

}

void AppLight::generateShadowMap(ID3D11Device* renderDevice, int width, int height)
{
	shadowMap = new ShadowMap(renderDevice, width, height);
}