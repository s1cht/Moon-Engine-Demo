#pragma once
#include <Core.h>

namespace Pawn::Render
{
	enum class ShaderType : SIZE_T
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3x3, Mat4x4,
		Bool
	};


	struct BufferLayout
	{
		ShaderType Type;
		AnsiString Name;
		SIZE_T Size;
		SIZE_T Offset;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = delete;

		virtual void Bind() = 0;


		//virtual 

	public:
		static VertexBuffer* Create();

	};

	class IndexBuffer
	{
	public:
		virtual void Bind() = 0;

		virtual uint32 GetCound() = 0;

	public:
		static IndexBuffer* Create();
	};


	SIZE_T SizeOfShaderType(ShaderType type);
}