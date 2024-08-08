#include "Private.h"


#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

struct gltfTexture
{
	u32 textureIndex;
	RenderTexture* resource;
};

glTFLoader& glTFLoader::Instance()
{
	static glTFLoader sInst;
	return sInst;
}

Image* loadImage(fastgltf::Asset& asset, fastgltf::Image& image)
{
    Image* imageData = nullptr;
    std::visit(fastgltf::visitor{
    [](auto& arg) {},
    [&](fastgltf::sources::URI& filePath) {
        assert(filePath.fileByteOffset == 0);  
        assert(filePath.uri.isLocalPath());

        const std::string path(filePath.uri.path().begin(), filePath.uri.path().end());
        imageData = ImageReader::Instance().LoadFromFile(path);
    },
    [&](fastgltf::sources::Array& vector) {
        imageData = ImageReader::Instance().LoadFromMemory((u8*)vector.bytes.data(), static_cast<int>(vector.bytes.size()));
    },
    [&](fastgltf::sources::BufferView& view) {
        auto& bufferView = asset.bufferViews[view.bufferViewIndex];
        auto& buffer = asset.buffers[bufferView.bufferIndex];
        std::visit(fastgltf::visitor {
            [](auto& arg) {},
            [&](fastgltf::sources::Array& vector) {
                imageData = ImageReader::Instance().LoadFromMemory((u8*)vector.bytes.data() + bufferView.byteOffset, static_cast<int>(bufferView.byteLength));
            }
            }, buffer.data);
        },
    }, image.data);

	imageData->Name = image.name.c_str();

	return imageData;
}

