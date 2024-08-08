#include "Private.h"

GameWorld& GameWorld::Instance()
{
	static GameWorld inst;
	return inst;
}

void GameWorld::AddLayer(Layer* layer)
{
	layer->Registe();
	_Layers.push_back(layer);
}

void GameWorld::SetupWorld()
{
	// camera.
	for (auto n : _Layers)
	{
		Component* c = n->FindFirstComponent(ComponentType::Camera);
		if (c)
		{
			_DefaultCamera = static_cast<CameraComponent*>(c);
			break;
		}
	}
	if (!_DefaultCamera)
	{
		auto config = GameEngine::Instance().GetRenderConfig();
		CameraComponent::Desc desc = {
			.Type = CameraType::Perspective,
			.AspectRatio = config.AspectRatio,
			.YFov = Math::Pi() / 4,
			.ZFar = 1000,
			.ZNear = 0.1f
		};
		_DefaultCamera = new CameraComponent("DefaultCamera",desc);
		_DefaultCamera->LookAt(float3(0, 0, 4), float3(0, 0, 0), float3(0,1,0));
		Node* node = new Node("DefaultCameraNode");
		_DefaultCamera->Attach(node);
		_Layers[0]->AddNode(node);
	}

	_CameraController = new CameraController;
	_CameraController->Attach(_DefaultCamera);

	// light.
	LightComponent::Desc ldesc = {
		.Type = LightType::Directional,
		.Color = float3(1,1,1),
		.Intensity = 50,
		.Range = 0,
		.InnerConeAngle = 0,
		.OuterConeAngle = 0
	};

	LightComponent* sunLight = new LightComponent("SunLight", ldesc);
	Node* lNode = new Node("SunLightNode");
	lNode->SetRotation(quaternion::rotation_euler_zxy(float3(Math::Deg2Rad() * 120, Math::Deg2Rad() * 30, Math::Deg2Rad() * 30)));
	sunLight->Attach(lNode);
	_Layers[0]->AddNode(lNode);
	sunLight->Registe();

	LightComponent::Desc lpdesc = {
		.Type = LightType::Point,
		.Color = float3(1,1,0.5),
		.Intensity = 20,
		.Range = 100,
		.InnerConeAngle = 0,
		.OuterConeAngle = 0
	};

	LightComponent* pointLight = new LightComponent("PointLight", lpdesc);
	Node* lpNode = new Node("PointLightNode");
	lpNode->SetTranslate(float3(0, 5, 0));
	pointLight->Attach(lpNode);
	_Layers[0]->AddNode(lpNode);
	pointLight->Registe();

	//sky
	SkyComponent::Desc sDesc;
	SkyComponent* skyComponent = new SkyComponent("SkyDome", sDesc);
	skyComponent->SetDirectionalLight(sunLight);
	Node* sNode = new Node("SkyNode");
	skyComponent->Attach(sNode);
	_Layers[0]->AddNode(sNode);
	skyComponent->Registe();
	_DefaultSkyComponent = skyComponent;

}

void GameWorld::Init(const std::string& scenefile)
{
	_RenderScene = GameEngine::Instance().GetRenderer().AllocateScene(this);

	std::vector<Layer*> newLayers;
	glTFLoader::Instance().Load(scenefile,newLayers);
	for (auto l : newLayers)
	{
		AddLayer(l);
	}

	SetupWorld();
	_RenderScene->BuildPrimitiveCache();
}

void GameWorld::Destroy()
{	
	delete _CameraController;

	for (auto l : _Layers)
	{
		delete l;
	}
	_Layers.clear();

	GameEngine::Instance().GetRenderer().RemoveScene(_RenderScene);
	_RenderScene = nullptr;
}

void GameWorld::Update(double delta)
{
	for (auto l : _Layers)
		l->Update(delta);

	_CameraController->Update(delta);
}