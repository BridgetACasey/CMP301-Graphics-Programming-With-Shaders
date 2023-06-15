#pragma once

#include "Light.h"
#include "ShadowMap.h"

//An extension of the light class with additional functionality for point lights and spotlights
class AppLight : public Light
{
public:
	AppLight(XMFLOAT4 type);
	~AppLight();

	void generateShadowMap(ID3D11Device* renderDevice, int width, int height);

	inline void setLightType(XMFLOAT4 type) { lightType = type; }
	inline void setRange(float r) { range = r; }
	inline void setExponent(float exp) { exponent = exp; }
	inline void setAttenuation(XMFLOAT3 att) { attenuation = att; }
	inline void setShadowBias(float bias) { shadowBias = bias; }
	inline void setNearPlane(float nearP) { nearPlane = nearP; }
	inline void setFarPlane(float farP) { farPlane = farP; }
	inline void setSoftenShadows(float soften) { softenShadows = soften; }
	inline void setSoftenRadius(float radius) { softenRadius = radius; }
	inline void setProjectionMatrixType(float type) { projectionMatrixType = type; }

	inline XMFLOAT4 getLightType() const { return lightType; }
	inline float getRange() const { return range; }
	inline float getExponent() const { return exponent; }
	inline XMFLOAT3 getAttenuation() const { return attenuation; }
	inline float getShadowBias() const { return shadowBias; }
	inline float getNearPlane() const { return nearPlane; }
	inline float getFarPlane() const { return farPlane; }
	inline float getSoftenShadows() const { return softenShadows; }
	inline float getSoftenRadius() const { return softenRadius; }
	inline float getProjectionMatrixType() const { return projectionMatrixType; }

	inline ShadowMap* getShadowMap() const { return shadowMap; }

	inline XMVECTOR getLookAt() const { return lookAt; }

private:
	XMFLOAT4 lightType;	//Light type is determined by which value is set to 1.0f - x for directional, y for point, z for spot
	float range;
	float exponent;
	XMFLOAT3 attenuation;
	float shadowBias;
	float nearPlane;
	float farPlane;
	float softenShadows;
	float softenRadius;
	float projectionMatrixType;
	ShadowMap* shadowMap;
};