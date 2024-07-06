#include "Private.h"

GameWorld& GameWorld::Instance()
{
	static GameWorld inst;
	return inst;
}

void GameWorld::AddLayer(Layer* layer)
{
	layer->RegisteToScene();
	_Layers.push_back(layer);
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

	// find first camera.
	for (auto n : _Layers)
	{
		Component* c = n->FindFirstComponent(ComponentType::Camera);
		if (c)
		{
			_DefaultCamera = static_cast<Camera*>(c);
			break;
		}
	}
	if (!_DefaultCamera)
	{
		auto config = GameEngine::Instance().GetRenderConfig();
		Camera::Desc desc = {
			.Type = CameraType::Perspective,
			.AspectRatio = config.FrameWidth * 1.f / config.FrameHeight,
			.YFov = 60 * Pi() / 180.f,
			.ZFar = 0,
			.ZNear = 1.f
		};
		_DefaultCamera = new Camera("DefaultCamera", desc);
		Node* node = new Node("DefaultCameraNode");
		_DefaultCamera->Attach(node);
		_Layers[0]->AddNode(node);
	}

	_CameraController = new CameraController;
	_CameraController->Attach(_DefaultCamera);
}

void GameWorld::Destroy()
{	
	GameEngine::Instance().GetRenderer().RemoveScene(_RenderScene);

	delete _CameraController;

	for (auto l : _Layers)
	{
		delete l;
	}
	_Layers.clear();

}

void GameWorld::Update(double delta)
{
	for (auto l : _Layers)
		l->Update(delta);

	_CameraController->Update(delta);
}