#pragma once

#include <Core/Containers/String.h>

namespace Pawn::Render
{
	class PAWN_API Shader
	{
	public:
		enum class Type
		{
			None = 0,
			Vertex, Pixel, Compute, Geometry, Hull, Domain
		};

		struct CompiledShader
		{
			void* ShaderPtr;
			SIZE_T ShaderSize;
		};

	public:
		virtual ~Shader() {};

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual Shader::Type GetShaderType() const = 0;

	public:
		static Shader* CreateShader(const uchar* fileName, Shader::Type type, bool compiled);

	public:
		static Shader* CreateCompiledShader(const uchar* fileName, Shader::Type type);
		static Shader* CreateAndCompileShader(const uchar* fileName, Shader::Type type);

	// Compiled
	public:
		static Shader* CreateCompiledDirectX11Shader(const uchar* fileName, Shader::Type type);

	// Read then compile
	public:
		static Shader* CreateAndCompileDirectX11Shader(const uchar* fileName, Shader::Type type);

	public:
		static void SetShaderSourceExtension(const uchar* extension)
		{
			s_ShaderSourceExtension = extension;
		}

		static const uchar* GetShaderSourceExtension()
		{
			return s_ShaderSourceExtension;
		}

	public:
		static void SetCompiledShaderExtension(const uchar* extension)
		{
			s_CompiledShaderExtension = extension;
		}

		static const uchar* GetCompiledShaderExtension()
		{
			return s_CompiledShaderExtension;
		}

	private:
		static const uchar* s_ShaderSourceExtension;
		static const uchar* s_CompiledShaderExtension;

	};
}

