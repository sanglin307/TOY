#pragma once


class RenderScene
{
public:
	void Init();
	void Destroy();

	struct Vertex
	{
		Vector3f position;
		Vector3f color;
	};

	struct SceneConstantBuffer
	{
		Vector3f offset;
		float padding[61]; // Padding so the constant buffer is 256-byte aligned.
	};
};
