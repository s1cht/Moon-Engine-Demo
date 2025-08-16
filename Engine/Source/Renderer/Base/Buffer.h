#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Renderer/Base/Pipeline.h"
#include "Renderer/Base/Shader.h"

namespace ME::Render
{
	enum class MemoryType : uint8
	{
		VRAM, RAM
	};

	struct VertexBufferSpecification
	{
		SIZE_T Size;
		ME::Core::Containers::String DebugName;
	};

	struct IndexBufferSpecification
	{
		int32 IndexCount;
		ME::Core::Containers::String DebugName;
	};

	struct UniformSpecification
	{
		uint32 SetIndex;
		SIZE_T Size;
		uint32 Set;
		uint32 Binding;
		Render::ResourceLayout Layout;
		ME::Render::MemoryType MemoryType;
		ME::Render::ShaderStage Stage;
		ME::Core::Containers::String DebugName;
	};

	struct StorageBufferSpecification
	{
		SIZE_T Size;
		uint32 Set;
		uint32 Binding;
		Render::ResourceLayout Layout;
		ME::Render::MemoryType MemoryType;
		ME::Render::ShaderStage Stage;
		ME::Core::Containers::String DebugName;
	};

	class MOON_API Buffer : public RenderObject
	{
	public:
		virtual void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) = 0;
		virtual void Unbind() = 0;
		 
		virtual void SetData(void* data, SIZE_T size) = 0;
		virtual void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size) = 0;

	};

	class MOON_API VertexBuffer : public Buffer
	{
	public:
		static ME::Core::Memory::Reference<ME::Render::VertexBuffer> Create(const VertexBufferSpecification& specification);

	private:
		static ME::Core::Memory::Reference<ME::Render::VertexBuffer> CreateVulkan(const VertexBufferSpecification& specification);

	};

	class MOON_API IndexBuffer : public Buffer
	{
	public:
		virtual uint32 GetCount() const = 0;

		virtual void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, uint32* indices, SIZE_T indexCount) = 0;

	public:
		static ME::Core::Memory::Reference<ME::Render::IndexBuffer> Create(const IndexBufferSpecification& specification);

	private:
		static ME::Core::Memory::Reference<ME::Render::IndexBuffer> CreateVulkan(const IndexBufferSpecification& specification);
	};

	class MOON_API Uniform : public Buffer
	{
	public:
		void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override {}
		void Unbind() override {}

		virtual inline uint32 GetResourceSet() const = 0;

		virtual inline const UniformSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<ME::Render::Uniform> Create(const UniformSpecification& specification);

	private:
		static ME::Core::Memory::Reference<ME::Render::Uniform> CreateVulkan(const UniformSpecification& specification);
	};

	class MOON_API StorageBuffer : public Buffer
	{
	public:
		void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override {}
		void Unbind() override {}

		virtual inline uint32 GetResourceSet() const = 0;

		virtual inline const UniformSpecification& GetSpecification() const = 0;

	};

}