MeshComponent* loadMesh(fastgltf::Asset& asset, fastgltf::Mesh& mesh, std::vector<Material*>& materials, std::set<Material*>& matSet)
{
	MeshComponent* m = new MeshComponent(mesh.name.c_str());
	for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) 
	{
		check(it->type == fastgltf::PrimitiveType::Triangles); // only implement triangle first....
		MeshSegment* segment = new MeshSegment(PrimitiveTopology::Triangle);
		auto* positionIt = it->findAttribute("POSITION");
		check(positionIt != it->attributes.end());
		check(it->indicesAccessor.has_value());  
 
		if (it->materialIndex.has_value()) 
		{
			segment->SetMaterial(materials[it->materialIndex.value()]);
			matSet.erase(materials[it->materialIndex.value()]);
		}
		 
		{
			// Position
			auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
			if (!positionAccessor.bufferViewIndex.has_value())
			{
				check(0);
			}
			check(positionAccessor.type == fastgltf::AccessorType::Vec3);
			Vector3f* vertices = (Vector3f*)std::malloc(positionAccessor.count * sizeof(Vector3f));
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
				vertices[idx] = Vector3(pos.x(), pos.y(), pos.z());
				});
			VertexData data = {
				.Format = VertexFormat::Float32x3,
				.Data = (u8*)vertices,
				.Size = positionAccessor.count * sizeof(Vector3f)
			};
			segment->InsertAttribute(VertexAttribute::Position, data);
			std::free(vertices);
		}

		constexpr u32 UVCount = 1;
		for (u32 i = 0; i < UVCount; i++) // uv0-4.
		{
			auto texcoordAttribute = std::string("TEXCOORD_") + std::to_string(i);
			if (const auto* texcoord = it->findAttribute(texcoordAttribute); texcoord != it->attributes.end()) 
			{
				// Tex coord
				auto& texCoordAccessor = asset.accessors[texcoord->accessorIndex];
				if (!texCoordAccessor.bufferViewIndex.has_value())
				{
					check(0);
				}
				check(texCoordAccessor.type == fastgltf::AccessorType::Vec2);
				Vector2f* texData = (Vector2f*)std::malloc(sizeof(Vector2f) * texCoordAccessor.count);
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
					texData[idx] = Vector2f(uv.x(),uv.y());
				});

				VertexData data = {
				.Format = VertexFormat::Float32x2,
				.Data = (u8*)texData,
				.Size = sizeof(Vector2f) * texCoordAccessor.count
				};
				segment->InsertAttribute((VertexAttribute)((u32)VertexAttribute::UV0 + i), data);
				std::free(texData);
			}
		}

		constexpr u32 ColorCount = 1;
		for (u32 i = 0; i < ColorCount; i++) 
		{
			auto colorAttribute = std::string("COLOR_") + std::to_string(i);
			if (const auto* iter = it->findAttribute(colorAttribute); iter != it->attributes.end())
			{
				auto& colorAccessor = asset.accessors[iter->accessorIndex];
				if (!colorAccessor.bufferViewIndex.has_value())
				{
					check(0);
				}
				if (colorAccessor.type == fastgltf::AccessorType::Vec3)
				{
					Vector3f* colorData = (Vector3f*)std::malloc(sizeof(Vector3f) * colorAccessor.count);
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, colorAccessor, [&](fastgltf::math::fvec3 color, std::size_t idx) {
						colorData[idx] = Vector3f(color.x(),color.y(),color.z());
						});

					VertexData data = {
					.Format = VertexFormat::Float32x3,
					.Data = (u8*)colorData,
					.Size = sizeof(Vector3f) * colorAccessor.count
					};
					segment->InsertAttribute((VertexAttribute)((u32)VertexAttribute::Color0 + i), data);
					std::free(colorData);
				}
				else if (colorAccessor.type == fastgltf::AccessorType::Vec4)
				{
					Vector4f* colorData = (Vector4f*)std::malloc(sizeof(Vector4f) * colorAccessor.count);
					fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(asset, colorAccessor, [&](fastgltf::math::fvec4 c, std::size_t idx) {
						colorData[idx] = Vector4(c[0],c[1],c[2],c[3]);
						});

					VertexData data = {
					.Format = VertexFormat::Float32x4,
					.Data = (u8*)colorData,
					.Size = sizeof(Vector4f) * colorAccessor.count
					};
					segment->InsertAttribute((VertexAttribute)((u32)VertexAttribute::Color0 + i), data);
					std::free(colorData);
				}
			}
		}

		// Tangent
		if (const auto* tangentIt = it->findAttribute("TANGENT"); tangentIt != it->attributes.end())
		{
			auto& tangentAccessor = asset.accessors[tangentIt->accessorIndex];
			if (!tangentAccessor.bufferViewIndex.has_value())
			{
				check(0);
			}
			check(tangentAccessor.type == fastgltf::AccessorType::Vec4);
			Vector4f* data = (Vector4f*)std::malloc(tangentAccessor.count * sizeof(Vector4f));
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(asset, tangentAccessor, [&](fastgltf::math::fvec4 d, std::size_t idx) {
				data[idx] = Vector4f(d[0],d[1],d[2],d[3]);
				});
			VertexData tdata = {
				.Format = VertexFormat::Float32x4,
				.Data = (u8*)data,
				.Size = tangentAccessor.count * sizeof(Vector4f)
			};
			segment->InsertAttribute(VertexAttribute::Tangent, tdata);
			std::free(data);
		}

		//Normal
		if (const auto* normalIt = it->findAttribute("NORMAL"); normalIt != it->attributes.end())
		{
			auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
			if (!normalAccessor.bufferViewIndex.has_value())
			{
				check(0);
			}
			check(normalAccessor.type == fastgltf::AccessorType::Vec3);
			Vector3f* data = (Vector3f*)std::malloc(normalAccessor.count * sizeof(Vector3f));
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, normalAccessor, [&](fastgltf::math::fvec3 d, std::size_t idx) {
				data[idx] = Vector3f(d[0],d[1],d[2]);
				});
			VertexData tdata = {
				.Format = VertexFormat::Float32x3,
				.Data = (u8*)data,
				.Size = normalAccessor.count * sizeof(Vector3f)
			};
			segment->InsertAttribute(VertexAttribute::Normal, tdata);
			std::free(data);
		}

		if (it->indicesAccessor.has_value())
		{
			auto& indexAccessor = asset.accessors[it->indicesAccessor.value()];
			if (!indexAccessor.bufferViewIndex.has_value())
			{
				check(0);
			}

			if (indexAccessor.componentType == fastgltf::ComponentType::UnsignedByte || indexAccessor.componentType == fastgltf::ComponentType::UnsignedShort) 
			{
				// use u32 always.
				u32* data = (u32*)std::malloc(sizeof(u32) * indexAccessor.count);
				fastgltf::copyFromAccessor<u32>(asset, indexAccessor, data);
				VertexData tdata = {
					.Format = VertexFormat::U32,
					.Data = (u8*)data,
					.Size = indexAccessor.count * sizeof(u32)
				};
				segment->AddIndicesData(tdata);
				std::free(data);
			}
			else 
			{
				u32* data = (u32*)std::malloc(sizeof(u32) * indexAccessor.count);
				fastgltf::copyFromAccessor<u32>(asset, indexAccessor, data);
				VertexData tdata = {
					.Format = VertexFormat::U32,
					.Data = (u8*)data,
					.Size = indexAccessor.count * sizeof(u32)
				};
				segment->AddIndicesData(tdata);
				std::free(data);
			}
		}

		m->AddSegment(segment);
	}

	m->Build();

	return m;
}

