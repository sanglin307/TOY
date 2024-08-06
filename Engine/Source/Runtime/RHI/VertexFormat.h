#pragma once

enum class VertexFormat
{
	Float32x4 = 0,
	Float32x3,
	Float32x2,
	Float32,
	U16,
	U32
};

enum class VertexAttribute
{
	Position = 0,
	Normal,
	UV0,
	Tangent,
	Color0,
	Max
};

struct VertexData
{
	VertexFormat    Format;
	u8* Data;
	u64 Size;

	u64 GetCount() const
	{
		return Size / GetStride();
	}

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
		else if (Format == VertexFormat::U16)
		{
			return sizeof(u16);
		}
		else if (Format == VertexFormat::U32)
		{
			return sizeof(u32);
		}
		else
			check(0);

		return 0;
	}
};