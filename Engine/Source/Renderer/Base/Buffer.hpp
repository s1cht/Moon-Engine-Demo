// Buffer.h
//
// Abstract classes for every supported buffer type:
// Vertex, Index, Uniform, Storage, Indirect
//
// To create buffer, fill the "BufferType"Specification
// and pass it into the ME::Render::"BufferType"::Create() method
//
// 2025

#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Renderer/Base/Pipeline.h"
#include "Renderer/Base/Shader.h"


// This macro contains definitions and implementations of these methods and members:
//
// Members:
// 1. SpecificationType m_Specification (accessible in API realization)
// 
// Methods:
// 1. Abstract class constructor (to init m_Specification)
// 2. GetSpecification()
// 3. Create()
// and Create"API" (for API object creation)
// 


#define ME_BUFFER_TEMPLATE(bufferType, specsType)																\
public:																											\
    bufferType(specsType specs) : Buffer(specs.DebugName), m_Specification(std::move(specs)) {}					\
    inline const specsType& GetSpecification() const { return m_Specification; }								\
	inline const BufferSpecification& GetBaseSpecification() const final										\
        { return static_cast<const BufferSpecification&>(m_Specification); }									\
protected:																										\
	specsType m_Specification;																					\
public:																											\
    static ME::Core::Memory::Reference<ME::Render::bufferType> Create(const specsType& specification);			\
private:																										\
    static ME::Core::Memory::Reference<ME::Render::bufferType> CreateVulkan(const specsType& specification);	\

#define ME_BUFFER_SET																					\
    public: inline void ChangeSet(uint32 set) final { m_Specification.Set = set; }			

namespace ME::Render
{
	enum class MemoryType : uint8
	{
		VRAM, RAM
	};

    #pragma region Specifications

	struct BufferSpecification
	{
		uint32 SetIndex;
		uint32 Set;
		uint32 Binding;
		Render::ResourceBinding ResourceBinding;
		ME::Core::String DebugName;
	};

	struct VertexBufferSpecification : BufferSpecification
	{
		SIZE_T Size;
	};

	struct IndexBufferSpecification : BufferSpecification
	{
		SIZE_T IndexCount;
	};

	struct UniformSpecification : BufferSpecification
	{
		SIZE_T Size;
		ME::Render::MemoryType MemoryType;
	};

	struct StorageBufferSpecification : BufferSpecification
	{
		SIZE_T Size;
		ME::Render::MemoryType MemoryType;
	};

	struct IndirectBufferSpecification : BufferSpecification
	{
		SIZE_T Size;
    };

    #pragma endregion

    // Buffer abstract class
	// Contains primary methods
	class MEAPI Buffer : public RenderObject
	{
	public:
		Buffer(const ME::Core::String& debugName) : m_DebugName(debugName) {}

		virtual void ChangeSet(uint32 set) = 0;

	    virtual void SetData(void* data, SIZE_T size, SIZE_T offset = 0) = 0;
		virtual void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset = 0) = 0;

		virtual void Resize(SIZE_T size) = 0;

		virtual void Clear() = 0;
		virtual void Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) = 0;

		virtual inline void UpdateResourceSet(uint32 setIndex) = 0;
	    virtual inline uint32 GetResourceSet() const = 0;

		virtual inline const BufferSpecification& GetBaseSpecification() const = 0;

	protected:
		ME::Core::String m_DebugName;
	};

	// Vertex buffer abstract class
	class MEAPI VertexBuffer : public Buffer
	{
	    ME_BUFFER_TEMPLATE(VertexBuffer, VertexBufferSpecification);

	public:
		// Overriding every unsupported method
		void UpdateResourceSet(uint32 setIndex) final { ME_WARN("UpdateResourceSet() is unsupported in vertex buffer! Object: \"{}\"", m_DebugName); }
		uint32 GetResourceSet() const final { ME_WARN("GetResourceSet() is unsupported in vertex buffer! Object: \"{}\"", m_DebugName); return ~0u; }
		void ChangeSet(uint32 set) final { ME_WARN("ChangeSet() is unsupported in vertex buffer! Object: \"{}\"", m_DebugName); }
	};

	// Index buffer abstract class
	class MEAPI IndexBuffer : public Buffer
	{
		ME_BUFFER_TEMPLATE(IndexBuffer, IndexBufferSpecification);

	public:
		// Overriding every unsupported method
	    void UpdateResourceSet(uint32 setIndex) final { ME_WARN("UpdateResourceSet() is unsupported in index buffer! Object: \"{}\"", m_DebugName); }
	    uint32 GetResourceSet() const final { ME_WARN("GetResourceSet() is unsupported in index buffer! Object: \"{}\"", m_DebugName); return ~0u; }
		void ChangeSet(uint32 set) final { ME_WARN("ChangeSet() is unsupported in index buffer! Object: \"{}\"", m_DebugName); }

	    void SetData(void* data, SIZE_T size, SIZE_T offset) final { ME_WARN("SetData() is unsupported in index buffer! Object: \"{}\"", m_DebugName); }
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset) final { ME_WARN("SetData() is unsupported in index buffer! Object: \"{}\"", m_DebugName); }

	public:
		virtual SIZE_T GetCount() const = 0;

		virtual void SetData(uint32* indices, SIZE_T indexCount, SIZE_T offset = 0) = 0;
		virtual void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, uint32* indices, SIZE_T indexCount, SIZE_T offset = 0) = 0;

		virtual void Resize(SIZE_T indexCount) = 0;

	};

	// Uniform abstract class
	class MEAPI Uniform : public Buffer
	{
		ME_BUFFER_TEMPLATE(Uniform, UniformSpecification);
	    ME_BUFFER_SET;
	};

	// Storage buffer abstract class
	class MEAPI StorageBuffer : public Buffer
	{
		ME_BUFFER_TEMPLATE(StorageBuffer, StorageBufferSpecification);
		ME_BUFFER_SET;
	};

	// Indirect buffer abstract class
	class MEAPI IndirectBuffer : public Buffer
	{
		ME_BUFFER_TEMPLATE(IndirectBuffer, IndirectBufferSpecification);
		ME_BUFFER_SET;

	public:
		// Do nothing for SetData
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset) final {}
		void SetData(void* data, SIZE_T size, SIZE_T offset) final {};
	};
}