PixelFormat TranslateToSRGBFormat(PixelFormat format)
{
	if (format == PixelFormat::R8G8B8A8_UNORM)
	{
		format = PixelFormat::R8G8B8A8_UNORM_SRGB;
	}
	else if (format == PixelFormat::BC1_UNORM)
	{
		format = PixelFormat::BC1_UNORM_SRGB;
	}
	else if (format == PixelFormat::BC2_UNORM)
	{
		format = PixelFormat::BC2_UNORM_SRGB;
	}
	else if (format == PixelFormat::BC3_UNORM)
	{
		format = PixelFormat::BC3_UNORM_SRGB;
	}
	else if (format == PixelFormat::BC7_UNORM)
	{
		format = PixelFormat::BC7_UNORM_SRGB;
	}
	else if (format == PixelFormat::B8G8R8A8_TYPELESS)
	{
		format = PixelFormat::B8G8R8A8_UNORM_SRGB;
	}
	else if (format == PixelFormat::B8G8R8X8_TYPELESS)
	{
		format = PixelFormat::B8G8R8X8_UNORM_SRGB;
	}

	return format;
}

RenderTexture* loadTexture(RenderDevice* device, fastgltf::Asset& asset, u32 textureIndex, bool srgb, ImageCompressType compressType, std::vector<gltfTexture>& gltfTextures, std::vector<Image*>& images, std::set<Image*>& imageSet, std::vector<Sampler*>& samplers,std::set<Sampler*>& samplerSet)
{
	if (gltfTextures[textureIndex].resource)
		return gltfTextures[textureIndex].resource;

	fastgltf::Texture tex = asset.textures[textureIndex];
	check(tex.imageIndex.has_value());
	Image* image = images[tex.imageIndex.value()];
	imageSet.erase(image);
	image->CompressType = compressType;

	RenderTexture::Desc desc = {
		.Width = image->Width,
		.Height = image->Height,
		.DepthOrArraySize = 1,
		.MipLevels = RenderTexture::CalculateMipCount(image->Width,image->Height),
		.Format = srgb ? TranslateToSRGBFormat(image->Format) : image->Format,
		.Usage = (u32)ResourceUsage::ShaderResource,
		.Dimension = ResourceDimension::Texture2D,
		.Data = image->Data,
		.Size = image->Size
	};

	desc.CompressType = compressType;

	RenderTexture* texture = device->CreateTexture(tex.name.c_str(), desc);
	if (tex.samplerIndex.has_value())
	{
		texture->OptionSampler = samplers[tex.samplerIndex.value()];
		samplerSet.erase(texture->OptionSampler);
	}

	gltfTextures[textureIndex].resource = texture;
	return texture;
}

