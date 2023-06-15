// Application.h
#ifndef _APPLICATION_H
#define _APPLICATION_H

// Includes
#include "DXF.h"
#include "shader/LightShader.h"
#include "shader/TextureShader.h"
#include "shader/VerticalBlurShader.h"
#include "shader/HorizontalBlurShader.h"
#include "shader/DepthShader.h"
#include "shader/BloomExtractShader.h"
#include "shader/BloomCompositeShader.h"
#include "shader/BillboardingShader.h"

class Application : public BaseApplication
{
public:
	Application();
	~Application();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

	void firstPass();
	void finalPass();

	void bloomExtract();
	void bloomComposite();

	void horizontalBlur();
	void verticalBlur();

	void depthPass();

private:
	void initShaders(HWND hwnd);
	void initLightingAndShadows();
	void initMeshes(int screenWidth, int screenHeight);
	void initTextures(int screenWidth, int screenHeight);
	
	void renderLightingGizmos();

	void guiGeneral();
	void guiLighting();
	void guiShadows();
	void guiPostProcessing();
	void guiVertexManipulation();
	void guiBillboarding();

	bool renderShadows = true;
	bool renderGizmos = false;
	bool renderGrass = true;

	CubeMesh* cubeMesh = nullptr;
	OrthoMesh* orthoMesh = nullptr;
	SphereMesh* sphereMesh = nullptr;
	PlaneMesh* planeMesh = nullptr;
	AModel* teapotModel = nullptr;

	RenderTexture* sceneTexture = nullptr;
	RenderTexture* bloomExtractTexture = nullptr;
	RenderTexture* bloomCompositeTexture = nullptr;
	RenderTexture* horizontalBlurTexture = nullptr;
	RenderTexture* verticalBlurTexture = nullptr;

	AppLight* lights[4] = {nullptr};

	LightShader* lightShader = nullptr;
	TextureShader* textureShader = nullptr;
	VerticalBlurShader* verticalBlurShader = nullptr;
	HorizontalBlurShader* horizontalBlurShader = nullptr;
	DepthShader* depthShader = nullptr;
	BloomExtractShader* bloomExtractShader = nullptr;
	BloomCompositeShader* bloomCompositeShader = nullptr;
	BillboardingShader* billboardingShader = nullptr;

	float sceneWidth = 100.0f;
	float sceneHeight = 100.0f;

	bool enableBloom = false;
	float bloomThreshold = 0.4f;
	float bloomIntensity = 1.25f;
	float bloomSaturation = 1.0f;
	float sceneIntensity = 1.0f;
	float sceneSaturation = 1.0f;

	float amplitude = 10.0f;
	float renderType = 0.0f;
};

#endif