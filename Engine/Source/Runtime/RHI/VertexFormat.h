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
};