#pragma once

enum class VertexFormat
{
	Float32x4 = 0,
	Float32x3 = 1,
	Float32x2 = 2,
	Float32 = 3,
};

enum class VertexAttribute
{
	Position = 0,
	Normal,
	UV0,
	UV1,
	Tangent,
	Color,
	Max
};

struct VertexData
{
	VertexFormat    Format;
	u8* Data;
	u64 Size;

	u32 GetStride() const 
	{
		if (Format == VertexFormat::Float32x4)
		{
			return sizeof(float) * 4;
		}
		else if (Format == VertexFormat::Float32x3)
		{
			return sizeof(float) * 3;
		}
		else if (Format == VertexFormat::Float32x2)
		{
			return sizeof(float) * 2;
		}
		else if (Format == VertexFormat::Float32)
		{
			return sizeof(float);
		}
		return 0;
	}
};