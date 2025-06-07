#pragma once

#include <Core.hpp>

#include "Core/Math/Math.hpp"
#include "Core/Misc/Rect2D.hpp"

namespace Pawn::Render
{
	enum class Format : uint32
	{
		None = 0,
		// 8-bit formats
		R8_UINT, R8_SINT, R8_SRGB, R8_UNORM, R8_SNORM,
		RG8_UINT, RG8_SINT, RG8_SRGB, RG8_UNORM, RG8_SNORM,
		RGB8_UINT, RGB8_SINT, RGB8_SRGB, RGB8_UNORM, RGB8_SNORM,
		RGBA8_UINT, RGBA8_SINT, RGBA8_SRGB, RGBA8_UNORM, RGBA8_SNORM,

		// 16-bit formats
		R16_UINT, R16_SINT, R16_UNORM, R16_SNORM, R16_SFLOAT,
		RG16_UINT, RG16_SINT, RG16_UNORM, RG16_SNORM, RG16_SFLOAT,
		RGB16_UINT, RGB16_SINT, RGB16_UNORM, RGB16_SNORM, RGB16_SFLOAT,
		RGBA16_UINT, RGBA16_SINT, RGBA16_UNORM, RGBA16_SNORM, RGBA16_SFLOAT,

		// 32-bit formats
		R32_UINT, R32_SINT, R32_SFLOAT,
		RG32_UINT, RG32_SINT, RG32_SFLOAT,
		RGB32_UINT, RGB32_SINT, RGB32_SFLOAT,
		RGBA32_UINT, RGBA32_SINT, RGBA32_SFLOAT,

		// 64-bit formats
		R64_UINT, R64_SINT, R64_SFLOAT,
		RG64_UINT, RG64_SINT, RG64_SFLOAT,
		RGB64_UINT, RGB64_SINT, RGB64_SFLOAT,
		RGBA64_UINT, RGBA64_SINT, RGBA64_SFLOAT,

		// Depth formats
		D16_UNORM,
		D16_UNORM_S8_UINT,
		D24_UNORM_S8_UINT,
		D32_SFLOAT,
		D32_SFLOAT_S8_UINT,

	};

	enum class ImageLayout : uint8
	{
		Undefined = 0,
		Present,
		ColorAttachment, DepthStencilAttachment,
		ShaderReadOnly,
		TransferSrc, TransferDst,
	};

	enum class ImageUsageFlags : uint32
	{
		None = 0,
		TransferSrc						= BIT(0),
		TransferDst						= BIT(1),
		Sampled							= BIT(2),
		Storage							= BIT(3),
		ColorAttachment					= BIT(4),
		DepthStencilAttachment			= BIT(5),
		TransientAttachment				= BIT(6),
		InputAttachment					= BIT(7),
		HostTransfer					= BIT(8),
		VideoDecodeDst					= BIT(9),
		VideoDecodeSrc					= BIT(10),
		VideoDecodeDpb					= BIT(11),
	};

	struct TextureSpecification
	{
		void* Data;
		SIZE_T DataSize;
		Format Format;
		ImageUsageFlags Usage;
		Pawn::Core::Containers::AnsiString DebugName;

		bool bOwnsImage = false;
	};

	struct Texture2DSpecification : TextureSpecification
	{
		Pawn::Core::Math::Resolution2D<uint32> Resolution;
	};

	class PAWN_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual bool IsLoaded() const = 0;

		virtual void SetData(void* data, SIZE_T size) = 0;

		virtual void* GetRawData() = 0;
		virtual SIZE_T GetRawDataSize() = 0;

		virtual Pawn::Core::Containers::StringView GetTexturePath() = 0;
		virtual Pawn::Core::Containers::AnsiStringView GetDebugName() = 0;
	};

	class PAWN_API Texture2D : public Texture
	{
	public:
		virtual Pawn::Render::Texture2DSpecification& GetSpecification() = 0;

		virtual Pawn::Core::Math::Resolution2D<uint32> GetResolution() const = 0;

	public:
		static Texture2D* Create(Texture2DSpecification& specification);

	private:
		static Texture2D* CreateVulkanTexture(Texture2DSpecification& specification);

	};
}
