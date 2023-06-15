#include "Application.h"

Application::Application()
{
}

void Application::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	initShaders(hwnd);
	initLightingAndShadows();
	initMeshes(screenWidth, screenHeight);
	initTextures(screenWidth, screenHeight);
}

Application::~Application()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
}

bool Application::frame()
{
	bool result;

	result = BaseApplication::frame();

	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();

	if (!result)
	{
		return false;
	}

	return true;
}

//Initialise the shader manager objects
void Application::initShaders(HWND hwnd)
{
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	bloomExtractShader = new BloomExtractShader(renderer->getDevice(), hwnd);
	bloomCompositeShader = new BloomCompositeShader(renderer->getDevice(), hwnd);
	billboardingShader = new BillboardingShader(renderer->getDevice(), hwnd);
}

//Initialise the scene lights
void Application::initLightingAndShadows()
{
	int shadowMapWidth = 1024;
	int shadowMapHeight = 1024;

	// Configure a set of lights
	lights[0] = new AppLight(XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f));	//Directional light
	lights[0]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	lights[0]->setDiffuseColour(0.2f, 0.2f, 0.2f, 1.0f);
	lights[0]->setSpecularColour(0.1f, 0.1f, 0.1f, 1.0f);
	lights[0]->setDirection(0.0f, -1.0f, 1.0f);
	lights[0]->setPosition(0.0f, 15.0f, 0.0f);
	lights[0]->setShadowBias(0.002f);
	lights[0]->setSoftenShadows(1.0f);
	lights[0]->setProjectionMatrixType(0.0f);
	lights[0]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.0f);
	lights[0]->generateShadowMap(renderer->getDevice(), shadowMapWidth, shadowMapHeight);

	lights[1] = new AppLight(XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));	//Point light
	lights[1]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setDiffuseColour(0.15f, 0.4f, 0.35f, 1.0f);
	lights[1]->setSpecularColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setPosition(6.0f, 12.0f, 20.0f);
	lights[1]->setProjectionMatrixType(0.0f);
	lights[1]->setRange(10.0f);
	lights[1]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.0f);
	lights[1]->generateShadowMap(renderer->getDevice(), shadowMapWidth, shadowMapHeight);

	lights[2] = new AppLight(XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f));	//Spotlight
	lights[2]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2]->setDiffuseColour(0.6f, 0.2f, 0.2f, 1.0f);
	lights[2]->setSpecularColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2]->setPosition(6.0f, 14.0f, 2.0f);
	lights[2]->setProjectionMatrixType(1.0f);
	lights[2]->setExponent(1.0f);
	lights[2]->setRange(25.0f);
	lights[2]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.0f);
	lights[2]->generateShadowMap(renderer->getDevice(), shadowMapWidth, shadowMapHeight);

	lights[3] = new AppLight(XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f));	//Additional spotlight
	lights[3]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[3]->setDiffuseColour(0.2f, 0.6f, 0.2f, 1.0f);
	lights[3]->setSpecularColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[3]->setPosition(-10.0f, 12.0f, 3.0f);
	lights[3]->setProjectionMatrixType(1.0f);
	lights[3]->setExponent(1.0f);
	lights[3]->setRange(25.0f);
	lights[3]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.0f);
	lights[3]->generateShadowMap(renderer->getDevice(), shadowMapWidth, shadowMapHeight);
}

//Initialise the meshes
void Application::initMeshes(int screenWidth, int screenHeight)
{
	cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	planeMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	teapotModel = new AModel(renderer->getDevice(), "res/teapot.obj");
}

//Initialise textures and render texture objects
void Application::initTextures(int screenWidth, int screenHeight)
{
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");
	textureMgr->loadTexture(L"height", L"res/height.png");
	textureMgr->loadTexture(L"wood", L"res/wood.png");
	textureMgr->loadTexture(L"check", L"res/checkerboard.png");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"dirt", L"res/elliott-butler-screenshot01.jpg");
	
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomExtractTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomCompositeTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
}