Material* loadMaterial(RenderDevice* device, fastgltf::Asset& asset, fastgltf::Material& material, std::vector<gltfTexture>& gltfTextures, std::vector<Image*>& images, std::set<Image*>& imageSet, std::vector<Sampler*>& samplers, std::set<Sampler*>& samplerSet)
{
	Material* mat = new Material;
	mat->Name = material.name;
	mat->Unit = material.unlit;
	mat->DoubleSide = material.doubleSided;

	mat->BaseColorFactor = float4(material.pbrData.baseColorFactor[0], material.pbrData.baseColorFactor[1], material.pbrData.baseColorFactor[2], material.pbrData.baseColorFactor[3]);
	mat->EmissiveFactor = float3(material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2]);
	mat->MetalnessFactor = material.pbrData.metallicFactor;
	mat->RoughnessFactor = material.pbrData.roughnessFactor;
	mat->IOR = material.ior;
	if (material.pbrData.baseColorTexture.has_value())
	{
		mat->BaseColorTexture.TextureCoordIndex = (u32)material.pbrData.baseColorTexture->texCoordIndex;
		u32 texIndex = (u32)material.pbrData.baseColorTexture->textureIndex;
		mat->BaseColorTexture.Texture = loadTexture(device,asset,texIndex,true,ImageCompressType::BC7_SRGB,gltfTextures,images,imageSet,samplers,samplerSet);
	}

	if (material.normalTexture.has_value())
	{
		mat->NormalTexture.TextureCoordIndex = (u32)material.normalTexture->texCoordIndex;
		u32 texIndex = (u32)material.normalTexture->textureIndex;
		mat->NormalTexture.Texture = loadTexture(device, asset, texIndex, false, ImageCompressType::BC5_SNorm, gltfTextures, images, imageSet, samplers, samplerSet);
	}
 
	if (material.pbrData.metallicRoughnessTexture.has_value())
	{
		mat->RoughnessMetalnessTexture.TextureCoordIndex = (u32)material.pbrData.metallicRoughnessTexture->texCoordIndex;
		u32 texIndex = (u32)material.pbrData.metallicRoughnessTexture->textureIndex;
		mat->RoughnessMetalnessTexture.Texture = loadTexture(device, asset, texIndex, false, ImageCompressType::BC7, gltfTextures, images, imageSet, samplers, samplerSet);
	}

	if (material.emissiveTexture.has_value())
	{
		mat->EmissiveTexture.TextureCoordIndex = (u32)material.emissiveTexture->texCoordIndex;
		u32 texIndex = (u32)material.emissiveTexture->textureIndex;
		mat->EmissiveTexture.Texture = loadTexture(device, asset, texIndex, true, ImageCompressType::BC7_SRGB, gltfTextures, images, imageSet, samplers, samplerSet);
	}

	if (material.anisotropy != nullptr)
	{
		if (material.anisotropy->anisotropyTexture.has_value())
		{
			mat->AnisotropyTexture.TextureCoordIndex = (u32)material.anisotropy->anisotropyTexture->texCoordIndex;
			u32 texIndex = (u32)material.anisotropy->anisotropyTexture->textureIndex;
			mat->AnisotropyTexture.Texture = loadTexture(device, asset, texIndex, false, ImageCompressType::None,gltfTextures, images, imageSet, samplers, samplerSet);
		}

		mat->AnisotropyRotation = material.anisotropy->anisotropyRotation;
		mat->AnisotropyStrength = material.anisotropy->anisotropyStrength;
	}
	

	if (material.alphaMode == fastgltf::AlphaMode::Opaque)
	{
		mat->BlendMode = MaterialBlend::Opaque;
	}
	else if (material.alphaMode == fastgltf::AlphaMode::Mask)
	{
		mat->BlendMode = MaterialBlend::Masked;
	}
	else if (material.alphaMode == fastgltf::AlphaMode::Blend)
	{
		mat->BlendMode = MaterialBlend::Transparent;
	}
	mat->AlphaCutoff = material.alphaCutoff;

	return mat;
}

