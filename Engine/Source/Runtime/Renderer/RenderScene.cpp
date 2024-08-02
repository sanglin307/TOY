#include "Private.h"

RenderCluster::~RenderCluster()
{
	for (auto v : VertexStreams)
	{
		delete v.Buffer;
	}

	if (IndexBuffer)
		delete IndexBuffer;

	if (Primitive)
		delete Primitive;

}

RenderLight::~RenderLight()
{
	if (Light)
		delete Light;

}

RenderScene::RenderScene(GameWorld* world,SceneRenderer* renderer)
{
	_World = world;
	_Renderer = renderer;
	renderer->BindScene(this);
}

RenderScene::~RenderScene()
{
	for (RenderCluster* p : _Clusters)
	{
		delete p;
	}

	for (RenderLight* l : _Lights)
	{
		delete l;
	}

	if (_Sky.SkyData)
	{
		delete _Sky.SkyData;
	}
}
void RenderScene::AddSky(SkyComponent* sky)
{
	if (_Sky.Component != sky)
	{
		_Sky.Component = sky;
		if (!_Sky.SkyData)
		{
			_Sky.SkyData = new ProceduralSky;
		}
		LightComponent* l = sky->GetDirectionalLight();
		check(l);
		float4x4& lightMat = l->GetOwner()->GetWorldMatrix();
		SkyComponent::Desc desc = sky->GetDesc();

		float4 direction = mul(float4(0,0,1,0), lightMat);
		_Sky.SkyData->SunDirection = direction.xyz;
		_Sky.SkyData->Rayleigh = desc.Rayleigh;
		_Sky.SkyData->Turbidity = desc.Turbidity;
		_Sky.SkyData->MieCoefficient = desc.MieCoefficient;
		_Sky.SkyData->Luminance = desc.Luminance;
		_Sky.SkyData->MieDirectionalG = desc.MieDirectionalG;
	}
}

void RenderScene::RemoveSky(SkyComponent* sky)
{
	if (_Sky.Component == sky)
	{
		_Sky.Component = nullptr;
		delete _Sky.SkyData;
		_Sky.SkyData = nullptr;
	}
}

void RenderScene::AddLight(const Transform& trans,LightComponent* light)
{
	auto iter = std::find_if(_Lights.begin(), _Lights.end(), [light](RenderLight* l) -> bool {
		if (l->Component == light)
			return true;
		return false;
		});

	if (iter != _Lights.end())
		return;

	auto desc = light->GetDesc();
	RenderLight* l = new RenderLight;
	l->Component = light;
	LightData* ld = new LightData;
	ld->Color = desc.Color;
	ld->Direction = mul(Coordinate::Forward(), trans.Rotation);
	ld->Position = trans.Translate;
	ld->Range = desc.Range;
	ld->SpotlightAngles = float2(desc.InnerConeAngle, desc.OuterConeAngle);
	ld->Intensity = desc.Intensity;
	if (desc.Type == LightType::Directional)
		ld->Type = LightType_Directional;
	else if (desc.Type == LightType::Point)
		ld->Type = LightType_Point;
	else if (desc.Type == LightType::Spot)
		ld->Type = LightType_Spot;
	else
		check(0);

	l->Light = ld;

	_Lights.push_back(l);
	_LightDataBuffer.push_back(*l->Light);

	_Renderer->UpdateLightBuffer(_LightDataBuffer);

}

void RenderScene::RemoveLight(LightComponent* light)
{
	auto iter = std::find_if(_Lights.begin(), _Lights.end(), [light](RenderLight* l) -> bool {
		if (l->Component == light)
			return true;
		return false;
		});

	delete* iter;
	u32 pos = (u32)std::distance(_Lights.begin(), iter);
	auto liter = _LightDataBuffer.begin();
	std::advance(liter, pos);
	_LightDataBuffer.erase(liter);
	_Lights.erase(iter);

	_Renderer->UpdateLightBuffer(_LightDataBuffer);
}

void RenderScene::AddPrimitive(const Transform& trans, PrimitiveComponent* primitive)
{
	auto iter = std::find_if(_Clusters.begin(), _Clusters.end(), [primitive](const RenderCluster* info) -> bool {
		return primitive == info->Component;
		});

	check(iter == _Clusters.end());

	RenderDevice* device = _Renderer->GetDevice();
	if (primitive->GetType() == PrimitiveType::Mesh)
	{
		MeshComponent* mesh = static_cast<MeshComponent*>(primitive);
		const std::vector<MeshSegment*>& segments = mesh->GetSegments();
		for (MeshSegment* s : segments)
		{
			RenderCluster* cluster = new RenderCluster;
			cluster->Component = primitive;
			cluster->Material = s->GetMaterial();
			const std::array<VertexData, (u32)VertexAttribute::Max>& vertexData = s->GetVertexData();
			for (u32 i = 0; i < vertexData.size(); i++)
			{
				const VertexData& d = vertexData[i];
				if (!d.Size)
					continue;

				RenderBuffer::Desc desc = {
					 .Size = d.Size,
					 .Stride = d.GetStride(),
					 .Usage = (u32)ResourceUsage::VertexBuffer,
					 .CpuAccess = CpuAccessFlags::None,
					 .Alignment = true,
					 .InitData = d.Data
				};

				// TODO , use seperate buffer first.
				cluster->VertexStreams.push_back(RenderCluster::Stream{
					.Attribute = (VertexAttribute)i,
					.Buffer = device->CreateBuffer(std::format("VertexBuffer_{}_{}",(u64)(primitive),i),desc),
					.ByteOffset = 0
					});
			}

			const VertexData& indexData = s->GetIndexData();
			if (indexData.Size > 0)
			{
				RenderBuffer::Desc desc = {
					 .Size = indexData.Size,
					 .Stride = indexData.GetStride(),
					 .Usage = (u32)ResourceUsage::IndexBuffer,
					 .CpuAccess = CpuAccessFlags::None,
					 .Alignment = true,
					 .InitData = indexData.Data
				};

				cluster->IndexBuffer = device->CreateBuffer(std::format("IndexBuffer_{}", (u64)(primitive)), desc);
			}
			cluster->Primitive = new PrimitiveData;
			cluster->Primitive->LocalToWorld = trans.WorldMatrix;
			_Clusters.push_back(cluster);

			_PrimitiveDataBuffer.push_back(*cluster->Primitive);
			_Renderer->AddCluster(u32(_PrimitiveDataBuffer.size()-1),cluster);
		}
	}
	else
	{
		check(0);
	}

	_Renderer->UpdatePrimitivesBuffer(_PrimitiveDataBuffer);
	
}

void RenderScene::RemovePrimitive(PrimitiveComponent* primitive)
{
	auto iter = _Clusters.begin();
	while (iter != _Clusters.end())
	{
		if ((*iter)->Component == primitive)
		{
			_Renderer->RemoveCluster(*iter);
			delete *iter;
			u32 pos = (u32)std::distance(_Clusters.begin(), iter);
			auto liter = _PrimitiveDataBuffer.begin();
			std::advance(liter, pos);
			_PrimitiveDataBuffer.erase(liter);
			iter = _Clusters.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	_Renderer->UpdatePrimitivesBuffer(_PrimitiveDataBuffer);
}

