#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Renderer/Base/Pipeline.h"
#include "Renderer/Base/Shader.h"

namespace ME::Render
{
	class VertexBufferLayout;

	enum class Usage
	{
		Default = 0,
		Immutable,
		Dynamic,
		Staging
	};

	class MOON_API VertexBuffer : public RenderObject
	{
	public:
		virtual ~VertexBuffer() {};

		virtual void Bind(VertexBufferLayout& layout) = 0;
		virtual void Unbind() = 0;

		virtual void SetData(void* data, SIZE_T size) = 0;
		virtual void* GetData() = 0;

	public:
		static VertexBuffer* Create(void* data, SIZE_T size, Usage usage);
		static VertexBuffer* CreateDirectX12VertexBuffer(void* data, SIZE_T size, Usage usage);
		// ...
	};

	class MOON_API IndexBuffer : public RenderObject
	{
	public:
		virtual ~IndexBuffer() {};

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(void* data, uint32 count) = 0;
		virtual void* GetData() = 0;

		virtual uint32 GetCount() = 0;

	public:
		static IndexBuffer* Create(void* data, uint32 count, Usage usage);

	};

	class MOON_API Uniform : public RenderObject
	{
	public:
		virtual ~Uniform() {};

		virtual void Bind(uint32 index, Shader::Type stage) = 0;
		virtual void Unbind(uint32 index, Shader::Type stage) = 0;

		virtual void SetData(void* data, SIZE_T size) = 0;
		virtual void* GetData() = 0;

	public:
		static Uniform* Create(SIZE_T size, Usage usage);

	};


}