Sampler* loadSampler(RenderDevice* device,fastgltf::Sampler& sampler)
{
	Sampler::Desc desc;
	if (sampler.magFilter.has_value())
	{
		//mag only have nearest & linear.
		if (sampler.magFilter.value() == fastgltf::Filter::Nearest)
		{
			desc.MagFilter = SampleFilterMode::Point;
		}
		else if (sampler.magFilter.value() == fastgltf::Filter::Linear)
		{
			desc.MagFilter = SampleFilterMode::Linear;
		}
	}
	else if (sampler.minFilter.has_value())
	{
		if (sampler.minFilter.value() == fastgltf::Filter::Nearest)
		{
			desc.MinFilter = SampleFilterMode::Point;
		}
		else if (sampler.magFilter.value() == fastgltf::Filter::Linear)
		{
			desc.MinFilter = SampleFilterMode::Linear;
		}
		else if (sampler.minFilter.value() == fastgltf::Filter::NearestMipMapNearest)
		{
			desc.MinFilter = SampleFilterMode::Point;
			desc.MipFilter = SampleFilterMode::Point;
		}
		else if (sampler.minFilter.value() == fastgltf::Filter::LinearMipMapNearest)
		{
			desc.MinFilter = SampleFilterMode::Linear;
			desc.MipFilter = SampleFilterMode::Point;
		}
		else if (sampler.minFilter.value() == fastgltf::Filter::NearestMipMapLinear)
		{
			desc.MinFilter = SampleFilterMode::Point;
			desc.MipFilter = SampleFilterMode::Linear;
		}
		else if (sampler.minFilter.value() == fastgltf::Filter::LinearMipMapLinear)
		{
			desc.MinFilter = SampleFilterMode::Linear;
			desc.MipFilter = SampleFilterMode::Linear;
		}
	}
	if (sampler.wrapS == fastgltf::Wrap::ClampToEdge)
	{
		desc.AddressU = TextureAddressMode::Clamp;
	}
	else if (sampler.wrapS == fastgltf::Wrap::MirroredRepeat)
	{
		desc.AddressU = TextureAddressMode::Mirror;
	}
	else if (sampler.wrapS == fastgltf::Wrap::Repeat)
	{
		desc.AddressU = TextureAddressMode::Wrap;
	}

	if (sampler.wrapT == fastgltf::Wrap::ClampToEdge)
	{
		desc.AddressV = TextureAddressMode::Clamp;
	}
	else if (sampler.wrapT == fastgltf::Wrap::MirroredRepeat)
	{
		desc.AddressV = TextureAddressMode::Mirror;
	}
	else if (sampler.wrapT == fastgltf::Wrap::Repeat)
	{
		desc.AddressV = TextureAddressMode::Wrap;
	}

	Sampler* s = device->CreateSampler(desc);
	s->SetName(sampler.name.c_str());
	return s;
}

LightComponent* loadLight(fastgltf::Light& light)
{
	LightComponent::Desc desc = {
		.Color = float3(light.color[0],light.color[1],light.color[2]),
		.Intensity = light.intensity,
		.Range = light.range.value_or(-1),
		.InnerConeAngle = light.innerConeAngle.value_or(-1),
		.OuterConeAngle = light.outerConeAngle.value_or(-1)
	};
	desc.Type = LightType::Point;
	if (light.type == fastgltf::LightType::Directional)
		desc.Type = LightType::Directional;
	else if (light.type == fastgltf::LightType::Spot)
		desc.Type = LightType::Spot;

	return new LightComponent(light.name.c_str(), desc);
}

