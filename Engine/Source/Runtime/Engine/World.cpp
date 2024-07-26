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
			.YFov = 60 * Pi() / 180.f,
			.ZFar = 0,
			.ZNear = 0.1f
		};
		_DefaultCamera = new CameraComponent("DefaultCamera", desc);
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
		.Intensity = 10,
		.Range = 0,
		.InnerConeAngle = 0,
		.OuterConeAngle = 0
	};

	LightComponent* sunLight = new LightComponent("SunLight", ldesc);
	Node* lNode = new Node("SunLightNode");
	sunLight->Attach(lNode);
	_Layers[0]->AddNode(lNode);
	sunLight->Registe();

	LightComponent::Desc lpdesc = {
		.Type = LightType::Point,
		.Color = float3(1,0,0),
		.Intensity = 10,
		.Range = 1000,
		.InnerConeAngle = 0,
		.OuterConeAngle = 0
	};

	LightComponent* pointLight = new LightComponent("PointLight", lpdesc);
	Node* lpNode = new Node("PointLightNode");
	pointLight->Attach(lpNode);
	_Layers[0]->AddNode(lpNode);
	pointLight->Registe();
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