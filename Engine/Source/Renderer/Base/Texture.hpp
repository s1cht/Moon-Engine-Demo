#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/Rect2D.hpp>

#include "RenderCore.hpp"

#define ME_TEXTURE_TEMPLATE(textureType, specsType)																\
public:																											\
    textureType(specsType specs) : Texture(specs.DebugName), m_Specification(std::move(specs)) {}				\
    inline const specsType& GetSpecification() const { return m_Specification; }								\
	inline const TextureSpecification& GetBaseSpecification() const final										\
        { return static_cast<const TextureSpecification&>(m_Specification); }									\
protected:																										\
	specsType m_Specification;																					\
public:																											\
    static ME::Core::Memory::Reference<ME::Render::textureType> Create(const specsType& specification);			\
private:																										\
    static ME::Core::Memory::Reference<ME::Render::textureType> CreateVulkan(const specsType& specification);	\

#define ME_TEXTURE_SET()																					\
    public: inline void UpdateSetIndex(uint32 setIndex) final { m_Specification.SetIndex = setIndex; }		\
    public: inline void UpdateSet(uint32 set) final { m_Specification.Set = set; }							\
    public: inline void UpdateBinding(uint32 binding) final { m_Specification.Binding = binding; }							

namespace ME::Render
{
    struct TextureSpecification
	{
		uint32 SetIndex;
		uint32 Set;
		uint32 Binding;

		// String
		void* Data;
		SIZE_T DataSize;

		// Sampler
		bool EnableAnisotropy;
		float32 MaxAnisotropy;
		float32 MaxLOD;
		SamplerFilter MinFilter;
		SamplerFilter MagFilter;

		// Image
		bool IsDepth = false;
		bool IsStencil = false;
		bool bOwnsImage = true;
		uint32 MipLevels;
		uint32 CubeMapCount;
		ME::Render::Format Format;
		ME::Render::ImageLayout Layout;
		ME::Render::ImageUsageFlags Usage;
		ME::Render::SampleCount SampleCount;
		ME::Core::String DebugName;
	};

	struct Texture1DSpecification : TextureSpecification
	{
		uint32 Resolution;
	};

	struct Texture2DSpecification : TextureSpecification
	{
		ME::Core::Math::Resolution2D<uint32> Resolution;
	};

	struct Texture3DSpecification : TextureSpecification
	{
		ME::Core::Math::Resolution3D<uint32> Resolution;
	};

	class MEAPI Texture : public RenderMemoryObject
	{
	public:
		Texture(ME::Core::String debugName) { m_DebugName = std::move(debugName); }

	public:
		virtual void SetData(void* data, SIZE_T size) = 0;

		virtual void UpdateSetIndex(uint32 setIndex) = 0;
		virtual void UpdateSet(uint32 set) = 0;
		virtual void UpdateBinding(uint32 binding) = 0;

		virtual inline const TextureSpecification& GetBaseSpecification() const = 0;
	};

	class MEAPI Texture1D : public Texture
	{
		ME_TEXTURE_TEMPLATE(Texture1D, Texture1DSpecification);
		ME_TEXTURE_SET();

		ME_RENDER_OBJECT_TYPE(Texture1D);

	public:
		virtual uint32 GetResolution() const = 0;
	};

	class MEAPI Texture2D : public Texture
	{
		ME_TEXTURE_TEMPLATE(Texture2D, Texture2DSpecification);
		ME_TEXTURE_SET();

		ME_RENDER_OBJECT_TYPE(Texture2D);

	public:
		virtual ME::Core::Math::Resolution2D<uint32> GetResolution() const = 0;
	};

	class MEAPI Texture3D : public Texture
	{
		ME_TEXTURE_TEMPLATE(Texture3D, Texture3DSpecification);
		ME_TEXTURE_SET();

		ME_RENDER_OBJECT_TYPE(Texture3D);

	public:
		virtual ME::Core::Math::Resolution3D<uint32> GetResolution() const = 0;
	};

	inline constexpr MEAPI ImageUsageFlags operator|(ME::Render::ImageUsageFlags a, ME::Render::ImageUsageFlags b)
	{
		return static_cast<ImageUsageFlags>(static_cast<uint32>(a) | static_cast<uint32>(b));
	}

	inline constexpr MEAPI ImageUsageFlags operator|=(ME::Render::ImageUsageFlags& a, ME::Render::ImageUsageFlags b)
	{
		a = a | b;
		return a;
	}

	inline constexpr ImageUsageFlags operator&(ImageUsageFlags a, ImageUsageFlags b)
	{
		return static_cast<ImageUsageFlags>(static_cast<uint32>(a) & static_cast<uint32>(b));
	}
}