CameraComponent* loadCamera(fastgltf::Camera& camera)
{
	CameraComponent* cam = nullptr;
	std::visit(fastgltf::visitor{
		[&](fastgltf::Camera::Perspective& perspective) {
			CameraComponent::Desc desc = {
				.Type = CameraType::Perspective,
				.AspectRatio = perspective.aspectRatio.value_or(0),
				.YFov = perspective.yfov,
				.ZFar = perspective.zfar.value_or(-1),
				.ZNear = perspective.znear
			};
			cam = new CameraComponent(camera.name.c_str(), desc);
		},
		[&](fastgltf::Camera::Orthographic& orthographic) {
			CameraComponent::Desc desc = {
				.Type = CameraType::Orthographic,
				.Width = orthographic.xmag,
				.Height = orthographic.ymag,
				.ZFar = orthographic.zfar,
				.ZNear = orthographic.znear
			};
			cam = new CameraComponent(camera.name.c_str(), desc);
		},
		}, camera.camera);

	return cam;
}

void AddNodeChildren(size_t index, Node* parent,std::vector<Node*>& nodes, fastgltf::Asset& asset)
{
	Node* node = nodes[index];
	fastgltf::Node& child = asset.nodes[index];
	for (size_t c : child.children)
	{
		AddNodeChildren(c, node, nodes, asset);
	}
	parent->AddChild(node);
}