bool Application::render()
{
	firstPass();

	if (renderShadows)
	{
		depthPass();
	}

	bloomExtract();

	horizontalBlur();
	verticalBlur();

	bloomComposite();

	finalPass();

	return true;
}

void Application::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("Scene Settings");
	ImGui::Spacing(); ImGui::Separator();

	guiGeneral();
	guiVertexManipulation();
	guiLighting();
	guiShadows();
	guiPostProcessing();
	guiBillboarding();

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Application::firstPass()
{
	// Set the render target to be the render to texture and clear it
	if (enableBloom)
	{
		sceneTexture->setRenderTarget(renderer->getDeviceContext());
		sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.35f, 0.35f, 0.35f, 1.0f);
	}

	else
	{
		renderer->beginScene(0.35f, 0.35f, 0.35f, 1.0f);
	}

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render floor
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	planeMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"dirt"), textureMgr->getTexture(L"height"),
		amplitude, lights, camera->getPosition(), timer->getTime(), renderType, 100.0f, 0.0f, renderShadows);
	lightShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// Render teapot model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.0f, 10.f, 5.0f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	teapotModel->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"wood"), textureMgr->getTexture(L"wood"),
		1.0f, lights, camera->getPosition(), timer->getTime(), renderType, teapotModel->getIndexCount(), 1.0f, renderShadows);
	lightShader->render(renderer->getDeviceContext(), teapotModel->getIndexCount());

	//Render cube
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(12.0f, 10.f, 3.0f);
	cubeMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"wood"), textureMgr->getTexture(L"wood"),
		1.0f, lights, camera->getPosition(), timer->getTime(), renderType, cubeMesh->getIndexCount(), 1.0f, renderShadows);
	lightShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	//Render sphere
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-12.0f, 10.f, 2.0f);
	sphereMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"wood"), textureMgr->getTexture(L"wood"),
		1.0f, lights, camera->getPosition(), timer->getTime(), renderType, sphereMesh->getIndexCount(), 1.0f, renderShadows);
	lightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	if (renderGrass)
	{
		//Render billboards
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(-50.f, 1.0f, -10.f);

		//The opposite translations must be applied to the camera to ensure it aligns with the plane mesh in the geometry shader
		XMFLOAT3 camPos = XMFLOAT3(camera->getPosition().x + 50.0f, camera->getPosition().y - 1.0f, camera->getPosition().z + 10.0f);

		if (!enableBloom)	//Avoids artefacting when used in conjunction with bloom
		{
			renderer->setAlphaBlending(true);
		}
		renderer->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		planeMesh->sendData(renderer->getDeviceContext());
		billboardingShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), camPos);
		billboardingShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());
		if (!enableBloom)
		{
			renderer->setAlphaBlending(false);
		}
	}

	if (renderGizmos)
	{
		renderLightingGizmos();
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void Application::finalPass()
{
	// Clear the scene
	if (enableBloom)
	{
		renderer->beginScene(0.35f, 0.35f, 0.35f, 1.0f);

		// RENDER THE RENDER TEXTURE SCENE
		// Requires 2D rendering and an ortho mesh.
		renderer->setZBuffer(false);
		//camera->update();
		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
		XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

		orthoMesh->sendData(renderer->getDeviceContext());

		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, bloomCompositeTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

		renderer->setZBuffer(true);
	}

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

void Application::bloomExtract()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	bloomExtractTexture->setRenderTarget(renderer->getDeviceContext());
	bloomExtractTexture->clearRenderTarget(renderer->getDeviceContext(), 0.35f, 0.35f, 0.35f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = bloomExtractTexture->getOrthoMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	bloomExtractShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView(),
		bloomThreshold, bloomIntensity, bloomSaturation, sceneIntensity, sceneSaturation);
	bloomExtractShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void Application::bloomComposite()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	bloomCompositeTexture->setRenderTarget(renderer->getDeviceContext());
	bloomCompositeTexture->clearRenderTarget(renderer->getDeviceContext(), 0.35f, 0.35f, 0.35f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = bloomExtractTexture->getOrthoMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	bloomCompositeShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView(),
		verticalBlurTexture->getShaderResourceView(), bloomThreshold, bloomIntensity, bloomSaturation, sceneIntensity, sceneSaturation);
	bloomCompositeShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void Application::horizontalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.35f, 0.35f, 0.35f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, bloomExtractTexture->getShaderResourceView(), screenSizeX, -25.0f, 25.0f, 15.0f);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void Application::verticalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.35f, 0.35f, 0.35f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY, -25.0f, 25.0f, 15.0f);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void Application::depthPass()
{
	// get the world, view, and projection matrices from the camera and d3d objects.
	for (int i = 0; i < 4; i++)
	{
		lights[i]->getShadowMap()->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

		lights[i]->generateViewMatrix();

		XMMATRIX lightViewMatrix = lights[i]->getViewMatrix();
		XMMATRIX lightProjectionMatrix;

		if (lights[i]->getProjectionMatrixType() == 0.0f)
		{
			lights[i]->generateOrthoMatrix(sceneWidth, sceneHeight, lights[i]->getNearPlane(), lights[i]->getFarPlane());
			lightProjectionMatrix = lights[i]->getOrthoMatrix();
		}

		else if(lights[i]->getProjectionMatrixType() == 1.0f)
		{
			lights[i]->generateProjectionMatrix(lights[i]->getNearPlane(), lights[i]->getFarPlane());
			lightProjectionMatrix = lights[i]->getProjectionMatrix();
		}

		XMMATRIX worldMatrix = renderer->getWorldMatrix();

		worldMatrix = XMMatrixTranslation(-50.0f, 0.0f, -10.0f);

		// Set the render target to be the render to texture.
		// Render floor
		planeMesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"height"),
			lights[i]->getNearPlane(), lights[i]->getFarPlane(), amplitude, 0.0f);
		depthShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

		// Render model
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(0.0f, 7.0f, 5.0f);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		teapotModel->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"height"),
			lights[i]->getNearPlane(), lights[i]->getFarPlane(), amplitude, 1.0f);
		depthShader->render(renderer->getDeviceContext(), teapotModel->getIndexCount());

		//Render cube
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(12.0f, 10.f, 3.0f);
		cubeMesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"height"),
			lights[i]->getNearPlane(), lights[i]->getFarPlane(), amplitude, 1.0f);
		depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

		//Render sphere
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(-12.0f, 10.f, 2.0f);
		sphereMesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"height"),
			lights[i]->getNearPlane(), lights[i]->getFarPlane(), amplitude, 1.0f);
		depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

		// Set back buffer as render target and reset view port.
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}
}

