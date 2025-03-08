#pragma once
#include <Core.h>

namespace Pawn::Render
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = delete;

		//virtual 

	public:
		static VertexBuffer* Create();

	};

	class IndexBuffer
	{

	public:
		static IndexBuffer* Create();
	};
}