void glTFLoader::Load(std::string_view path, std::vector<Layer*>& newLayers)
{
	auto file = PathUtil::glTFs() / path;
	if (!std::filesystem::exists(file))
	{
		LOG_ERROR(glTFLoader, std::format("can't find {} to load!", file.string()));
		return;
	}

	static constexpr auto supportedExtensions =
		fastgltf::Extensions::KHR_mesh_quantization |
		fastgltf::Extensions::KHR_texture_transform |
		fastgltf::Extensions::KHR_materials_variants;

	fastgltf::Parser parser(supportedExtensions);

	constexpr auto gltfOptions =
		fastgltf::Options::DontRequireValidAssetMember |
		fastgltf::Options::AllowDouble |
		fastgltf::Options::LoadExternalBuffers |
		fastgltf::Options::LoadExternalImages |
		fastgltf::Options::GenerateMeshIndices |
		fastgltf::Options::DecomposeNodeMatrices;

	auto gltfFile = fastgltf::MappedGltfFile::FromPath(file);
	
	if (!bool(gltfFile)) 
	{
		LOG_ERROR(glTFLoader, std::format("parse file {} error: {}", file.string(), fastgltf::getErrorMessage(gltfFile.error())));
		return;
	}

	auto asset = parser.loadGltf(gltfFile.get(), file.parent_path(), gltfOptions);
	if (asset.error() != fastgltf::Error::None) 
	{
		LOG_ERROR(glTFLoader, std::format("Failed to load glTF: {} error: {}", file.string(), fastgltf::getErrorMessage(asset.error())));
		return;
	}

	RenderDevice* device = GameEngine::Instance().GetRHI().GetDevice();

	// these set used to check any asset don't referrence in layer, avoid memory leak.
	std::set<Image*> imageSet;
	std::set<Sampler*> samplerSet;
	std::set<RenderTexture*> texSet;
	std::set<Material*> matSet;
	std::set<MeshComponent*> meshSet;
	std::set<CameraComponent*> camSet;
	std::set<LightComponent*> lightSet;

    std::vector<Image*> images;
    for (auto image : asset->images)
    {
		Image* im = loadImage(asset.get(), image);
        images.push_back(im);
		imageSet.insert(im);
    }


	std::vector<Sampler*> samplers;
	for (fastgltf::Sampler& sampler : asset->samplers)
	{
		Sampler* s = loadSampler(device, sampler);
		samplers.push_back(s);
		samplerSet.insert(s);
	}

	std::vector<gltfTexture> textures;
	for (u32 i=0; i<asset->textures.size();i++)
	{
		textures.push_back(gltfTexture{
			.textureIndex = i,
			.resource = nullptr
			});
	}

	std::vector<Material*> materials;
	for (fastgltf::Material& mat : asset->materials)
	{
		Material* m = loadMaterial(device,asset.get(), mat, textures, images, imageSet,samplers,samplerSet);
		materials.push_back(m);
		matSet.insert(m);
	}

	std::vector<MeshComponent*> meshes;
	for (fastgltf::Mesh& mesh : asset->meshes)
	{
		MeshComponent* m = loadMesh(asset.get(), mesh, materials,matSet);
		meshes.push_back(m);
		meshSet.insert(m);
	}

	std::vector<CameraComponent*> cameras;
	for (fastgltf::Camera& cam : asset->cameras)
	{
		CameraComponent* c = loadCamera(cam);
		cameras.push_back(c);
		camSet.insert(c);
	}

	std::vector<LightComponent*> lights;
	for (fastgltf::Light& light : asset->lights)
	{
		LightComponent* l = loadLight(light);
		lights.push_back(l);
		lightSet.insert(l);
	}

	std::vector<Node*> nodes;
	for (fastgltf::Node& node : asset->nodes)
	{
		Node* n = new Node(node.name.c_str());
		std::visit(fastgltf::visitor{
					[&](const fastgltf::math::fmat4x4& matrix) {
						check(0); // Option::DecomposeNodeMatrices 
					},
					[&](const fastgltf::TRS& trs) {
						n->SetScale(float3(trs.scale.x(),trs.scale.y(),trs.scale.z()));
						n->SetRotation(quaternion(trs.rotation.x(), trs.rotation.y(), trs.rotation.z(), trs.rotation.w()));
						n->SetTranslate(float3(trs.translation.x(), trs.translation.y(), trs.translation.z()));
					}
			}, node.transform);

		if (node.cameraIndex.has_value())
		{
			n->Attach(cameras[node.cameraIndex.value()]);
			camSet.erase(cameras[node.cameraIndex.value()]);
		}

		if (node.lightIndex.has_value())
		{
			n->Attach(lights[node.lightIndex.value()]);
			lightSet.erase(lights[node.lightIndex.value()]);
		}

		if (node.meshIndex.has_value())
		{
			n->Attach(meshes[node.meshIndex.value()]);
			meshSet.erase(meshes[node.meshIndex.value()]);
		}
		nodes.push_back(n);
	}

	for (fastgltf::Scene& s : asset->scenes)
	{
		if (s.nodeIndices.size() > 0)
		{
			Layer* layer = new Layer(s.name.c_str());
			for (size_t index : s.nodeIndices)
			{
				Node* n = nodes[index];
				fastgltf::Node& tfnode = asset->nodes[index];
				for (size_t c : tfnode.children)
				{
					AddNodeChildren(c, n, nodes, asset.get());
				}
				layer->AddNode(n);
			}
			newLayers.push_back(layer);
		}
	}

	if (imageSet.size() > 0)
	{
		for (Image* im : imageSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced image : {}", file.string(), im->Name));
			// don't delete it, saved in ImageReader's cache.
		}
	}

	if (samplerSet.size() > 0)
	{
		for (Sampler* s : samplerSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced sampler : {}", file.string(), s->GetName()));
			// cached in RHI derived implementation.
		}
	}

	if (texSet.size() > 0)
	{
		for (RenderTexture* t : texSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced texture : {}", file.string(), t->GetName()));
			delete t;
		}
	}

	if (matSet.size() > 0)
	{
		for (Material* m : matSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced material : {}", file.string(), m->Name));
			delete m;
		}
	}

	if (meshSet.size() > 0)
	{
		for (MeshComponent* m : meshSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced mesh : {}", file.string(), m->GetName()));
			delete m;
		}
	}

	if (camSet.size() > 0)
	{
		for (CameraComponent* c : camSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced camera : {}", file.string(), c->GetName()));
			delete c;
		}
	}
 
	if (lightSet.size() > 0)
	{
		for (LightComponent* l : lightSet)
		{
			LOG_ERROR(glTFLoader, std::format("glTF: {} not referrenced light : {}", file.string(), l->GetName()));
			delete l;
		}
	}
}