void Application::renderLightingGizmos()
{
	for (AppLight* light : lights)
	{
		if (light->getLightType().w == 0.0f)	//Check the light hasn't been disabled
		{
			camera->update();
			XMMATRIX worldMatrix = renderer->getWorldMatrix();
			XMMATRIX viewMatrix = camera->getViewMatrix();
			XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

			//Translate to the light's position
			worldMatrix *= XMMatrixTranslation(light->getPosition().x, light->getPosition().y, light->getPosition().z);

			sphereMesh->sendData(renderer->getDeviceContext());
			textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"check"));
			textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
		}
	}
}

void Application::guiGeneral()
{
	if (ImGui::CollapsingHeader("General", 0))
	{
		static float cameraSpeed = 3.0f;

		ImGui::Text("FPS: %.2f", timer->getFPS());
		ImGui::Text("Camera Position: (x) %.3f (y) %.3f (z) %.3f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
		ImGui::DragFloat("Camera Speed", &cameraSpeed, 0.1f, 0.1f, 15.0f);
		ImGui::Checkbox("Toggle Wireframe", &wireframeToggle);
		ImGui::Separator();
		ImGui::Spacing();

		camera->move(timer->getTime() * cameraSpeed);
	}
}

void Application::guiLighting()
{
	if (ImGui::CollapsingHeader("Lighting", 0))
	{
		static bool toggleGizmos = renderGizmos;
		ImGui::Checkbox("Render Lighting Gizmos", &toggleGizmos);
		renderGizmos = toggleGizmos;

		int index = 1;

		for (AppLight* light : lights)
		{
			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			//Set current light title
			std::string title = "Light " + std::to_string(index);
			ImGui::Text(title.c_str());

			bool toggleActive = (bool)light->getLightType().w;

			std::string activeLabel = "(" + std::to_string(index) + ") Toggle Active";
			ImGui::Checkbox(activeLabel.c_str(), &toggleActive);

			light->setLightType(XMFLOAT4(light->getLightType().x, light->getLightType().y, light->getLightType().z, (float)toggleActive));

			if (light->getLightType().w <= 0.0f)
			{
				float type[3] = { 0.0f, 0.0f, 0.0f };
				float ambient[4] = { light->getAmbientColour().x, light->getAmbientColour().y, light->getAmbientColour().z, light->getAmbientColour().w };
				float diffuse[4] = { light->getDiffuseColour().x, light->getDiffuseColour().y, light->getDiffuseColour().z, light->getDiffuseColour().w };
				float specular[4] = { light->getSpecularColour().x, light->getSpecularColour().y, light->getSpecularColour().z, light->getSpecularColour().w };
				float specularPower = light->getSpecularPower();
				float position[3] = { light->getPosition().x, light->getPosition().y, light->getPosition().z };

				//Select light type and update if a radio button has been pressed
				ImGui::Text("Type");
				int selectDirectional = light->getLightType().x;
				int selectPoint = light->getLightType().y;
				int selectSpot = light->getLightType().z;
				std::string directionalLabel = "(" + std::to_string(index) + ") Directional";
				type[0] = (float)ImGui::RadioButton(directionalLabel.c_str(), &selectDirectional, 1);	ImGui::SameLine();
				std::string pointLabel = "(" + std::to_string(index) + ") Point";
				type[1] = (float)ImGui::RadioButton(pointLabel.c_str(), &selectPoint, 1);		ImGui::SameLine();
				std::string spotLabel = "(" + std::to_string(index) + ") Spot";
				type[2] = (float)ImGui::RadioButton(spotLabel.c_str(), &selectSpot, 1);

				if (type[0] || type[1] || type[2])
				{
					light->setLightType(XMFLOAT4(type[0], type[1], type[2], 0.0f));
				}

				//Update general attributes for all light types
				std::string ambientLabel = "(" + std::to_string(index) + ") Ambient";
				ImGui::DragFloat4(ambientLabel.c_str(), ambient, 0.05f, 0.0f, 1.0f);
				std::string diffuseLabel = "(" + std::to_string(index) + ") Diffuse";
				ImGui::DragFloat4(diffuseLabel.c_str(), diffuse, 0.05f, 0.0f, 1.0f);
				std::string specularLabel = "(" + std::to_string(index) + ") Specular";
				ImGui::DragFloat4(specularLabel.c_str(), specular, 0.05f, 0.0f, 1.0f);
				std::string specPowerLabel = "(" + std::to_string(index) + ") Spec. Power";
				ImGui::DragFloat(specPowerLabel.c_str(), &specularPower, 0.01f, 0.01f, 10.0f);
				std::string positionLabel = "(" + std::to_string(index) + ") Position";
				ImGui::DragFloat3(positionLabel.c_str(), position, 0.1f, -100.0f, 100.0f);

				light->setAmbientColour(ambient[0], ambient[1], ambient[2], ambient[3]);
				light->setDiffuseColour(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
				light->setSpecularColour(specular[0], specular[1], specular[2], specular[3]);
				light->setSpecularPower(specularPower);
				light->setPosition(position[0], position[1], position[2]);

				ImGui::Spacing();

				//Update directional light settings
				if (light->getLightType().x > 0.0f)
				{
					float direction[3] = { light->getDirection().x, light->getDirection().y, light->getDirection().z };

					std::string directionLabel = "(" + std::to_string(index) + ") Direction";
					ImGui::DragFloat3(directionLabel.c_str(), direction, 0.01f, -1.0f, 1.0f);

					light->setDirection(direction[0], direction[1], direction[2]);
				}

				//Update point light settings
				else if (light->getLightType().y > 0.0f)
				{
					float attenuation[3] = { light->getAttenuation().x, light->getAttenuation().y, light->getAttenuation().z };

					std::string attenuationLabel = "(" + std::to_string(index) + ") Attenuation";
					ImGui::DragFloat3(attenuationLabel.c_str(), attenuation, 0.001f, 0.0f, 1.0f);

					light->setAttenuation(XMFLOAT3(attenuation[0], attenuation[1], attenuation[2]));
				}

				//Update spotlight settings
				else if (light->getLightType().z > 0.0f)
				{
					float direction[3] = { light->getDirection().x, light->getDirection().y, light->getDirection().z };
					float attenuation[3] = { light->getAttenuation().x, light->getAttenuation().y, light->getAttenuation().z };
					float exponent = light->getExponent();
					float range = light->getRange();

					std::string directionLabel = "(" + std::to_string(index) + ") Direction";
					ImGui::DragFloat3(directionLabel.c_str(), direction, 0.01f, -1.0f, 1.0f);
					std::string attenuationLabel = "(" + std::to_string(index) + ") Attenuation";
					ImGui::DragFloat3(attenuationLabel.c_str(), attenuation, 0.001f, 0.0f, 1.0f);
					std::string exponentLabel = "(" + std::to_string(index) + ") Exponent";
					ImGui::DragFloat(exponentLabel.c_str(), &exponent, 0.01f, 0.1f, 10.0f);
					std::string rangeLabel = "(" + std::to_string(index) + ") Range";
					ImGui::DragFloat(rangeLabel.c_str(), &range, 0.1f, 1.0f, 100.0f);

					light->setDirection(direction[0], direction[1], direction[2]);
					light->setAttenuation(XMFLOAT3(attenuation[0], attenuation[1], attenuation[2]));
					light->setExponent(exponent);
					light->setRange(range);
				}
			}

			//Increment light index
			index++;
		}
	}
}

void Application::guiShadows()
{
	if (ImGui::CollapsingHeader("Shadows", 0))
	{
		static bool shadow = renderShadows;
		ImGui::Checkbox("Toggle Shadows", &shadow);
		renderShadows = shadow;

		int index = 1;
		const char* matrixLabels[] = { "orthographic", "perspective" };
		static const char* currentLabel[4] =
		{
			matrixLabels[(int)lights[0]->getProjectionMatrixType()],
			matrixLabels[(int)lights[1]->getProjectionMatrixType()],
			matrixLabels[(int)lights[2]->getProjectionMatrixType()],
			matrixLabels[(int)lights[3]->getProjectionMatrixType()]
		};

		for (AppLight* light : lights)
		{
			ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

			//Set current light title
			std::string title = "Light " + std::to_string(index) + "'s Shadow";
			ImGui::Text(title.c_str());

			float bias = light->getShadowBias();
			float nearP = light->getNearPlane();
			float farP = light->getFarPlane();
			bool soften = light->getSoftenShadows();
			int radius = light->getSoftenRadius();

			std::string matrixTextLabel = "Projection Matrix (" + std::to_string(index) + ")";
			if (ImGui::BeginCombo(matrixTextLabel.c_str(), currentLabel[index - 1]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(matrixLabels); i++)
				{
					bool selected = (currentLabel[index - 1] == matrixLabels[i]);

					if (ImGui::Selectable(matrixLabels[i], selected))
					{
						currentLabel[index - 1] = matrixLabels[i];

						ImGui::SetItemDefaultFocus();

						light->setProjectionMatrixType((float)i);
					}
				}

				ImGui::EndCombo();
			}

			std::string biasLabel = "(" + std::to_string(index) + ") Shadow Bias";
			ImGui::DragFloat(biasLabel.c_str(), &bias, 0.0001f, 0.0001f, 0.05f);
			std::string nearLabel = "(" + std::to_string(index) + ") Near Plane";
			ImGui::DragFloat(nearLabel.c_str(), &nearP, 0.1f, 0.1f, (farP - 0.1f));
			std::string farLabel = "(" + std::to_string(index) + ") Far Plane";
			ImGui::DragFloat(farLabel.c_str(), &farP, 0.1f, (nearP + 0.1f), 100.0f);
			ImGui::Spacing();
			std::string softLabel = "(" + std::to_string(index) + ") Soften Shadows (PCF)";
			ImGui::Checkbox(softLabel.c_str(), &soften);
			std::string radiusLabel = "(" + std::to_string(index) + ") Softening Radius";
			ImGui::DragInt(radiusLabel.c_str(), &radius, 1, 1, 25);

			//To prevent exceptions errors - the near plane must always be less than the far plane
			if (nearP >= farP)
			{
				nearP -= 0.1f;
			}

			if (farP <= nearP)
			{
				farP += 0.1f;
			}

			light->setShadowBias(bias);
			light->setNearPlane(nearP);
			light->setFarPlane(farP);
			light->setSoftenShadows(soften);
			light->setSoftenRadius(radius);

			index++;
		}
	}
}

void Application::guiPostProcessing()
{
	if (ImGui::CollapsingHeader("Post Processing", 0))
	{
		static bool bloom = enableBloom;
		static float threshold = bloomThreshold;
		static float intensity = bloomIntensity;
		static float saturation = bloomSaturation;
		static float sIntensity = sceneIntensity;
		static float sSaturation = sceneSaturation;

		ImGui::Checkbox("Toggle Bloom", &bloom);
		ImGui::DragFloat("Bloom Threshold", &threshold, 0.001f, 0.001f, 1.0f);
		ImGui::DragFloat("Bloom Intensity", &intensity, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Bloom Saturation", &saturation, 0.01f, 0.01f, 5.0f);
		ImGui::DragFloat("Scene Intensity", &sIntensity, 0.01f, 0.01f, 5.0f);
		ImGui::DragFloat("Scene Saturation", &sSaturation, 0.01f, 0.01f, 5.0f);

		enableBloom = bloom;
		bloomThreshold = threshold;
		bloomIntensity = intensity;
		bloomSaturation = saturation;
		sceneIntensity = sIntensity;
		sceneSaturation = sSaturation;
	}
}

void Application::guiVertexManipulation()
{
	if (ImGui::CollapsingHeader("Vertex Manipulation", 0))
	{
		static float ampl = amplitude;
		const char* renderLabels[] = { "textures (per pixel)", "normals (per pixel)", "textures (per vertex)", "normals (per vertex)" };
		static const char* currentLabel = renderLabels[0];

		ImGui::DragFloat("Amplitude", &ampl, 0.1f, 0.1f, 50.0f);

		if (ImGui::BeginCombo("Render Type", currentLabel))
		{
			for (int i = 0; i < IM_ARRAYSIZE(renderLabels); i++)
			{
				bool selected = (currentLabel == renderLabels[i]);
				
				if (ImGui::Selectable(renderLabels[i], selected))
				{
					currentLabel = renderLabels[i];

					ImGui::SetItemDefaultFocus();
		
					renderType = (float)i;
				}
			}
		
			ImGui::EndCombo();
		}

		amplitude = ampl;
	}
}

void Application::guiBillboarding()
{
	if (ImGui::CollapsingHeader("Billboarding", 0))
	{
		static bool grass = renderGrass;

		ImGui::Checkbox("Toggle Grass", &grass);

		renderGrass = grass;
	}
}